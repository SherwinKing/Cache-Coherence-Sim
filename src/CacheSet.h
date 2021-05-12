#ifndef PROJECT_CSIM_CACHESET_H
#define PROJECT_CSIM_CACHESET_H


//#include "Cache.h"
#include <vector>
#include "Statistics.h"

// global variable for the number of processors
extern int TOTAL_PROC_NUM;
enum MESIState { M, E, S, I};

typedef struct {
    MESIState mesiState;
    int tokenNum;
} CoherenceState;

typedef struct{
    long tag;
    long setID;
    int dirtyBit;
    int isEmpty;
    int lastUseTimestamp;
    CoherenceState coherenceState;
} CacheLine;

class CacheSet {
private:
    long setID;
    int setSize;
    std::vector<CacheLine> cacheLines;
public:
    CacheSet(long setID, int setSize);

    long getSetID() const;

    int getSetSize() const;

    CacheLine * findCacheLine(long tag);

    CacheLine & findCacheBlockToReplace();

    const std::vector<CacheLine> &getCacheLines() const;
};


#endif //PROJECT_CSIM_CACHESET_H
