#include "Interconnection.h"

Interconnection::Interconnection(TOPO topo, std::vector<std::shared_ptr<NodeController>> &nodeVector) :
        nodeControllerSmartPointerVector(nodeVector){
//    this->nodeControllerSmartPointerVector = nodeControllerSmartPointerVector;
    initializeLatencyMatrix(latencyMatrix, topo, nodeVector.size());
}

Response Interconnection::sendRequest(int sourceID, int receiverID, Request request, int & latency) {
    // If receiver is special (used for directory node)
    if (receiverID == -1) {
        receiverID = nodeControllerSmartPointerVector.size() - 1;
        latency = 2;
    } else {
        latency = 2 * getLatency(sourceID, receiverID);
    }

    Response response = nodeControllerSmartPointerVector[receiverID]->requestHandler(request, sourceID);
    return response;
}

int Interconnection::getLatency(int nodeID1, int nodeID2) {
    return latencyMatrix[nodeID1][nodeID2];
}

int Interconnection::broadcastRequest(int sourceID, Request request, std::vector<Response> &responseVector) {
    int totalLatency = 0;
    for (int receiverID = 0; receiverID < nodeControllerSmartPointerVector.size(); receiverID++) {
        if (receiverID == sourceID)
            continue;
        int latency;
        Response response = sendRequest(sourceID, receiverID, request, latency);

        // Update totalLatency
        totalLatency = (latency > totalLatency) ? latency : totalLatency;

        responseVector.push_back(response);
    }
    totalLatency += responseVector.size();
    return  totalLatency;
}

void Interconnection::sendEviction(int sourceID, long setID, long tag, int content) {
    nodeControllerSmartPointerVector[nodeControllerSmartPointerVector.size()-1]->evictionHandler(sourceID, setID, tag, content);
}
