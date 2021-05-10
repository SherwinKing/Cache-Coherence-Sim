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
    std::vector<NodeController> cacheVector;

    void setLatency(int i, int j, int latency);
    int getLatency(int nodeID1, int nodeID2);
public:
    Interconnection(TOPO topo, int nodeNum);

    int sendRequest(int sourceID, int receiverID, Request request, Response & response);
    int broadcastRequest(int sourceID, Request request, std::vector<Response> & responseVector);
    void sendEviction(int sourceID, long setID, long tag, int content);
};


#endif //PROJECT_CSIM_INTERCONNECTION_H
