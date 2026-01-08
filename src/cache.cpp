#include "cache.h"
#include <iostream>

// Cache line structure
struct Cache::CacheLine{
    bool valid = false;
    int tag = 0;

    // LRU, LFU, FIFO
    int lastUsed = 0, frequency = 0, insertedAt = 0;
};

// Cache constructor
Cache::Cache(int cacheSize, int blockSize, int associativity, Cache* next, Memory* memory) 
    : cacheSize(cacheSize), blockSize(blockSize), associativity(associativity), next(next), memory(memory), policy(ReplacementPolicy::FIFO), globalTime(0), hits(0), misses(0)
{
    numBlocks = cacheSize / blockSize;           // Total cache blocks
    numSets = numBlocks / associativity;         // Total cache sets
    
    // Initialize cache
    sets.resize(numSets, std::vector<CacheLine>(associativity));
}

// Access cache address
bool Cache::access(int address){
    int blockNumber = address / blockSize;      // Compute block number
    int index = blockNumber % numSets;          // Compute set index
    int tag = blockNumber / numSets;            // Compute tag

    auto& set = sets[index];
    globalTime++;
    
    // HIT
    for (auto& line : set){
        if (line.valid && line.tag == tag){
            line.lastUsed = globalTime;
            line.frequency++;
            hits++;
            return true;
        }
    }

    // MISS
    misses++;
    if (next) next->access(address);
    else if (memory) memory->access(address);

    // Fill empty line
    for (auto& line : set){
        if (!line.valid) {
            line.valid = true;
            line.tag = tag;
            line.insertedAt = globalTime;
            line.lastUsed = globalTime;
            line.frequency = 1;
            return false;
        }
    }

    // Replacement Policy
    CacheLine* victim = &set[0];
    if (policy == ReplacementPolicy::FIFO){
        for (auto& line : set){
            if (line.insertedAt < victim->insertedAt) victim = &line;
        }
    } else if (policy ==ReplacementPolicy::LRU){
        for (auto& line : set){
            if (line.lastUsed < victim->lastUsed) victim = &line;
        }
    }  else {
        for (auto& line : set){
            if (line.frequency < victim->frequency) victim = &line;
        }
    }

    // Replace victim cache line
    victim->valid = true;
    victim->tag = tag;
    victim->insertedAt = globalTime;
    victim->lastUsed = globalTime;
    victim->frequency = 1;
    return false;
}

// Set cache replacement policy
bool Cache::setPolicy(std::string policyName){
    if (policyName == "fifo") policy = ReplacementPolicy::FIFO;
    else if (policyName == "lru") policy = ReplacementPolicy::LRU;
    else if (policyName == "lfu") policy = ReplacementPolicy::LFU;
    else return false;
    return true;
}

// Invalidate cache lines overlapping memory range
void Cache::invalidateRange(int start, int size){
    int end = start + size;
    for (int i = 0; i < numSets; i++){
        for (int j = 0; j < associativity; j++){
            if (!sets[i][j].valid) continue;
            int blockStart = (sets[i][j].tag * numSets + i) * blockSize;
            int BlockEnd = blockStart + blockSize;

            if (blockStart < end && BlockEnd > start) sets[i][j].valid = false;
        }
    }

    if (next) next->invalidateRange(start, size);
}

// Print cache statistics
void Cache::stats(int level){
    std::cout << "==== Cache L" << level << " Statistics ===\n";

    std::cout << "Hits          : " << hits << '\n';
    std::cout << "Misses        : " << misses<< '\n';
    std::cout << "Hit Ratio     : " << (hits + misses ? (double)hits/(hits + misses) : 0.0) << '\n';

    if (next) {
        std::cout << "Misses propagated to L" << level+1 << " : " << misses << '\n';
        next->stats(level+1);
    } else {
        std::cout << "Misses propagated to Memory : " << misses << '\n';
    }
}