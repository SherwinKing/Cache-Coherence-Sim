#include <iostream>
#include <getopt.h>
#include <string>
#include "../src/Simulator.h"
#include "../src/Interconnection.h"

int main(int argc, char * argv[]) {
    int option;

    int verbose;
    int s;
    int E;
    int b;
    int n;  // The number of processor
    std::string traceFilePath;

    std::string modeString;

    TOPO interconnectionTOPO = TOPO::D_RING;
    CoherenceType mode = SNOOPING;

    while( (option = getopt(argc, argv, "hvs:E:b:t:n:m:")) != -1 ){
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
            case 'n':
                n = atoi(optarg);
                break;
            case 't':
                traceFilePath = optarg;
                break;
            case 'm':
                modeString = optarg;
                if (modeString == "snooping") {
                    mode = SNOOPING;
                } else if (modeString == "token") {
                    mode = TOKEN;
                } else if (modeString == "directory") {
                    mode = DIRECTORY;
                } else {
                    std::cerr << "Wrong mode input of " << modeString << std::endl;
                    exit(EXIT_FAILURE);
                }

                std::cout << "Protocol: " << modeString << std::endl;
                break;
            default:
                std::cerr << "Usage: ./main [-v] [-m <coherence mode>] -s <s> -E <E> -b <b> -t <tracefile>" << std::endl;
                exit(EXIT_FAILURE);
        }
    }

    Simulator simulator(s, E, b, n, verbose, traceFilePath, interconnectionTOPO, mode);

    simulator.runTraceFile();


    simulator.printSummary();

    return 0;
}
