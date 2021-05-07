#ifndef PROJECT_CSIM_CACHE_H
#define PROJECT_CSIM_CACHE_H


#include "Request.h"    // RequestType

class Cache {
public:
    void stateTransition(long address, RequestType requestType);
};


#endif //PROJECT_CSIM_CACHE_H
