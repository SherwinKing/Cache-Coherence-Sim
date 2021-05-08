#ifndef PROJECT_CSIM_SIMULATOR_H
#define PROJECT_CSIM_SIMULATOR_H

#include <String>
#include <iostream>
#include "Cache.h"
#include "Statistics.h"
#include "Interconnection.h"

class Simulator {
private:
    std::vector<Cache> caches;
    Statistics statistics;
    Interconnection interconnection;

    void runTraceLine(std::string);

public:
    void runTraceFile(std::ifstream);
};


#endif //PROJECT_CSIM_SIMULATOR_H
