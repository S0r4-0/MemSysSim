#include <iostream>
#include <sstream>
#include <string>
#include "memsys.h"
#include "cache.h"

// -------- Helpers --------
int readIntOrDefault(const std::string& msg, int def) {
    std::cout << msg << " [" << def << "]: ";
    std::string line;
    std::getline(std::cin, line);
    if (line.empty()) return def;
    try {
        return std::stoi(line);
    } catch (...) {
        return def;
    }
}

std::string readStringOrDefault(const std::string& msg, const std::string& def) {
    std::cout << msg << " [" << def << "]: ";
    std::string line;
    std::getline(std::cin, line);
    return line.empty() ? def : line;
}

bool isPowerOfTwo(int x) {
    return x > 0 && (x & (x - 1)) == 0;
}

bool validCacheConfig(int cacheSize, int blockSize, int associativity) {
    if (cacheSize <= 0 || blockSize <= 0 || associativity <= 0) 
        return false;

    if (cacheSize % blockSize != 0)
        return false;

    int numBlocks = cacheSize / blockSize;
    if (numBlocks % associativity != 0)
        return false;

    if (!isPowerOfTwo(cacheSize) ||
        !isPowerOfTwo(blockSize) ||
        !isPowerOfTwo(associativity))
        return false;

    return true;
}

void printCacheConfigRules() {
    std::cout <<
    "Cache configuration rules:\n"
    "  - cache size, block size, and associativity must be > 0\n"
    "  - cache size must be divisible by block size\n"
    "  - (cache size / block size) must be divisible by associativity\n"
    "  - all values must be powers of two\n";
}

void initSystem(Memory*& mem, Cache*& L1, Cache*& L2) {
    while (true) {
        // -------- Memory --------
        int memSize = readIntOrDefault("Enter main memory size", 1024);

        if (memSize <= 0) {
            std::cout << "Memory size must be positive. Using default (1024).\n";
            memSize = 1024;
        }

        std::string allocType = readStringOrDefault(
            "Enter allocator (first_fit / best_fit / worst_fit / buddy)",
            "first_fit"
        );

        if (allocType == "buddy" && !isPowerOfTwo(memSize)) {
            std::cout << "Buddy allocator requires power-of-two memory size\n";
            std::cout << "Falling back to first_fit\n";
            allocType = "first_fit";
        }

        // -------- Cache --------
        std::cout << "\nNote:\n";
        printCacheConfigRules();

        std::cout << "\n--- L2 Cache Configuration ---\n";
        int l2Size  = readIntOrDefault("L2 cache size", 256);
        int l2Block = readIntOrDefault("L2 block size", 16);
        int l2Assoc = readIntOrDefault("L2 associativity", 4);

        std::cout << "\n--- L1 Cache Configuration ---\n";
        int l1Size  = readIntOrDefault("L1 cache size", 64);
        int l1Block = readIntOrDefault("L1 block size", 16);
        int l1Assoc = readIntOrDefault("L1 associativity", 2);

        if (!validCacheConfig(l2Size, l2Block, l2Assoc)) {
            std::cout << "Invalid L2 cache configuration. Using defaults.\n";
            l2Size = 256; l2Block = 16; l2Assoc = 4;
        }

        if (!validCacheConfig(l1Size, l1Block, l1Assoc)) {
            std::cout << "Invalid L1 cache configuration. Using defaults.\n";
            l1Size = 64; l1Block = 16; l1Assoc = 2;
        }

        // -------- Hierarchy Constraint --------
        if (!(l1Size < l2Size && l2Size < memSize)) {
            std::cout <<
            "Invalid cache hierarchy ordering.\n"
            "Requirement: L1 size < L2 size < Main memory size.\n"
            "Please re-enter configuration.\n\n";
            continue;  // restart init loop
        }

        // -------- Create System --------
        mem = new Memory(memSize);
        mem->setAllocator(allocType);

        L2 = new Cache(l2Size, l2Block, l2Assoc, nullptr, mem);
        L1 = new Cache(l1Size, l1Block, l1Assoc, L2, nullptr);

        // -------- Cache Policy --------
        std::string cachePolicy = readStringOrDefault(
            "Enter cache policy (fifo / lru / lfu)",
            "fifo"
        );

        if (!L1->setPolicy(cachePolicy) || !L2->setPolicy(cachePolicy)) {
            std::cout << "Invalid cache policy, using fifo\n";
            L1->setPolicy(std::string("fifo"));
            L2->setPolicy(std::string("fifo"));
        }

        std::cout << "\nSystem initialized.\n";
        break; // SUCCESS
    }
}

// -------- Main --------
int main() {
   
    Memory* mem = nullptr;
    Cache* L1 = nullptr;
    Cache* L2 = nullptr;

    initSystem(mem, L1, L2);

    // ================= Command Loop =================
    std::cout << "Type 'help' to see available commands.\n";
    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;

        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd.empty()) continue;
        if (cmd == "exit") break;

        // ---- Set policy ----
        if (cmd == "set") {
            std::string target, policy;
            ss >> target >> policy;

            if (target == "cache") {
                if (!L1->setPolicy(policy) || !L2->setPolicy(policy))
                    std::cout << "Invalid cache policy\n";
                else
                    std::cout << "Cache policy set to " << policy << '\n';
            }
            else if (target == "memory") {
                if (policy == "buddy") {
                    std::cout << "Buddy allocator can only be set at startup\n";
                }
                else if (!mem->setAllocator(policy)) {
                    std::cout << "Invalid allocator\n";
                }
                else {
                    std::cout << "Memory allocator set to " << policy << '\n';
                }
            }
            else {
                std::cout << "Usage: set cache|memory POLICY\n";
            }
        }

        // ---- Allocation ----
        else if (cmd == "malloc") {
            int size; ss >> size;
            
            if (size <= 0) {
                std::cout << "Size must be positive\n";
                continue;
            }

            int id = mem->malloc(size);

            if (id == -1) {
                std::cout << "Allocation failed\n";
            } else {
                int start, sz;
                if (mem->getLastAllocation(start, sz))
                    L1->invalidateRange(start, sz);
                std::cout << "Allocated block id = " << id << '\n';
            }
        }

        // ---- Free ----
        else if (cmd == "free") {
            int id; ss >> id;
            if (mem->free(id))
                std::cout << "Block " << id << " freed\n";
            else
                std::cout << "Invalid block id\n";
        }

        // ---- Dump ----
        else if (cmd == "dump") {
            mem->dump();
        }

        // ---- Access ----
        else if (cmd == "access") {
            int address; ss >> address;
            std::cout << (L1->access(address) ? "Cache hit\n" : "Cache miss\n");
        }

        // ---- Stats ----
        else if (cmd == "stats") {
            mem->stats();
            L1->stats(1);
        }

        // ---- Reinitialise system ----
        else if (cmd == "reinit") {
            std::cout << "Reinitializing system...\n";

            delete L1;
            delete L2;
            delete mem;

            L1 = nullptr;
            L2 = nullptr;
            mem = nullptr;

            initSystem(mem, L1, L2);
        }

        // ---- Help Menu ----
        else if (cmd == "help") {
            std::cout <<
            "Available commands:\n"
            "  malloc SIZE              Allocate memory block\n"
            "  free ID                  Free allocated block\n"
            "  access ADDRESS           Access memory address (cache lookup)\n"
            "  dump                     Dump memory layout\n"
            "  stats                    Show memory and cache statistics\n"
            "  set cache POLICY         Change cache replacement policy\n"
            "  set memory POLICY        Change memory allocation strategy\n"
            "  reinit                   Reinitialize system (full restart)\n"
            "  exit                     Exit simulator\n"
            "\n"
            "Memory allocation policies:\n"
            "  first_fit\n"
            "  best_fit\n"
            "  worst_fit\n"
            "  buddy        (startup only)\n"
            "\n"
            "Cache replacement policies:\n"
            "  fifo\n"
            "  lru\n"
            "  lfu\n";
        }

        else {
            std::cout << "Unknown command\n";
        }
    }

    delete L1;
    delete L2;
    delete mem;

    return 0;
}
