
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
    int id;
    int s;  // Number of set index bits (2^s is the cache size, or the number of sets in cache)
    int E;  // Associativity
    int b;  // Number of block bits

    long setIndMask;
    long tagMask;

    const CoherenceType coherenceType = SNOOPING;
    int processorID;
    Cache cache;
    Interconnection interconnection;
    Statistics & statistics;

    void transitCacheLineState(CacheLine &cacheLine, long cacheAddress, Request request) override;

    void updateCacheLine(CacheLine &cacheLineToUpdate, long newTag, std::string operation, int timeStamp);

public:
    void runCacheOp(long address, std::string operation, int timeStamp) override;

    Response requestHandler(Request request) override;

};


#endif //PROJECT_CSIM_SNOOPINGCACHECONTROLLER_H
