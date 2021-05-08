#ifndef PROJECT_CSIM_CACHESET_H
#define PROJECT_CSIM_CACHESET_H


#include "Cache.h"

class CacheSet {
private:
    long setID;
    int setSize;
    std::vector<CacheLine> cacheLines;
public:
    CacheSet(long setID, int setSize);

    long getSetID() const;

    int getSetSize() const;

    const std::vector<CacheLine> &getCacheLines() const;
};


#endif //PROJECT_CSIM_CACHESET_H
