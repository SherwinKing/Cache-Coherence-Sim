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

    TOPO interconnectionTOPO = TOPO::D_RING;
    CoherenceType mode = TOKEN;

    while( (option = getopt(argc, argv, "hvs:E:b:t:n:")) != -1 ){
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
            default:
                std::cerr << "Usage: ./main [-v] -s <s> -E <E> -b <b> -t <tracefile>";
                exit(EXIT_FAILURE);
        }
    }

    Simulator simulator(s, E, b, n, verbose, traceFilePath, interconnectionTOPO, mode);

    simulator.runTraceFile();


    simulator.printSummary();

    return 0;
}
