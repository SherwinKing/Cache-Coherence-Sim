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

    long setID = (address & setIDMask) >> b;
    long tag = (address & tagMask) >> (s+b);

    CacheLine * cacheLinePtr;

    // Cache found
    if (cache.ifCacheLinePresent(setID, tag)) {
        cacheLinePtr = & cache.getCacheLine(setID, tag);
        if (cacheLinePtr->coherenceState.mesiState == I) {
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
            cacheLinePtr->isEmpty = 1;
            // initlizate cache line to I state
            cacheLinePtr->coherenceState.mesiState = I;
        }
    }


    updateCacheLine(*cacheLinePtr, tag, operation, timeStamp);
    transitCacheLineStateOnOperation(*cacheLinePtr, address, operation);
}

/**
 * Cache hears requests on bus.
 * @param request
 * @return
 */
Response SnoopingCacheController::requestHandler(Request request, int sourceID) {
    Response response = Response(ACK_NOT_SHARED);
    long address = request.getRequestAddress();
    long setID = (address & setIDMask) >> b;
    long tag = (address & tagMask) >> (s+b);
    if (cache.ifCacheLinePresent(setID, tag)) {
        CacheLine & cacheLine = cache.getCacheLine(setID, tag);
        transitCacheLineStateOnRequest(cacheLine, request.getRequestAddress(), request);
        response.setResponseType(ACK_SHARED);
    }
    return response;
}

void SnoopingCacheController::transitCacheLineStateOnRequest(CacheLine &cacheLine, long cacheAddress, Request request) {
    switch(request.getRequestType()) {
        case BusRd:
            if (cacheLine.coherenceState.mesiState == M) {
                statistics.cacheFlush(processorID, cacheAddress);
            }
            if (cacheLine.coherenceState.mesiState != I) {
                cacheLine.coherenceState.mesiState = S;
            }
            break;
        case BusRdX:
            if (cacheLine.coherenceState.mesiState == M) {
                statistics.cacheFlush(processorID, cacheAddress);
            }
            if (cacheLine.coherenceState.mesiState != I) {
                statistics.cacheInvalidate(processorID, cacheAddress);
            }
            cacheLine.coherenceState.mesiState = I;
            break;
        default:
            throw "Invalid state: the request type is not supported!";
    }
}


void SnoopingCacheController::transitCacheLineStateOnOperation(CacheLine &cacheLine, long cacheAddress, std::string operation) {

    if (operation == "L") {
        switch (cacheLine.coherenceState.mesiState) {
            case MESIState::I: {
                // Broadcast request
                RequestType requestType = BusRd;
                Request request(requestType, coherenceType, cacheAddress);
                std::vector<Response> responseVector;
                int broadCastLatency = interconnection.broadcastRequest(processorID, request, responseVector);

                // Add interconnection latency
                statistics.addLatency(processorID, broadCastLatency);

                // Check if no other cache asserts shared
                int ifNoOtherShared = 1;
                for (int i = 0; i < responseVector.size(); i++) {
                    if (responseVector[i].getResponseType() == ACK_SHARED)
                        ifNoOtherShared = 0;
                }
                if (ifNoOtherShared == 1) {
                    cacheLine.coherenceState.mesiState = MESIState::E;
                    // Read from memory, add miss penalty latency
                    statistics.addLatency(processorID, 200);
                }
                else
                    cacheLine.coherenceState.mesiState = MESIState::S;
                break;
            }
            case MESIState::S:
            case MESIState::E:
            case MESIState::M:
                // No action
                break;
            default:
                throw "Illegal state: the mesi state is illegal!";
        }
    } else {    // operation == "S"
        switch (cacheLine.coherenceState.mesiState) {
            case MESIState::I:
            case MESIState::S: {
                // Broadcast request
                RequestType requestType = BusRdX;
                Request request(requestType, coherenceType, cacheAddress);
                std::vector<Response> responseVector;
                int broadCastLatency = interconnection.broadcastRequest(processorID, request, responseVector);

                // Add interconnection latency
                statistics.addLatency(processorID, broadCastLatency);

                // Should add miss penalty of memory read if data not from other cache (no other shared) for I state
                if (cacheLine.coherenceState.mesiState == MESIState::I) {
                    // Check if no other cache asserts shared
                    int ifNoOtherShared = 1;
                    for (int i = 0; i < responseVector.size(); i++) {
                        if (responseVector[i].getResponseType() == ACK_SHARED)
                            ifNoOtherShared = 0;
                    }
                    if (ifNoOtherShared == 1) {
                        // Read from memory, add miss penalty latency
                        statistics.addLatency(processorID, 200);
                    }
                }

                break;
            }
            case MESIState::E:
            case MESIState::M:
                // No action
                break;
            default:
                throw "Illegal state: the mesi state is illegal!";
        }
        cacheLine.coherenceState.mesiState = MESIState::M;
    }
}

SnoopingCacheController::SnoopingCacheController(int s, int E, int b,
                                                 int processorID, Interconnection &interconnection,
                                                 Statistics &statistics) : s(s), E(E), b(b),
                                                                           coherenceType(SNOOPING),
                                                                           processorID(processorID),
                                                                           interconnection(interconnection),
                                                                           cache(processorID, s, E, b),
                                                                           statistics(statistics) {
    setIDMask = ((1<<s) - 1) << b;
    tagMask = ~((1<<(s+b))-1);
//    this->s = s;
//    this->E = E;
//    this->b = b;
//    this->processorID = processorID;
//    this->interconnection = interconnection;
//    this->statistics = statistics;
//    cache = Cache(processorID, s, E, b);
}

void SnoopingCacheController::evictionHandler(int sourceID, long setID, long tag, int token) {
    return;
}

