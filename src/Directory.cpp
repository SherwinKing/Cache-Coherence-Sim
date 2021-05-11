#include "Directory.h"

long Directory::getLineID(long address){
    // effectively tag+set
    return address >> b_;
}

long Directory::getLineID(long setID, long tag){
    return (setID << b_) | (tag << s_+b_);
}

/**
 * @return true when line already exist, false means not in any caches
 */
bool Directory::getLine(DirectoryLine** line_ptr, long lineID){
    auto it = directory_.find(lineID);
    if(it == directory_.end()){
        *line_ptr = new DirectoryLine(numCaches_);
        directory_.insert({lineID, *line_ptr});
        return false;
    }
    *line_ptr = it->second;
    return true;
}

Response Directory::requestHandler(Request request, int sourceID){
    Response response(DIRECTORY_RESPONSE);

    assert(request.getRequestType() == DIRECTORY_REQUEST_R ||
           request.getRequestType() == DIRECTORY_REQUEST_W);
    assert(request.getCoherenceType() == DIRECTORY);

    long address = request.getRequestAddress();
    long lineID = getLineID(address);
    

    DirectoryLine* line;
    bool if_exist = getLine(&line, lineID);

    // read operation
    if(request.getRequestType() == DIRECTORY_REQUEST_R){
        // read miss means cache does not have the line
        assert(!line->presence_[sourceID]);
        if(line->dirty_){
            std::list<int> cacheOwnerID;
            for(int i=0; i<numCaches_; i++){
                if(line->presence_[i])
                    cacheOwnerID.push_back(i);
            }
            // only one cache can be on M state
            assert(cacheOwnerID.size() == 1);
            line->dirty_ = false;
            line->presence_[sourceID] = true;
            response.setCacheOwnerIDs(cacheOwnerID);
        }
        else{
            line->presence_[sourceID] = true;
            response.setCacheOwnerIDs(std::list<int>());
        }
    }
    // write operation
    if(request.getRequestType() == DIRECTORY_REQUEST_W){
        if(line->dirty_){
            std::list<int> cacheOwnerID;
            for(int i=0; i<numCaches_; i++){
                if(line->presence_[i]){
                    cacheOwnerID.push_back(i);
                    line->presence_[i] = false;
                }
            }
            assert(cacheOwnerID.size() == 1);
            line->presence_[sourceID] = true;
            response.setCacheOwnerIDs(cacheOwnerID);
        }   
        else{
            std::list<int> cacheOwnerIDs;
            for(int i=0; i<numCaches_; i++){
                if(i == sourceID)
                    continue;
                if(line->presence_[i]){
                    cacheOwnerIDs.push_back(i);
                    line->presence_[i] = false;
                }
            }
            line->dirty_ = true;
            line->presence_[sourceID] = true;
            response.setCacheOwnerIDs(cacheOwnerIDs);
        }
    }
    
    return response;
}


void Directory::evictionHandler(int sourceID, long setID, long tag, int token){
    long lineID = getLineID(setID, tag);
    DirectoryLine* line;
    bool if_exist = getLine(&line, lineID);
    assert(if_exist);
    assert(line->presence_[sourceID]);

    line->presence_[sourceID] = false;
    if(line->dirty_){
        for(auto p : line->presence_)
            assert(!p);
        directory_.erase(lineID);
    }
    else{
        bool any_valid = false;
        for(auto p : line->presence_)
            if(p)   any_valid = true;
        if(!any_valid){
            directory_.erase(lineID);
        }
    }


}


