#include <iostream>
#include "Statistics.h"

Statistics::Statistics() {
    totalOperationNum = 0;
    cacheHitNum = 0;
    cacheMissNum = 0;
    cacheFlushNum = 0;
    cacheEvictionNum = 0;
    cacheInvalidationNum = 0;
    latencySum = 0;
    totalOperationNum = 0;
    communicationCount = 0;
}

void Statistics::cacheHit(int processorID, long address) {
    totalOperationNum++;
    cacheHitNum++;
}

void Statistics::cacheMiss(int processorID, long address) {
    totalOperationNum++;
    cacheMissNum++;
}

void Statistics::cacheFlush(int processorID, long address) {
    cacheFlushNum++;
}

void Statistics::cacheEvict(int processorID, long setID, long tag) {
    cacheEvictionNum++;
}

void Statistics::cacheInvalidate(int processorID, long address) {
    cacheInvalidationNum++;
}

void Statistics::addLatency(int processorID, int latency) {
    latencySum += latency;
}

void Statistics::addCommunicationCount(int count) {
    communicationCount += count;
}

void Statistics::printSummary() {
    std::cout << "Number of processors: " << processorNum << std::endl;
    std::cout << "Total operation: " << totalOperationNum << std::endl;
    std::cout << "Cache hit number: " << cacheHitNum << std::endl;
    std::cout << "Cache miss number: " << cacheMissNum << std::endl;
    std::cout << "Cache flush number: " << cacheFlushNum << std::endl;
    std::cout << "Cache evict number: " << cacheEvictionNum << std::endl;
    std::cout << "Cache invalidate number: " << cacheInvalidationNum << std::endl;
    std::cout << "Total latency sum: " << latencySum << std::endl;
    std::cout << "Total communicationCount count: " << communicationCount << std::endl;

}

void Statistics::setProcessorNum(int processorNum) {
    Statistics::processorNum = processorNum;
}
