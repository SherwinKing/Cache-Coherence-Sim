#include "Simulator.h"

int TOTAL_PROC_NUM;

void Simulator::runTraceFile() {
    int processorID;
    std::string operation;
    std::string addressString;
    long address;

    int timeStamp = 0;
    while (traceFileStream >> processorID >> operation >> addressString)
    {
        // Convert hexadecimal string into long
        std::istringstream iss(addressString);
        iss >> std::hex >> address;

        // Execute trace line
        nodeSmartPointerControllers[processorID].get()->runCacheOp(address, operation, timeStamp);

        timeStamp++;
    }
}

void Simulator::printSummary() {
    statistics.printSummary();
}

Simulator::Simulator(int s, int E, int b, int n, int verbose, std::string traceFilePath, const TOPO topo, CoherenceType mode) :
        s(s), E(E), b(b), n(n), verbose(verbose),
        traceFileStream(traceFilePath), statistics(), interconnection(topo, statistics), mode_(mode) {
    if (!traceFileStream.is_open())
    {
        std::cerr << "Error opening trace file";
        traceFileStream.close();
        exit(EXIT_FAILURE);
    }

//    statistics = Statistics();
//    interconnection = Interconnection(topo, nodeSmartPointerControllers, statistics);
    TOTAL_PROC_NUM = n;
    statistics.setProcessorNum(n);

    for (int i = 0; i < n; i++) {
        std::shared_ptr<NodeController> nodeControllerPointer;
        if(mode_ == SNOOPING)
        nodeControllerPointer
                = std::make_shared<SnoopingCacheController>(s, E, b, i, interconnection, statistics);
        //nodeSmartPointerControllers.push_back(nodeControllerPointer);

        if(mode_ == DIRECTORY)
        // n cache controllers
        // Directory
        nodeControllerPointer = 
            std::make_shared<DirectoryCacheController>(s, E, b, i, interconnection, statistics);
        
        if(mode_ == TOKEN)
        // Token
        nodeControllerPointer = 
            std::make_shared<TokenCacheController>(s, E, b, i, interconnection, statistics);

        nodeSmartPointerControllers.push_back(nodeControllerPointer);
    }

    // one cache directory
    if(mode_ == DIRECTORY)
    nodeSmartPointerControllers.push_back(std::make_shared<Directory>(n, b, s, DIRECTORY, statistics));
    if(mode_ == TOKEN)
    nodeSmartPointerControllers.push_back(std::make_shared<Directory>(n, b, s, TOKEN, statistics));

    interconnection.setNodeControllerSmartPointerVector(nodeSmartPointerControllers);

}
