#ifndef PROJECT_CSIM_CACHECONTROLLER_H
#define PROJECT_CSIM_CACHECONTROLLER_H


#include <string>
#include "Response.h"
#include "Request.h"
#include "CacheSet.h"
#include <string>

class NodeController {
private:
    virtual void transitCacheLineStateOnRequest(CacheLine &cacheLine, long cacheAddress, Request request) = 0;
//    virtual void transitCacheLineStateOnOperation(CacheLine &cacheLine, long cacheAddress, std::string operation);
public:
    virtual void runCacheOp(long address, std::string operation, int timeStamp) = 0;
//    virtual Response requestHandler(Request request) = 0;

    virtual Response requestHandler(Request request, int sourceID) = 0;

    // directory function
    virtual void evictionHandler(int sourceID, long setID, long tag, int token) = 0;
};


#endif //PROJECT_CSIM_CACHECONTROLLER_H
