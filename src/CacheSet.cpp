#include "CacheSet.h"

CacheSet::CacheSet(long setID, int setSize) : setID(setID) {
    this->setID = setID;
    this->setSize = setSize;
}

long CacheSet::getSetID() const {
    return setID;
}

int CacheSet::getSetSize() const {
    return setSize;
}

const std::vector<CacheLine> &CacheSet::getCacheLines() const {
    return cacheLines;
}

CacheLine *CacheSet::findCacheLine(long tag) {
    for (int i = 0; i < cacheLines.size(); i++) {
        if (cacheLines[i].tag == tag) {
            return & cacheLines[i];
        }
    }
    return nullptr;
}

CacheLine &CacheSet::findCacheBlockToReplace() {
    std::vector<CacheLine>::iterator iterator = cacheLines.begin();
    int leastRecentTimeStamp = iterator->lastUseTimestamp;
    std::vector<CacheLine>::iterator cacheLineToReplace;
    while (iterator != cacheLines.end()) {
        // If found empty cache block, use it
        if (iterator->isEmpty) {
            cacheLineToReplace = iterator;
            break;
        }

        // Look for least recently used cache line.
        if (iterator->lastUseTimestamp < leastRecentTimeStamp) {
            cacheLineToReplace = iterator;
            leastRecentTimeStamp = iterator->lastUseTimestamp;
        }
        iterator++;
    }
    return *cacheLineToReplace;
}
