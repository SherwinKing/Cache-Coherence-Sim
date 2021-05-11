#include <iostream>
#include "CacheSet.h"

CacheSet::CacheSet(long setID, int setSize) : setID(setID) {
    this->setID = setID;
    this->setSize = setSize;
    for (int i = 0; i < setSize; i++) {
        CacheLine cacheLine;
        cacheLine.setID = setID;
        cacheLine.lastUseTimestamp = 0;
        cacheLine.isEmpty = 1;
        cacheLine.dirtyBit = 0;
        cacheLine.tag = 0;
        cacheLine.coherenceState = {.tokenNum = 0, .mesiState = I};
        cacheLines.push_back(cacheLine);
    }
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
        if (cacheLines[i].tag == tag && (!cacheLines[i].isEmpty)) {
            return & cacheLines[i];
        }
    }
    return nullptr;
}

CacheLine &CacheSet::findCacheBlockToReplace() {
    std::vector<CacheLine>::iterator iterator = cacheLines.begin();
    std::vector<CacheLine>::iterator cacheLineToReplace = iterator;
    int leastRecentTimeStamp = cacheLineToReplace->lastUseTimestamp;
    while (iterator != cacheLines.end()) {
        // If found empty cache block, use it
        if (iterator->isEmpty) {
            cacheLineToReplace = iterator;
            std::cerr << "findCacheBlockToReplace: found empty block. iterator address" << &*cacheLineToReplace << std::endl;
            break;
        }

        // Look for least recently used cache line.
        if (iterator->lastUseTimestamp < leastRecentTimeStamp) {
            cacheLineToReplace = iterator;
            leastRecentTimeStamp = iterator->lastUseTimestamp;
        }
        iterator++;
    }
    std::cerr << "findCacheBlockToReplace iterator address" << &*cacheLineToReplace << std::endl;
    std::cerr << "findCacheBlockToReplace block.isEmpty" << cacheLineToReplace->isEmpty << std::endl;
    return *cacheLineToReplace;
}
