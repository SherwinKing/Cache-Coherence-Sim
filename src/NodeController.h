#ifndef PROJECT_CSIM_CACHECONTROLLER_H
#define PROJECT_CSIM_CACHECONTROLLER_H


#include "Response.h"
#include "Request.h"
#include "CacheSet.h"

class NodeController {
private:
    virtual void transitCacheLineState(CacheLine &cacheLine, long cacheAddress, Request request);
public:
    virtual void runCacheOp(long address, std::string operation, int timeStamp);
    virtual Response requestHandler(Request request);
};


#endif //PROJECT_CSIM_CACHECONTROLLER_H
