#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include "memory.h"

// Cache simulator
class Cache{
private:
    struct CacheLine;                       // Single cache line

    // Cache replacement policies
    enum class ReplacementPolicy{
        FIFO, LRU, LFU
    };
    
    int cacheSize, blockSize, associativity;
    int numBlocks, numSets;

    // Address format: tag | index | offset
    
    std::vector<std::vector<CacheLine>> sets;   // Cache sets
    Cache* next;                                // Next cache level
    Memory* memory;                             // Backing memory
    ReplacementPolicy policy;                   // Active policy
    int globalTime;

    int hits, misses;

public:
    Cache(int cacheSize, int blockSize, int associativity, Cache* next, Memory* memory);
    
    bool access(int address);                   // Access cache address
    bool setPolicy(std::string& policyName);    // Set replacement policy
    void invalidateRange(int start, int size);  // Invalidate cache range
    void stats(int level);                      // Print cache stats
};

#endif