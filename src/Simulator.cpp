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

Simulator::Simulator(int s, int E, int b, int n, int verbose, std::string traceFilePath, const TOPO topo) :
        s(s), E(E), b(b), n(n), verbose(verbose),
        traceFileStream(traceFilePath) ,interconnection(topo, nodeSmartPointerControllers), statistics() {
    if (!traceFileStream.is_open())
    {
        std::cerr << "Error opening trace file";
        traceFileStream.close();
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; i++) {
        /*
        std::shared_ptr<NodeController> nodeControllerPointer
                = std::make_shared<SnoopingCacheController>(s, E, b, i, interconnection, statistics);
        nodeSmartPointerControllers.push_back(nodeControllerPointer);
        */

        // n cache controllers
        // Directory
        /*
        std::shared_ptr<NodeController> nodeControllerPointer = 
            std::make_shared<DirectoryCacheController>(s, E, b, i, interconnection, statistics);
        */
        // Token
        std::shared_ptr<NodeController> nodeControllerPointer = 
            std::make_shared<TokenCacheController>(s, E, b, i, interconnection, statistics);
        nodeSmartPointerControllers.push_back(nodeControllerPointer);
    }
    // one cache directory
    /*
    nodeSmartPointerControllers.push_back(std::make_shared<Directory>(n, b, s, DIRECTORY));
    */
    nodeSmartPointerControllers.push_back(std::make_shared<Directory>(n, b, s, TOKEN));

    interconnection = Interconnection(topo, nodeSmartPointerControllers);
    statistics = Statistics();
    TOTAL_PROC_NUM = n;
    statistics.setProcessorNum(n);
}
