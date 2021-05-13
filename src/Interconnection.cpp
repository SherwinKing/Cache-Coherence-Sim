#include "Interconnection.h"

Interconnection::Interconnection(TOPO topo, Statistics & statisticsRef) :
        statistics(statisticsRef){
    this->topo = topo;
//    this->nodeControllerSmartPointerVector = nodeControllerSmartPointerVector;

}

Response Interconnection::sendRequest(int sourceID, int receiverID, Request request, int & latency) {
    // If receiver is special (used for directory node)
    if (receiverID == -1) {
        receiverID = nodeControllerSmartPointerVector.size() - 1;
        latency = 2 * HOP_TO_HOP_LATENCY;
    } else {
        latency = 2 * HOP_TO_HOP_LATENCY * getLatency(sourceID, receiverID);
    }

    std::cerr << "addCommunicationCount" << std::endl;
    statistics.addCommunicationCount(2);

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

//    statistics.addCommunicationCount(2 * responseVector.size());

    return  totalLatency;
}

void Interconnection::setNodeControllerSmartPointerVector(
        const std::vector<std::shared_ptr<NodeController>> &nodeVector) {
    Interconnection::nodeControllerSmartPointerVector = nodeVector;
    initializeLatencyMatrix(latencyMatrix, topo, nodeVector.size());
}
