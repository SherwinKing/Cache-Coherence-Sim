#include <iostream>
#include <getopt.h>
#include <string>

int main(int argc, char * argv[]) {
    int option;

    int verbose;
    int s;
    int E;
    int b;
    std::string traceFilePath;

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
                traceFilePath = optarg;
                break;
            default:
                std::cerr << "Usage: ./main [-v] -s <s> -E <E> -b <b> -t <tracefile>");
                exit(EXIT_FAILURE);
        }


    }

    return 0;
}