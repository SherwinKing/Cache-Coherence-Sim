#ifndef PROJECT_CSIM_CACHECONTROLLER_H
#define PROJECT_CSIM_CACHECONTROLLER_H


#include "Response.h"
#include "Request.h"

class CacheController {
public:
    virtual void runCacheOp(long tag, long setInd, char operation);
    virtual Response requestHandler(Request request);
};


#endif //PROJECT_CSIM_CACHECONTROLLER_H
