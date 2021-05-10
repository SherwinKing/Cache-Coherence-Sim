#ifndef PROJECT_CSIM_SIMULATOR_H
#define PROJECT_CSIM_SIMULATOR_H

#include <String>
#include <iostream>
#include "Cache.h"
#include "Statistics.h"
#include "Interconnection.h"

class Simulator {
private:
    std::vector<NodeController> cacheControllers;
    Statistics & statistics;
    Interconnection interconnection;
    int getOptions(int argc, char* argv[]);

    int s;  // Number of set index bits (2^s is the cache size, or the number of sets in cache)
    int E;  // Associativity
    int b;  // Number of block bits

    int verbose;

    std::ifstream traceFileStream;

public:

    void runTraceFile(std::ifstream);
};


#endif //PROJECT_CSIM_SIMULATOR_H
