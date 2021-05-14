#include "DirectoryCacheController.h"
#include "CacheSet.h"

void DirectoryCacheController::updateCacheLine(CacheLine &cacheLineToUpdate, long newTag, std::string operation,
                                              int timeStamp) {
    cacheLineToUpdate.tag = newTag;
    //std::cerr << "updateCacheLine << cacheLineToUpdate.tag: " << cacheLineToUpdate.tag << std::endl;
    //std::cerr << "in updateCacheLine, address: " << &cacheLineToUpdate;
    cacheLineToUpdate.isEmpty = 0;
    cacheLineToUpdate.lastUseTimestamp = timeStamp;
    cacheLineToUpdate.dirtyBit = (operation == "S") ? 1 : 0;
}


void DirectoryCacheController::runCacheOp(long address, std::string operation, int timeStamp) {

    long setID = (address & setIDMask) >> b;
    long tag = (address & tagMask) >> (s+b);

    CacheLine * cacheLinePtr;

    // Cache hit
    if (cache.ifCacheLinePresent(setID, tag)) {
        cacheLinePtr = & cache.getCacheLine(setID, tag);
        if(cacheLinePtr->coherenceState.mesiState == I)
            statistics.cacheMiss(processorID, address);
        else
            statistics.cacheHit(processorID, address);
    }
    // Cache miss
    else {
        statistics.cacheMiss(processorID, address);

        // Find an empty cache block, or evict one when necessary. Then update the cache line.
        cacheLinePtr = & cache.findCacheBlockToReplace(setID);
        // Check if the cache line is to be evicted
        if (!cacheLinePtr->isEmpty) {
            statistics.cacheEvict(processorID, cacheLinePtr->setID, cacheLinePtr->tag);
            // TODO send message to directory
            interconnection.sendEviction(processorID, setID, tag, 0);
            //cacheLinePtr->isEmpty = 1;
            // initialize this line to I state
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
Response DirectoryCacheController::requestHandler(Request request, int sourceID) {
    Response response = Response(ACK_SHARED);
    long address = request.getRequestAddress();
    long setID = (address & setIDMask) >> b;
    long tag = (address & tagMask) >> (s+b);
    // cache must have the line in order to receive the request
    assert(cache.ifCacheLinePresent(setID, tag));
    CacheLine & cacheLine = cache.getCacheLine(setID, tag);
    // cache line must not be in I state
    assert(cacheLine.coherenceState.mesiState != I);
    transitCacheLineStateOnRequest(cacheLine, request.getRequestAddress(), request);
    return response;

    /*
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
    */
}

void DirectoryCacheController::transitCacheLineStateOnRequest(CacheLine &cacheLine, long cacheAddress, Request request) {
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
            //cacheLine.isEmpty = 1;
            break;
        default:
            throw "Invalid state: the request type is not supported!";
    }
}


void DirectoryCacheController::transitCacheLineStateOnOperation(CacheLine &cacheLine, long cacheAddress, std::string operation) {

    if (operation == "L") {
        switch (cacheLine.coherenceState.mesiState) {
            case MESIState::I: {
                // send request to directory first
                int directoryLatency = 0;
                int cacheLatency = 0;
                int cacheLatencyMx = 0;
                RequestType directoryRequestType = DIRECTORY_REQUEST_R;
                Request directoryRequest(directoryRequestType, coherenceType, cacheAddress);
                Response directoryResponse = interconnection.sendRequest(processorID, -1, directoryRequest, directoryLatency);
                // if empty cache owner id, go to state E, S otherwise
                if(directoryResponse.getCacheOwnerIDs().size() == 0){
                    cacheLine.coherenceState.mesiState = MESIState::E;
                }
                else{
                    cacheLine.coherenceState.mesiState = MESIState::S;
                    RequestType cacheRequestType = BusRd;
                    Request cacheRequest(cacheRequestType, coherenceType, cacheAddress);
                    for(auto ownerID : directoryResponse.getCacheOwnerIDs()){
                        Response cacheResponse = interconnection.sendRequest(processorID, ownerID, cacheRequest, cacheLatency);
                        // TODO need to calculate the latency somehow
                        cacheLatencyMx = (cacheLatencyMx<cacheLatency) ? cacheLatency : cacheLatencyMx;         
                    }
                }
                statistics.addLatency(processorID, directoryLatency+cacheLatencyMx);

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
                // send request to directory first
                int directoryLatency = 0;
                int cacheLatency = 0;
                int cacheLatencyMx = 0;
                RequestType directoryRequestType = DIRECTORY_REQUEST_W;
                Request directoryRequest(directoryRequestType, coherenceType, cacheAddress);
                Response directoryResponse = interconnection.sendRequest(processorID, -1, directoryRequest, directoryLatency);
                // invalidata all other caches having this line
                RequestType cacheRequestType = BusRdX;
                Request cacheRequest(cacheRequestType, coherenceType, cacheAddress);
                for(auto ownerID : directoryResponse.getCacheOwnerIDs()){
                    Response cacheResponse = interconnection.sendRequest(processorID, ownerID, cacheRequest, cacheLatency);
                    // TODO
                    cacheLatencyMx = (cacheLatencyMx<cacheLatency) ? cacheLatency : cacheLatencyMx;
                }
                statistics.addLatency(processorID, directoryLatency+cacheLatencyMx);

                break;
                /*
                // Broadcast request
                RequestType requestType = BusRdX;
                Request request(requestType, coherenceType, cacheAddress);
                std::vector<Response> responseVector;
                interconnection.broadcastRequest(processorID, request, responseVector);
                break;
                */
            }
            case MESIState::E: {
                // send request to directory only for updating dirty bit
                int directoryLatency = 0;
                int cacheLatency = 0;
                RequestType directoryRequestType = DIRECTORY_REQUEST_W;
                Request directoryRequest(directoryRequestType, coherenceType, cacheAddress);
                Response directoryResponse = interconnection.sendRequest(processorID, -1, directoryRequest, directoryLatency);
                // NO other cache will have this line
                assert(directoryResponse.getCacheOwnerIDs().size() == 0);
            }
            case MESIState::M:
                // No action
                break;
            default:
                throw "Illegal state: the mesi state is illegal!";
        }
        cacheLine.coherenceState.mesiState = MESIState::M;
    }
}

DirectoryCacheController::DirectoryCacheController(int s, int E, int b,
                                                 int processorID, Interconnection &interconnection,
                                                 Statistics &statistics) : s(s), E(E), b(b),
                                                                           coherenceType(DIRECTORY),
                                                                           processorID(processorID),
                                                                           interconnection(interconnection),
                                                                           cache(processorID, s, E, b),
                                                                           statistics(statistics) {
    // The way to get the mask is the same as 15213 Cache Lab implementation
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

void DirectoryCacheController::evictionHandler(int sourceID, long setID, long tag, int token) {
    return;
}

