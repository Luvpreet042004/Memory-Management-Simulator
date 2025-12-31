#include "allocator/physical_memory.h"
#include <iostream>
#include <algorithm>

PhysicalMemory::PhysicalMemory(size_t total)
    : total_size(total), memory(total)
{
    free_list.push_back({0, total});
}

void PhysicalMemory::set_strategy(FitStrategy s) {
    strategy = s;
}

std::list<MemoryBlock>::iterator PhysicalMemory::find_block(size_t size) {
    if (strategy == FitStrategy::FIRST_FIT) {
        return std::find_if(free_list.begin(), free_list.end(),
                            [size](auto& b) { return b.size >= size; });
    }

    auto best = free_list.end();
    for (auto it = free_list.begin(); it != free_list.end(); ++it) {
        if (it->size >= size) {
            if (best == free_list.end() ||
                (strategy == FitStrategy::BEST_FIT && it->size < best->size) ||
                (strategy == FitStrategy::WORST_FIT && it->size > best->size)) {
                best = it;
            }
        }
    }
    return best;
}

size_t PhysicalMemory::allocate(size_t size) {
    total_requests++;

    auto it = find_block(size);
    if (it == free_list.end()) return 0;

    size_t start = it->start;
    it->start += size;
    it->size -= size;
    if (it->size == 0) free_list.erase(it);

    size_t id = next_id++;
    allocated[id] = {id, start, size, size};

    successful_requests++;
    total_requested_bytes += size;

    return id;
}


bool PhysicalMemory::deallocate(size_t id) {
    auto it = allocated.find(id);
    if (it == allocated.end()) return false;

    free_list.push_back({it->second.start, it->second.size});
    allocated.erase(it);
    coalesce();
    return true;
}

AllocatedBlock PhysicalMemory::get_block(size_t id) const {
    auto it = allocated.find(id);
    if (it == allocated.end()) {
        throw std::runtime_error("Invalid block id");
    }
    return it->second;
}

void PhysicalMemory::coalesce() {
    free_list.sort([](auto& a, auto& b) { return a.start < b.start; });

    for (auto it = free_list.begin(); it != free_list.end();) {
        auto next = std::next(it);
        if (next != free_list.end() && it->start + it->size == next->start) {
            it->size += next->size;
            free_list.erase(next);
        } else {
            ++it;
        }
    }
}

void PhysicalMemory::dump() const {
    std::cout << "Memory dump:\n";

    for (const auto& b : free_list)
        std::cout << "[0x" << std::hex << b.start
                  << " - 0x" << (b.start + b.size - 1)
                  << "] FREE\n";

    for (const auto& [_, b] : allocated)
        std::cout << "[0x" << std::hex << b.start
                  << " - 0x" << (b.start + b.size - 1)
                  << "] USED (id=" << std::dec << b.id << ")\n";
}

void PhysicalMemory::stats() const {
    size_t used = 0;
    for (auto& [_, b] : allocated) used += b.size;

    size_t free = total_size - used;

    size_t largest_free = 0;
    for (auto& b : free_list)
        largest_free = std::max(largest_free, b.size);

    double utilization = (double)used / total_size * 100.0;
    double external_frag = free == 0 ? 0.0 : (1.0 - (double)largest_free / free) * 100.0;
    double internal_frag = allocated.empty() ? 0.0 :
        (double)(used - total_requested_bytes) / used * 100.0;
    double success_rate = total_requests == 0 ? 0.0 :
        (double)successful_requests / total_requests * 100.0;

    std::cout << "Total memory: " << total_size << "\n";
    std::cout << "Used memory: " << used << "\n";
    std::cout << "Free memory: " << free << "\n";
    std::cout << "Utilization: " << utilization << "%\n";
    std::cout << "External fragmentation: " << external_frag << "%\n";
    std::cout << "Internal fragmentation: " << internal_frag << "%\n";
    std::cout << "Allocation success rate: " << success_rate << "%\n";
}

