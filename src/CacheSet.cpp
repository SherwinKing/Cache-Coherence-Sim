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
