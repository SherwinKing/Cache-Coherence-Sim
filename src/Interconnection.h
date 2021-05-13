#ifndef PROJECT_CSIM_INTERCONNECTION_H
#define PROJECT_CSIM_INTERCONNECTION_H

#define HOP_TO_HOP_LATENCY 5

#include <vector>
#include "Response.h"
#include "Request.h"
#include "NodeController.h"
#include "Topo.h"

class Interconnection {
private:
    std::vector<std::vector<int>> latencyMatrix;
    std::vector<std::shared_ptr<NodeController>> nodeControllerSmartPointerVector;

    Statistics & statistics;

//    void setLatency(int i, int j, int latency);
    int getLatency(int nodeID1, int nodeID2);
public:
    Interconnection(TOPO topo, std::vector<std::shared_ptr<NodeController>> &nodeVector, Statistics & statistics);

    Response sendRequest(int sourceID, int receiverID, Request request, int & latency);
    int broadcastRequest(int sourceID, Request request, std::vector<Response> & responseVector);
    void sendEviction(int sourceID, long setID, long tag, int content);
};


#endif //PROJECT_CSIM_INTERCONNECTION_H
