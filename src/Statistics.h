#ifndef PROJECT_CSIM_STATISTICS_H
#define PROJECT_CSIM_STATISTICS_H


class Statistics {
private:
    long totalOperationNum;
    long cacheHitNum;
    long cacheMissNum;
    long cacheFlushNum;
    long cacheEvictionNum;
    long cacheInvalidationNum;
    long latencySum;

public:
    void cacheHit(int processorID, long address);
    void cacheMiss(int processorID, long address);
    void cacheFlush(int processorID, long address);
    void cacheEvict(int processorID, long setID, long tag);
    void cacheInvalidate(int processorID, long address);

    void printSummary();

    Statistics();
};


#endif //PROJECT_CSIM_STATISTICS_H
