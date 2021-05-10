#ifndef PROJECT_CSIM_STATISTICS_H
#define PROJECT_CSIM_STATISTICS_H


class Statistics {

public:
    void cacheHit(int processorID, long address);
    void cacheMiss(int processorID, long address);
    void cacheFlush(int processorID, long address);
    void cacheEvict(int processorID, long setID, long tag);
    void cacheInvalidate(int processorID, long address);
};


#endif //PROJECT_CSIM_STATISTICS_H
