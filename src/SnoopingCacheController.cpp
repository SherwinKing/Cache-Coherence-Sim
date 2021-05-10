#include <string>
#include "SnoopingCacheController.h"
#include "CacheSet.h"

void SnoopingCacheController::updateCacheLine(CacheLine &cacheLineToUpdate, long newTag, std::string operation,
                                              int timeStamp) {
    cacheLineToUpdate.tag = newTag;
    cacheLineToUpdate.isEmpty = 0;
    cacheLineToUpdate.lastUseTimestamp = timeStamp;
    cacheLineToUpdate.dirtyBit = (operation == "S") ? 1 : 0;
}

void SnoopingCacheController::runCacheOp(long address, std::string operation, int timeStamp) {
    long setID = (address & setIndMask) >> b;
    long tag = (address & tagMask) >> (s+b);

    // Cache hit
    if (cache.ifCacheLinePresent(setID, tag)) {
        statistics.cacheHit(processorID, address);
        CacheLine & requestedCache = cache.getCacheLine(setID, tag);
        updateCacheLine(requestedCache, tag, operation, timeStamp);
    }
    // Cache miss
    else {
        statistics.cacheMiss(processorID, address);
        RequestType requestType = (operation == "L") ? BusRd : BusRdX;
        Request request(requestType, coherenceType, address);

        // Broadcast request
        std::vector<Response> responseVector;
        interconnection.broadcastRequest(processorID, request, responseVector);

        // Find an empty cache block, or evict one when necessary. Then update the cache line.
        CacheLine & cacheLineToReplace = cache.findCacheBlockToReplace(setID);
        // Check if the cache line is to be evicted
        if (!cacheLineToReplace.isEmpty) {
            statistics.cacheEvict(processorID, cacheLineToReplace.setID, cacheLineToReplace.tag);
            cacheLineToReplace.isEmpty = 1;
        }

        // Update the chosen cache block
        updateCacheLine(cacheLineToReplace, tag, operation, timeStamp);
    }
    return;
}

/**
 * Cache hears requests on bus.
 * @param request
 * @return
 */
Response SnoopingCacheController::requestHandler(Request request) {
    Response response = Response(ACK);
    long address = request.getRequestAddress();
    long setInd = (address & setIndMask) >> b;
    long tag = (address & tagMask) >> (s+b);
    if (cache.ifCacheLinePresent(setInd, tag)) {
        CacheLine & cacheLine = cache.getCacheLine(request.getRequestAddress(), 0);
        transitCacheLineState(cacheLine, request.getRequestAddress(), request);
    }
    return response;
}

void SnoopingCacheController::transitCacheLineState(CacheLine &cacheLine, long cacheAddress, Request request) {
    switch(request.getRequestType()) {
        case BusRd:
            if (cacheLine.coherenceState.mesiState == M) {
                statistics.cacheFlush(processorID, cacheAddress);
            }
            cacheLine.coherenceState.mesiState = S;
            break;
        case BusRdX:
            if (cacheLine.coherenceState.mesiState == M) {
                statistics.cacheInvalidate(processorID, cacheAddress);
                statistics.cacheFlush(processorID, cacheAddress);
            }
            cacheLine.coherenceState.mesiState = I;
            cacheLine.isEmpty = 1;
            break;
        default:
            throw "Invalid state: the request type is not supported!";
    }
}
