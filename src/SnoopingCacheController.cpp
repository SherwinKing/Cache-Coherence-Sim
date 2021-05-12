#include "SnoopingCacheController.h"
#include "CacheSet.h"

void SnoopingCacheController::updateCacheLine(CacheLine &cacheLineToUpdate, long newTag, std::string operation,
                                              int timeStamp) {
    cacheLineToUpdate.tag = newTag;
    //std::cerr << "updateCacheLine << cacheLineToUpdate.tag: " << cacheLineToUpdate.tag << std::endl;
    //std::cerr << "in updateCacheLine, address: " << &cacheLineToUpdate;
    cacheLineToUpdate.isEmpty = 0;
    cacheLineToUpdate.lastUseTimestamp = timeStamp;
    cacheLineToUpdate.dirtyBit = (operation == "S") ? 1 : 0;
}


void SnoopingCacheController::runCacheOp(long address, std::string operation, int timeStamp) {

    long setID = (address & setIDMask) >> b;
    long tag = (address & tagMask) >> (s+b);

    CacheLine * cacheLinePtr;

    // Cache hit
    if (cache.ifCacheLinePresent(setID, tag)) {
        statistics.cacheHit(processorID, address);

        cacheLinePtr = & cache.getCacheLine(setID, tag);
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
                //printf("Cache[%d] in S   |   ", processorID);
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
            cacheLine.isEmpty = 1;
            break;
        default:
            throw "Invalid state: the request type is not supported!";
    }
}


void SnoopingCacheController::transitCacheLineStateOnOperation(CacheLine &cacheLine, long cacheAddress, std::string operation) {

    //printf("\n*********************\n");
    if (operation == "L") {
        //printf("Cache[%d] has L operation\n", processorID);
        switch (cacheLine.coherenceState.mesiState) {
            case MESIState::I: {
                // Broadcast request
                RequestType requestType = BusRd;
                Request request(requestType, coherenceType, cacheAddress);
                std::vector<Response> responseVector;
                interconnection.broadcastRequest(processorID, request, responseVector);

                // Check if no other cache asserts shared
                int ifNoOtherShared = 1;
                for (int i = 0; i < responseVector.size(); i++) {
                    if (responseVector[i].getResponseType() == ACK_SHARED)
                        ifNoOtherShared = 0;
                }
                if (ifNoOtherShared == 1){
                    //printf("Cache[%d] in E   |   ", processorID);
                    cacheLine.coherenceState.mesiState = MESIState::E;
                }
                else{
                    //printf("Cache[%d] in S   |   ", processorID);
                    cacheLine.coherenceState.mesiState = MESIState::S;
                }
                break;
            }
            case MESIState::S:
                //printf("Cache[%d] in S   |   ", processorID);
                //break;
            case MESIState::E:
                //printf("Cache[%d] in E   |   ", processorID);
                //break;
            case MESIState::M:
                //printf("Cache[%d] in M   |   ", processorID);
                // No action
                break;
            default:
                throw "Illegal state: the mesi state is illegal!";
        }
    } else {    // operation == "S"
        //printf("Cache[%d] has S operation\n", processorID);
        switch (cacheLine.coherenceState.mesiState) {
            case MESIState::I:
            case MESIState::S: {
                // Broadcast request
                RequestType requestType = BusRdX;
                Request request(requestType, coherenceType, cacheAddress);
                std::vector<Response> responseVector;
                interconnection.broadcastRequest(processorID, request, responseVector);
                break;
            }
            case MESIState::E:
            case MESIState::M:
                // No action
                break;
            default:
                throw "Illegal state: the mesi state is illegal!";
        }
        //printf("Cache[%d] in M   |   ", processorID);
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

