#include "Interconnection.h"

int Interconnection::sendRequest(int sourceID, int receiverID, Request request, Response & response) {
    response = cacheVector[receiverID].requestHandler(request);
    return getLatency(sourceID, receiverID);
}

int Interconnection::getLatency(int nodeID1, int nodeID2) {
    return -1;
}