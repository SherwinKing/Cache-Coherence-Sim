#include <iostream>
#include "Cache.h"
#include "CacheSet.h"
#include <cassert>

Cache::Cache(int id, int s, int E, int b) : id(id), s(s), E(E), b(b) {
    this->id = id;
    this->s = s;
    this->E = E;
    this->b = b;

    setIDMask = ((1<<s) - 1) << b;
    tagMask = ~((1<<(s+b))-1);

    for (long setID = 0; setID < 1 << s; setID++) {
        cacheSets.push_back(CacheSet(setID, this->E));
    }
    assert(cacheSets[0].getSetID() == 0);
}

CacheLine *Cache::findCacheLine(long setID, long tag) {
//    // Get the set index and tag for the block
    std::cerr << "findcacheline setID: " << setID << std::endl;
    CacheSet & cacheSet = cacheSets[setID];
    return cacheSet.findCacheLine(tag);
}

int Cache::ifCacheLinePresent(long setInd, long tag) {
    std::cerr << "pointer:" << findCacheLine(setInd, tag) << std::endl;
    return findCacheLine(setInd, tag) != nullptr;
}


CacheLine &Cache::getCacheLine(long setID, long tag) {
    CacheLine * cacheLine = findCacheLine(setID, tag);
    if (cacheLine == nullptr)
        throw "The cache line does not exist!";
    return *cacheLine;
}

CacheLine & Cache::findCacheBlockToReplace(long setID) {
    CacheSet & cacheSet = cacheSets[setID];
    return cacheSet.findCacheBlockToReplace();
}
