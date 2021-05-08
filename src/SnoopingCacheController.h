
#ifndef PROJECT_CSIM_SNOOPINGCACHECONTROLLER_H
#define PROJECT_CSIM_SNOOPINGCACHECONTROLLER_H


#include "CacheController.h"
#include "Cache.h"
#include "Interconnection.h"
#include "Statistics.h"
#include "Response.h"
#include "Request.h"

class SnoopingCacheController: public CacheController {
private:
    Cache cache;
    Interconnection interconnection;
    Statistics statistics;

public:
    void runCacheOp(long tag, long setInd, char operation) override;

    Response requestHandler(Request request) override;
    
};


#endif //PROJECT_CSIM_SNOOPINGCACHECONTROLLER_H
