#include "Interconnection.h"

Interconnection::Interconnection(TOPO topo, int nodeNum) {
    initializeLatencyMatrix(latencyMatrix, topo, nodeNum);
}

int Interconnection::sendRequest(int sourceID, int receiverID, Request request, Response & response) {
    int latency;
    // If receiver is special (used for directory node)
    if (receiverID == -1) {
        receiverID = cacheVector.size() - 1;
        latency = 1;
    } else {
        latency = getLatency(sourceID, receiverID);
    }

    response = cacheVector[receiverID].requestHandler(request);
    return latency;
}

int Interconnection::getLatency(int nodeID1, int nodeID2) {
    return latencyMatrix[nodeID1][nodeID2];
}