#include <iostream>
#include <sstream>
#include "memory.h"
#include "cache.h"

int main(){
    Memory* mem = nullptr;
    Cache *L1 = nullptr, *L2 = nullptr;

    std::string line;
    while (true){
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;

        std::stringstream ss(line);
        std::string cmd; 
        ss >> cmd;
        
        if (cmd.empty()) continue;

        if (cmd == "exit") break;

        if (cmd == "init"){
            std::string tmp;
            int size;
            ss >> tmp >> size;
            
            std::string allocType = "first_fit";
            ss >> allocType;

            if (size <= 0) {std::cout << "Size must be positive " << '\n'; continue;}

            mem = new Memory(size);
            std::cout << "Memory initialized with size " << size << '\n';
            if (!mem->setAllocator(allocType)) std::cout << "Invalid allocation type, using first_fit" << '\n';
            if (allocType == "buddy" && (size & (size-1)) != 0) std::cout << "Unable to set buddy. Buddy alloctor requires memory to be of form 2^x, using first_fit" << '\n';

            L2 = new Cache(256, 16, 4, nullptr, mem);
            L1 = new Cache(64, 16, 2, L2, nullptr);
            continue;
        }

        if (!mem){
            std::cout << "System not initializaed" << '\n';
            continue;
        }

        if (cmd == "set"){
            std::string what;
            ss >> what;

            if (what == "cache"){
                std::string level, policy;
                ss >> level >> policy;

                Cache* target = nullptr;
                if (level == "L1") target = L1;
                else if (level == "L2") target = L2;

                if (!target) {std::cout << "Invalid cache level" << '\n'; continue;}
                if (!target->setPolicy(policy)) {std::cout << "Invalid cache policy" << '\n'; continue;}

                std::cout << "Cache policy for " << level << " set to " << policy << '\n';
            } else {
                std::string type;
                ss >> type;
                if (type == "buddy"){
                    std::cout << "Buddy allocator can only be set at init" << '\n';
                    continue;
                }
    
                if (!mem->setAllocator(type)) std::cout << "Invalid allocation type" << '\n';
                else std::cout << "Allocator set to " << type << '\n';
            }
        } else if (cmd == "malloc"){
            int size; ss >> size;
            int id = mem->malloc(size);
            
            if (id == -1) {
                std::cout << "Allocation failed" << '\n';
            } else {
                int start, sz;
                if (mem->getLastAllocation(start, sz)) L1->invalidateRange(start, sz);

                std::cout << "Allocation block id = " << id << '\n';
            }
        } else if (cmd == "free"){
            int id; ss >> id;
            if (mem->free(id)){
                std::cout << "Block " << id << " freed and merged" << '\n';
            } else {
                std::cout << "Invalid block id" << '\n';
            }
        } else if (cmd == "dump"){
            mem->dump();
        } else if (cmd == "access"){
            int address; ss >> address;
            bool hit = L1->access(address);
            std::cout << (hit ? "Cache hit" : "Cache miss") << '\n';
        } else if (cmd == "stats"){
            mem->stats();
        } else if (cmd == "stats_cache") {
            L1->stats(1);
        } else {
            std::cout << "Unknown command" << '\n';
        }
    }
    return 0;
}