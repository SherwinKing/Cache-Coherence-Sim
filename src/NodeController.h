#ifndef PROJECT_CSIM_CACHECONTROLLER_H
#define PROJECT_CSIM_CACHECONTROLLER_H


#include "Response.h"
#include "Request.h"
#include "CacheSet.h"
#include <string>

class NodeController {
private:
    virtual void transitCacheLineState(CacheLine &cacheLine, long cacheAddress, Request request) = 0;
public:
    virtual void runCacheOp(long address, std::string operation, int timeStamp) = 0;
    virtual Response requestHandler(Request request, int sourceID) = 0;
    // directory function
    virtual void evictionHandler(int sourceID, long setID, long tag, int token) = 0;

    virtual ~NodeController() {}
};


#endif //PROJECT_CSIM_CACHECONTROLLER_H
