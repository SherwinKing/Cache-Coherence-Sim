#ifndef PROJECT_CSIM_INTERCONNECTION_H
#define PROJECT_CSIM_INTERCONNECTION_H


#include <vector>
#include "Response.h"
#include "Request.h"
#include "NodeController.h"
#include "Topo.h"

class Interconnection {
private:
    std::vector<std::vector<int>> latencyMatrix;
    std::vector<std::shared_ptr<NodeController>> nodeControllerSmartPointerVector;

//    void setLatency(int i, int j, int latency);
    int getLatency(int nodeID1, int nodeID2);
public:
    Interconnection(TOPO topo, std::vector<std::shared_ptr<NodeController>> &nodeVector);

    Response sendRequest(int sourceID, int receiverID, Request request, int & latency);
    int broadcastRequest(int sourceID, Request request, std::vector<Response> & responseVector);
};


#endif //PROJECT_CSIM_INTERCONNECTION_H
