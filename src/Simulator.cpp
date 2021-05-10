#include <sstream>
#include <fstream>
#include <getopt.h>
#include "Simulator.h"


int Simulator::getOptions(int argc, char **argv) {
    int option;
    while( (option = getopt(argc, argv, "hvs:E:b:t:")) != -1 ){
        switch(option){
            case 'v':
                verbose = 1;
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                traceFileStream.open("example.txt");
                if (!traceFileStream.is_open())
                {
                    std::cerr << "Error opening trace file";
                    traceFileStream.close();
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                fprintf(stderr,
                        "Usage: ./csim [-v] -s <s> -E <E> -b <b> -t <tracefile>");
                exit(EXIT_FAILURE);
        }
    }
    return 0;
}

void Simulator::runTraceFile(std::ifstream traceFileStream) {
    int processorID;
    std::string operation;
    std::string addressString;
    long address;

    int timeStamp = 0;
    while (traceFileStream >> processorID >> operation >> addressString);
    {
        // Convert hexadecimal string into long
        std::istringstream iss(addressString);
        iss >> std::hex >> address;

        // Execute trace line
        cacheControllers[processorID].runCacheOp(address, operation, 0);

        timeStamp++;
    }
}