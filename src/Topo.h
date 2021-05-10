#ifndef PROJECT_CSIM_TOPO_H
#define PROJECT_CSIM_TOPO_H

#include <vector>
#include <math.h>
#include <iostream>

enum class TOPO { RING, D_RING, MESH, TORUS, TREES, CROSSBAR };

/**
 * generate latency matrix of size num of nodes * num of nodes
 * @param matrix    reference of matrix from Interconnection
 * @param topo      topology used
 * @param num       number of nodes in the network
 */
void initializeLatencyMatrix(std::vector<std::vector<int>> &matrix, TOPO topo, int num);

void ringLatency(std::vector<std::vector<int>>& matrix, int num);
void dringLatency(std::vector<std::vector<int>>& matrix, int num);
void meshLatency(std::vector<std::vector<int>>& matrix, int num);
void torusLatency(std::vector<std::vector<int>>& matrix, int num);
void treesLatency(std::vector<std::vector<int>>& matrix, int num);
void crossbarLatency(std::vector<std::vector<int>>& matrix, int num);



#endif //PROJECT_CSIM_TOPO_H
