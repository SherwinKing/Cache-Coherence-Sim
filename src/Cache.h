#ifndef PROJECT_CSIM_CACHE_H
#define PROJECT_CSIM_CACHE_H


#include <vector>
#include <vector>
#include <string>
#include "Request.h"    // RequestType
#include "CacheSet.h"
#include <cassert>

class Cache {
private:
    int id;
    int s;  // Number of set index bits (2^s is the cache size, or the number of sets in cache)
    int E;  // Associativity
    int b;  // Number of block bits

    // Masks
    long setIDMask;
    long tagMask;

    int totalProcessorNum;  // Total number of processors
    std::vector<CacheSet> cacheSets;

    CacheLine *findCacheLine(long setID, long tag);

public:
    Cache(int id, int s, int E, int b);

    int ifCacheLinePresent(long setInd, long tag);
    CacheLine &getCacheLine(long setID, long tag);
    CacheLine &findCacheBlockToReplace(long setID);
};


#endif //PROJECT_CSIM_CACHE_H
