#include "Simulator.h"


//int Simulator::getOptions(int argc, char **argv) {
//    int option;
//    while( (option = getopt(argc, argv, "hvs:E:b:t:")) != -1 ){
//        switch(option){
//            case 'v':
//                verbose = 1;
//                break;
//            case 's':
//                s = atoi(optarg);
//                break;
//            case 'E':
//                E = atoi(optarg);
//                break;
//            case 'b':
//                b = atoi(optarg);
//                break;
//            case 't':
//                traceFileStream.open("example.txt");
//                if (!traceFileStream.is_open())
//                {
//                    std::cerr << "Error opening trace file";
//                    traceFileStream.close();
//                    exit(EXIT_FAILURE);
//                }
//                break;
//            default:
//                fprintf(stderr,
//                        "Usage: ./csim [-v] -s <s> -E <E> -b <b> -t <tracefile>");
//                exit(EXIT_FAILURE);
//        }
//    }
//    return 0;
//}

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
        nodeSmartPointerControllers[processorID].get()->runCacheOp(address, operation, 0);

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
        std::shared_ptr<NodeController> nodeControllerPointer
                = std::make_shared<SnoopingCacheController>(s, E, b, i, interconnection, statistics);
        nodeSmartPointerControllers.push_back(nodeControllerPointer);
    }

    interconnection = Interconnection(topo, nodeSmartPointerControllers);
    statistics = Statistics();
}