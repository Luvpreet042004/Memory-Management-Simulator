#include <iostream>
#include <sstream>
#include <stdexcept>
#include "allocator/physical_memory.h"
#include "cache/cache.h"

int main() {
    PhysicalMemory* pm = nullptr;

    Cache l1;
    Cache l2;
    size_t total_cycles = 0;

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "exit") break;

        if (cmd == "init") {
            std::string mem;
            size_t size;
            iss >> mem >> size;
            delete pm;
            pm = new PhysicalMemory(size);
            std::cout << "Initialized memory: " << size << " bytes\n";
        }

        else if (cmd == "set") {
            std::string what;
            iss >> what;

            if (what == "allocator") {
                std::string strat;
                iss >> strat;
                if (pm) {
                    if (strat == "first_fit") pm->set_strategy(FitStrategy::FIRST_FIT);
                    else if (strat == "best_fit") pm->set_strategy(FitStrategy::BEST_FIT);
                    else if (strat == "worst_fit") pm->set_strategy(FitStrategy::WORST_FIT);
                    std::cout << "Allocator set to " << strat << "\n";
                }
            }

            else if (what == "cache") {
                std::string level;
                iss >> level;

                size_t size, block, assoc;
                iss >> size >> block >> assoc;

                if (level == "l1") l1.configure(size, block, assoc);
                else if (level == "l2") l2.configure(size, block, assoc);

                std::cout << level << " cache configured\n";
            }
        }

        else if (cmd == "malloc") {
            if (!pm) { std::cout << "Initialize memory first\n"; continue; }

            size_t size;
            iss >> size;
            size_t id = pm->allocate(size);
            if (id == 0) std::cout << "Allocation failed\n";
            else std::cout << "Allocated block id=" << id << "\n";
        }

        else if (cmd == "free") {
            if (!pm) { std::cout << "Initialize memory first\n"; continue; }

            size_t id;
            iss >> id;
            if (pm->deallocate(id))
                std::cout << "Block " << id << " freed\n";
            else
                std::cout << "Invalid block id\n";
        }

        else if (cmd == "dump") {
            if (pm) pm->dump();
        }

        else if (cmd == "stats") {
            if (pm) pm->stats();
        }

        else if (cmd == "access") {
            if (!pm) { std::cout << "Initialize memory first\n"; continue; }

            size_t id, offset;
            iss >> id >> offset;

            try {
                auto block = pm->get_block(id);
                if (offset >= block.size) {
                    std::cout << "Offset out of bounds\n";
                    continue;
                }

                size_t addr = block.start + offset;

                if (l1.access(addr)) {
                    total_cycles += 1;
                    std::cout << "L1 hit (1 cycle)\n";
                }
                else if (l2.access(addr)) {
                    total_cycles += 10;
                    std::cout << "L2 hit (10 cycles)\n";
                }
                else {
                    total_cycles += 100;
                    std::cout << "Memory access (100 cycles)\n";
                }

            }
            catch (const std::exception&) {
                std::cout << "Invalid block id\n";
            }
        }

        else if (cmd == "cache") {
            std::string sub;
            iss >> sub;
            if (sub == "stats") {
               size_t l1_access = l1.get_hits() + l1.get_misses();
                size_t l2_access = l2.get_hits() + l2.get_misses();

                double l1_ratio = l1_access ? (double)l1.get_hits() / l1_access * 100.0 : 0;
                double l2_ratio = l2_access ? (double)l2.get_hits() / l2_access * 100.0 : 0;

                std::cout << "L1 hits: " << l1.get_hits()
                        << ", misses: " << l1.get_misses()
                        << ", hit ratio: " << l1_ratio << "%\n";

                std::cout << "L2 hits: " << l2.get_hits()
                        << ", misses: " << l2.get_misses()
                        << ", hit ratio: " << l2_ratio << "%\n";

                std::cout << "Total cycles: " << total_cycles << "\n";
            }
        }

        else {
            std::cout << "Unknown command\n";
        }
    }

    delete pm;
    return 0;
}
