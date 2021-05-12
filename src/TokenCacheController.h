#ifndef PROJECT_CSIM_TOKENCACHECONTROLLER_H
#define PROJECT_CSIM_TOKENCACHECONTROLLER_H

#include <string>
#include "NodeController.h"
#include "Cache.h"
#include "Interconnection.h"
#include "Statistics.h"
#include "Response.h"
#include "Request.h"

class TokenCacheController: public NodeController {
private:
    int id;
    int s;  // Number of set index bits (2^s is the cache size, or the number of sets in cache)
    int E;  // Associativity
    int b;  // Number of block bits

    long setIDMask;
    long tagMask;

    const CoherenceType coherenceType = TOKEN;
    int processorID;
    Cache cache;
    Interconnection & interconnection;
    Statistics & statistics;

    void transitCacheLineStateOnRequest(CacheLine &cacheLine, long cacheAddress, Request request) override ;

    void transitCacheLineStateOnOperation(CacheLine &cacheLine, long cacheAddress, std::string operation) override ;

    void updateCacheLine(CacheLine &cacheLineToUpdate, long newTag, std::string operation, int timeStamp);

public:
    TokenCacheController(int s, int E, int b, int processorID,
                            Interconnection &interconnection, Statistics &statistics);

    void runCacheOp(long address, std::string operation, int timeStamp) override;

    Response requestHandler(Request request, int sourceID) override;

    void evictionHandler(int sourceID, long setID, long tag, int token) override;
};


#endif //PROJECT_CSIM_TOKENCACHECONTROLLER_H
