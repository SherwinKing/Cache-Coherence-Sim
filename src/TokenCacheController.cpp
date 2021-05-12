#include "TokenCacheController.h"
#include "CacheSet.h"

void TokenCacheController::transitCacheLineStateOnRequest(CacheLine &cacheLine, long cacheAddress, Request request) {
    // If cache line is dirty, and loose write right, flush
    if (cacheLine.coherenceState.tokenNum == TOTAL_PROC_NUM && cacheLine.dirtyBit) {
        statistics.cacheFlush(processorID, cacheAddress);
    }

    // Gives one token if this cache line has more than one tokens. Otherwise don't give
    if (request.getRequestType() == TOKEN_REQUEST_R) {
        if (cacheLine.coherenceState.tokenNum > 1) {
            cacheLine.coherenceState.tokenNum--;
        }
    } else {
        assert((request.getRequestType() == TOKEN_REQUEST_W));
        cacheLine.coherenceState.tokenNum = 0;
        statistics.cacheInvalidate(processorID, cacheAddress);
    }
}

void
TokenCacheController::transitCacheLineStateOnOperation(CacheLine &cacheLine, long cacheAddress, std::string operation) {
    if (operation == "L") {
        if (cacheLine.coherenceState.tokenNum == 0) {
            // Broadcast request
            RequestType requestType = TOKEN_REQUEST_R;
            Request request(requestType, coherenceType, cacheAddress);
            std::vector<Response> responseVector;
            interconnection.broadcastRequest(processorID, request, responseVector);

            // Add received tokens
            int receivedTokenNum = 0;
            for (Response response : responseVector) {
                receivedTokenNum += response.getTokenNum();
            }
            cacheLine.coherenceState.tokenNum += receivedTokenNum;

        } else {
            assert(cacheLine.coherenceState.tokenNum > 0);
            // Do nothing
        }
    } else {    // operation == "S"
        if (cacheLine.coherenceState.tokenNum < TOTAL_PROC_NUM) {
            // Broadcast request
            RequestType requestType = TOKEN_REQUEST_W;
            Request request(requestType, coherenceType, cacheAddress);
            std::vector<Response> responseVector;
            interconnection.broadcastRequest(processorID, request, responseVector);

            // Add received tokens
            int receivedTokenNum = 0;
            for (Response response : responseVector) {
                receivedTokenNum += response.getTokenNum();
            }
            cacheLine.coherenceState.tokenNum += receivedTokenNum;
//            if (cacheLine.coherenceState.tokenNum != TOTAL_PROC_NUM){
//                std::cout << cacheLine.coherenceState.tokenNum;
//            }
            assert(cacheLine.coherenceState.tokenNum == TOTAL_PROC_NUM);
        } else {
            assert(cacheLine.coherenceState.tokenNum == TOTAL_PROC_NUM);
            // Do nothing
        }
    }
}

void
TokenCacheController::updateCacheLine(CacheLine &cacheLineToUpdate, long newTag, std::string operation, int timeStamp) {
    cacheLineToUpdate.tag = newTag;
    cacheLineToUpdate.isEmpty = 0;
    cacheLineToUpdate.lastUseTimestamp = timeStamp;
    cacheLineToUpdate.dirtyBit = (operation == "S") ? 1 : 0;
}

TokenCacheController::TokenCacheController(int s, int E, int b, int processorID, Interconnection &interconnection,
                                           Statistics &statistics)  : s(s), E(E), b(b),
                                                                      coherenceType(TOKEN),
                                                                      processorID(processorID),
                                                                      interconnection(interconnection),
                                                                      cache(processorID, s, E, b),
                                                                      statistics(statistics) {
    setIDMask = ((1<<s) - 1) << b;
    tagMask = ~((1<<(s+b))-1);
}

void TokenCacheController::runCacheOp(long address, std::string operation, int timeStamp) {
    long setID = (address & setIDMask) >> b;
    long tag = (address & tagMask) >> (s+b);

    CacheLine * cacheLinePtr;

    // Cache found
    if (cache.ifCacheLinePresent(setID, tag)) {
        cacheLinePtr = & cache.getCacheLine(setID, tag);
        if (cacheLinePtr->coherenceState.tokenNum == 0) {
            // Coherence miss
            statistics.cacheMiss(processorID, address);
        } else {
            // Cache hit
            statistics.cacheHit(processorID, address);
        }
    }
        // Cache miss
    else {
        statistics.cacheMiss(processorID, address);

        // Find an empty cache block, or evict one when necessary. Then update the cache line.
        cacheLinePtr = & cache.findCacheBlockToReplace(setID);
        // Check if the cache line is to be evicted
        if (!cacheLinePtr->isEmpty) {
            statistics.cacheEvict(processorID, cacheLinePtr->setID, cacheLinePtr->tag);
            // TODO: Where do the tokens go?
            cacheLinePtr->isEmpty = 1;
            cacheLinePtr->coherenceState.tokenNum = TOTAL_PROC_NUM;
        }
    }
    updateCacheLine(*cacheLinePtr, tag, operation, timeStamp);
    transitCacheLineStateOnOperation(*cacheLinePtr, address, operation);
}

Response TokenCacheController::requestHandler(Request request, int sourceID) {
    int responseTokenNum = 0;

    long address = request.getRequestAddress();
    long setID = (address & setIDMask) >> b;
    long tag = (address & tagMask) >> (s+b);
    if (cache.ifCacheLinePresent(setID, tag)) {
        CacheLine & cacheLine = cache.getCacheLine(setID, tag);

        // Gives one token if this cache line has more than one tokens. Otherwise don't give
        if (request.getRequestType() == TOKEN_REQUEST_R) {
            if (cacheLine.coherenceState.tokenNum <= 1) {
                responseTokenNum = 0;
            } else {
                responseTokenNum = 1;
            }
        } else {
            assert((request.getRequestType() == TOKEN_REQUEST_W));
            responseTokenNum = cacheLine.coherenceState.tokenNum;
        }

        // Change the token num of this cache
        transitCacheLineStateOnRequest(cacheLine, request.getRequestAddress(), request);
    }
    Response response = Response(TOKEN_RESPONSE);
    response.setTokenNum(responseTokenNum);
    return response;
}

void TokenCacheController::evictionHandler(int sourceID, long setID, long tag, int token) {
    return;
}
