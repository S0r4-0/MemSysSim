#include "memory.h"
#include <iostream>
#include <algorithm>

// Memory block representation
struct Memory::Block{
    int start, size, id;
    bool free;
    Block* next;

    Block (int start, int size, int id, bool free, Block* next)
        : start(start), size(size), id(id), free(free), next(next) {} 

    // Allocate block without splitting
    void allocateExact(int id){
        free = false;
        this->id = id;
    }

    // Split block and allocate required size
    void splitAndAllocate(int need, int id){
        Block* split = new Block(start + need, size - need, -1, true, next);
        size = need;
        free = false;
        this->id = id;
        next = split;
    }

    // Mark block as free
    void makeFree(){
        free = true;
        id = -1;
    }

    // Merge with next free block
    void mergeNext(){
        Block* tmp = next;
        size += tmp->size;
        next = tmp->next;
        delete tmp;
    }
};

// Common allocation handler
int Memory::allocate(Block* block, int need){
    usedMemory += need;
    int id = nextId++;

    lastAllocStart = block->start;
    lastAllocSize = need;

    if (block->size == need) block->allocateExact(id);
    else block->splitAndAllocate(need, id);
    return id;
}

// First Fit allocation
int Memory::mallocFF(int need){
    Block* cur = head;
    while (cur){
        if (cur->free && cur->size >= need){
            return allocate(cur, need);
        }
        cur = cur->next;
    }
    return -1;
}

// Best Fit allocation
int Memory::mallocBF(int need){
    Block* cur = head, *best = nullptr;
    while (cur){
        if (cur->free && cur->size >= need){
            if (best == nullptr || best->size > cur->size){
                best = cur;
            }
        }
        cur = cur->next;
    }
    return best ? allocate(best, need) : -1;
}

// Worst Fit allocation
int Memory::mallocWF(int need){
    Block* cur = head, *worst = nullptr;
    while (cur){
        if (cur->free && cur->size >= need){
            if (worst == nullptr || worst->size < cur->size){
                worst = cur;
            }
        }
        cur = cur->next;
    }
    return worst ? allocate(worst, need) : -1;
}

// Initialize memory and buddy system
Memory::Memory(int size) : totalMemory(size), nextId(1), lastAllocStart(-1), lastAllocSize(0), allocator(AllocatorType::FIRST_FIT) {
    head = new Block(0, size, -1, true, nullptr);

    maxOrder = 0;
    while ((1 << maxOrder) < size) maxOrder++;

    freeLists.resize(maxOrder+1);
    freeLists[maxOrder].push_back(0);
}

// Set allocation strategy
bool Memory::setAllocator(std::string& type){
    if (type == "first_fit") allocator = AllocatorType::FIRST_FIT;
    else if (type == "best_fit") allocator = AllocatorType::BEST_FIT;
    else if (type == "worst_fit") allocator = AllocatorType::WORST_FIT;
    else if (type == "buddy") allocator = AllocatorType::BUDDY; 
    else return false;
    return true;
}

// Allocate memory
int Memory::malloc(int size){
    if (size <= 0) return -1;

    totalAllocs++;
    lastAllocStart = -1;

    int id;
    if (allocator == AllocatorType::BUDDY) id = buddyMalloc(size);
    else if (allocator == AllocatorType::FIRST_FIT) id = mallocFF(size);
    else if (allocator == AllocatorType::BEST_FIT) id = mallocBF(size);
    else id = mallocWF(size);

    if (id == -1) failedAllocs++;
    return id;
}

// Free allocated memory
bool Memory::free(int id){
    if (allocator == AllocatorType::BUDDY) return buddyFree(id);

    Block* cur = head, *prev = nullptr;
    while (cur){
        if (!cur->free && cur->id == id){
            cur->makeFree();
            if (cur->next && cur->next->free) cur->mergeNext();
            if (prev && prev->free) prev->mergeNext();

            usedMemory -= cur->size;
            return true;
        }
        prev = cur; cur = cur->next;
    }

    return false;
}

// Dummy access validation
bool Memory::access(int id){
    id++; id--;                 // Added just to remove warning (or removing warning flag from Makefile)
    return true;
}

// Fetch last allocation info
bool Memory::getLastAllocation(int& start, int&size){
    if (lastAllocStart == -1) return false;
    start = lastAllocStart;
    size = lastAllocSize;
    return true;
}

// Print memory layout
void Memory::dump(){
    Block* cur = head;
    while (cur){
        std::cout << "[0x" << std::hex << cur->start << " - 0x"
            << (cur->start + cur->size - 1) << "] ";
        if (cur->free) std::cout << "FREE" << '\n';
        else std::cout << "Used (id=" << std::dec << cur->id << ")" << '\n';
        cur = cur->next;
    }
    std::cout << std::dec;
}

// Buddy allocation
int Memory::buddyMalloc(int size){
    int need = 1;
    while (need < size) need <<= 1;

    int order = 0;
    while ((1 << order) < need) order++;

    int cur = order;
    while (cur <= maxOrder && freeLists[cur].empty()) cur++;
    if (cur > maxOrder) return -1;

    int id = freeLists[cur].back();
    freeLists[cur].pop_back();

    while (cur > order){
        cur--;
        int buddy = id + (1 << cur);
        freeLists[cur].push_back(buddy);
    }

    buddyAllocated[id] = {order, size};
    int allocSize = 1 << order;

    lastAllocStart = id;
    lastAllocSize = allocSize;

    usedMemory += allocSize;
    internalFrag += allocSize - size;

    return id;
}

// Buddy deallocation with merge
bool Memory::buddyFree(int id){
    if (!buddyAllocated.count(id)) return false;

    int order = buddyAllocated[id].first, sizeNeed = buddyAllocated[id].second;
    buddyAllocated.erase(id);

    int allocSize = (1 << order);

    usedMemory -= allocSize;
    internalFrag -= allocSize - sizeNeed;

    while (order < maxOrder){
        int buddy = id ^ (1 << order);
        auto& list = freeLists[order];
        
        auto it = std::find(list.begin(), list.end(), buddy);
        if (it == list.end()) break;

        list.erase(it);
        id = std::min(id, buddy);
        order++;
    }
    freeLists[order].push_back(id);
    return true;
}

// Print memory statistics
void Memory::stats(){
    std::cout << "==== Memory Statistics ====" << '\n';

    std::cout << "Total memory           : " << totalMemory << '\n';
    std::cout << "Used memory            : " << usedMemory << '\n';
    std::cout << "Free memory            : " << totalMemory - usedMemory << '\n';
    std::cout << "Memory Utilization     : " << (double)usedMemory/totalMemory << '\n';

    
    int largestFree = 0;
    if (allocator == AllocatorType::BUDDY){
        for (int order = 0; order <= maxOrder; order++){
            if (freeLists[order].size()) largestFree = std::max(largestFree, (1 << order));
        }
    } else {
        Block* cur = head;
        while (cur){
            if (cur->free) largestFree = std::max(largestFree, cur->size);
            cur = cur->next;
        }
    }
    
    std::cout << "Internal fragmentation : " << (usedMemory ?  (double)internalFrag/usedMemory : 0.0) << '\n'; 
    std::cout << "External fragmentation : " << (totalMemory == usedMemory ? 0.0 : 1 - (double)largestFree/(totalMemory - usedMemory)) << '\n'; 

    std::cout << "Total allocations      : " << totalAllocs << '\n';
    std::cout << "Successful allocations : " << totalAllocs - failedAllocs << '\n';
    std::cout << "Failed allocations     : " << failedAllocs << '\n';
    std::cout << "Success rate           : " << (totalAllocs ?  1 - (double)failedAllocs/totalAllocs : 0.0)<< '\n'; 
    std::cout << "Failed rate            : " << (totalAllocs ? (double)failedAllocs/totalAllocs : 0.0) << '\n'; 
}