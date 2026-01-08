#ifndef MEMORY_H
#define MEMORY_H

#include<string>
#include<vector>
#include<unordered_map>

// Memory allocator
class Memory {
private:
    struct Block;                       // Memory block structure

    // Supported allocation strategies
    enum class AllocatorType{
        FIRST_FIT, BEST_FIT, WORST_FIT, BUDDY
    };

    Block* head;                        // Head of memory block list
    int totalMemory;
    int nextId, lastAllocStart, lastAllocSize;
    AllocatorType allocator;            // Active allocator

    // Buddy Allocation
    int maxOrder;                       // Max buddy order
    std::vector<std::vector<int>> freeLists;
    std::unordered_map<int,std::pair<int,int>> buddyAllocated;

    // Statistics utilities
    int totalAllocs = 0, failedAllocs = 0, usedMemory = 0, internalFrag = 0;

    int allocate(Block* block, int size);   // Internal allocation helper
    int mallocFF(int size);                 // First Fit
    int mallocBF(int size);                 // Best Fit
    int mallocWF(int size);                 // Worst Fit

    int buddyMalloc(int size);              // Buddy allocation
    bool buddyFree(int id);                 // Buddy deallocation
public:
    Memory(int size);                       // Constructor

    bool setAllocator(std::string type);   // Set allocator type
    int malloc(int size);                   // Allocate memory
    bool free(int id);                      // Free allocation
    bool access(int id);                    // Access check
    bool getLastAllocation(int& start, int& size);  // Last allocation info

    void dump();                            // Print memory layout
    void stats();                           // Print statistics
};

#endif