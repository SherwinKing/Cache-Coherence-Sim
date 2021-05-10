#ifndef PROJECT_CSIM_INTERCONNECTION_H
#define PROJECT_CSIM_INTERCONNECTION_H


#include <vector>
#include "Response.h"
#include "Request.h"
#include "CacheController.h"

class Interconnection {
private:
    std::vector<int> latencyVector;
    std::vector<CacheController> cacheVector;

    void setLatency(int i, int j, int latency);
    int getLatency(int nodeID1, int nodeID2);
public:
    Interconnection(int nodeNum) {
        latencyVector.reserve(nodeNum * nodeNum);
    }

    int sendRequest(int sourceID, int receiverID, Request request, Response & response);
    int broadcastRequest(int sourceID, Request request, std::vector<Response> & responseVector);
};


#endif //PROJECT_CSIM_INTERCONNECTION_H
