#ifndef PROJECT_CSIM_DIRECTORY_H
#define PROJECT_CSIM_DIRECTORY_H

#include <cassert>
#include <unordered_map>
#include <vector>
#include "NodeController.h"
#include "Response.h"
#include "Request.h"


// information about all datalines in caches
struct DirectoryLine {
    DirectoryLine(int numCaches)
        : dirty_(false) {
            presence_.resize(numCaches, false);
        }
    bool dirty_;
    std::vector<bool> presence_;
};



class Directory: public NodeController {
public:
    
    Directory(int numCaches, int b, int s)
        : nodeID_(-1),
          numCaches_(numCaches),
          b_(b),
          s_(s) {}

    virtual ~Directory() {
        for(auto& i : directory_)
            delete i.second;
    }

    Response requestHandler(Request request, int sourceID) override;
    void evictionHandler(int sourceID, long setID, long tag, int token) override;
    
    // will not use these
    void runCacheOp(long address, std::string operation, int timeStamp) override{}
    void transitCacheLineState(CacheLine &cacheLine, long cacheAddress, Request request) override {}

private:
    
    int nodeID_;    // directory has id of -1
    int numCaches_; 
    int b_;         // number of cache line bits
    int s_;         // number of set bits

    bool getLine(DirectoryLine** line_ptr, long lineID);
    long getLineID(long address);
    long getLineID(long setID, long tag);
    std::unordered_map<long, DirectoryLine*> directory_;

};


#endif  //PROJECT_CSIM_DIRECTORY_H
