#ifndef PROJECT_CSIM_SIMULATOR_H
#define PROJECT_CSIM_SIMULATOR_H

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <getopt.h>
#include "SnoopingCacheController.h"
#include "DirectoryCacheController.h"
#include "TokenCacheController.h"
#include "Directory.h"
#include "TokenCacheController.h"
#include "Cache.h"
#include "Statistics.h"
#include "Interconnection.h"

class Simulator {
private:
    std::vector<std::shared_ptr<NodeController>> nodeSmartPointerControllers;
    Statistics statistics;
    Interconnection interconnection;
    CoherenceType mode_;
//    int getOptions(int argc, char* argv[]);

    int s;  // Number of set index bits (2^s is the cache size, or the number of sets in cache)
    int E;  // Associativity
    int b;  // Number of block bits
    int n;  // Number of processors

    int verbose;

    std::ifstream traceFileStream;

public:
    Simulator(int s, int E, int b, int n, int verbose, std::string traceFilePath, const TOPO topo, CoherenceType mode);

    void runTraceFile();

    void printSummary();
};


#endif //PROJECT_CSIM_SIMULATOR_H
