#ifndef PROJECT_CSIM_CACHE_H
#define PROJECT_CSIM_CACHE_H


#include <vector>
#include <vector>
#include "Request.h"    // RequestType
#include "CacheSet.h"

typedef struct{
    long tag;
    long setID;
    int dirtyBit;
    int isEmpty;
    int lastUseTimestamp;
} CacheLine;

class Cache {
private:
    int id;
    int s;  // Number of set index bits (2^s is the cache size, or the number of sets in cache)
    int E;  // Associativity
    int b;  // Number of block bits
    std::vector<CacheSet> cacheSets;

public:
    Cache(int id, int s, int E, int b);

    void stateTransition(long address, RequestType requestType);

};


#endif //PROJECT_CSIM_CACHE_H
