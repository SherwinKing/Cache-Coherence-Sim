#include "Directory.h"

long Directory::getLineID(long address){
    // effectively tag+set
    return address >> b_;
}

long Directory::getLineID(long setID, long tag){
    return setID | tag << s_;
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
    if(mode_ == DIRECTORY)
        return requestHandler_DIREC(request, sourceID);
    if(mode_ == TOKEN)
        return requestHandler_TOKEN(request, sourceID);
}

void Directory::evictionHandler(int sourceID, long setID, long tag, int token){
    if(mode_ == DIRECTORY)
        evictionHandler_DIREC(sourceID, setID, tag, token);
    if(mode_ == TOKEN)
        evictionHandler_TOKEN(sourceID, setID, tag, token);
}

Response Directory::requestHandler_TOKEN(Request request, int sourceID){
    Response response(TOKEN_RESPONSE);

    assert(request.getRequestType() == TOKEN_REQUEST_R ||
           request.getRequestType() == TOKEN_REQUEST_W);
    assert(request.getCoherenceType() == TOKEN);

    long address = request.getRequestAddress();
    long lineID = getLineID(address);

    DirectoryLine* line;
    bool if_exist = getLine(&line, lineID);

    // add memory fetch latency
    if(!if_exist)
        stat_.addLatency(-1, 200);

    if(request.getRequestType() == TOKEN_REQUEST_R){
        if(line->token_pool_ > 0){
            line->token_pool_--;
            response.setTokenNum(1);
        }
        else
            response.setTokenNum(0);
    }
    if(request.getRequestType() == TOKEN_REQUEST_W){
        if(line->token_pool_ > 0){
            response.setTokenNum(line->token_pool_);
            line->token_pool_ = 0;
        }
        else
            response.setTokenNum(0);
    }

    return response;
}


void Directory::evictionHandler_TOKEN(int sourceID, long setID, long tag, int token){
    long lineID = getLineID(setID, tag);
    DirectoryLine* line;
    bool if_exist = getLine(&line, lineID);
    assert(if_exist);
    assert(token >= 0);
    
    line->token_pool_ += token;
    if(line->token_pool_ == numCaches_){
        directory_.erase(lineID);
        delete line;
    }
}

Response Directory::requestHandler_DIREC(Request request, int sourceID){
    Response response(DIRECTORY_RESPONSE);

    assert(request.getRequestType() == DIRECTORY_REQUEST_R ||
           request.getRequestType() == DIRECTORY_REQUEST_W);
    assert(request.getCoherenceType() == DIRECTORY);

    long address = request.getRequestAddress();
    long lineID = getLineID(address);
    

    DirectoryLine* line;
    bool if_exist = getLine(&line, lineID);

    // memory fetch latency
    if(!if_exist)
        stat_.addLatency(-1, 200);

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
            std::list<int> cacheOwnerIDs;
            for(int i=0; i<numCaches_; i++){
                if(line->presence_[i])
                    cacheOwnerIDs.push_back(i);
            }
            line->presence_[sourceID] = true;
            response.setCacheOwnerIDs(cacheOwnerIDs);
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


void Directory::evictionHandler_DIREC(int sourceID, long setID, long tag, int token){
    long lineID = getLineID(setID, tag);
    DirectoryLine* line;
    bool if_exist = getLine(&line, lineID);
    assert(if_exist);
    // evicted line might be invalid
    //assert(line->presence_[sourceID]);

    line->presence_[sourceID] = false;
    if(line->dirty_){
        for(auto p : line->presence_)
            assert(!p);
        directory_.erase(lineID);
        delete line;
    }
    else{
        bool any_valid = false;
        for(auto p : line->presence_)
            if(p)   any_valid = true;
        if(!any_valid){
            directory_.erase(lineID);
            delete line;
        }
    }


}


