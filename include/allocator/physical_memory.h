#pragma once
#include <cstddef>
#include <vector>
#include <list>
#include <map>
#include <cstdint>

enum class FitStrategy {
    FIRST_FIT,
    BEST_FIT,
    WORST_FIT
};

struct MemoryBlock {
    size_t start;
    size_t size;
};

struct AllocatedBlock {
    size_t id;
    size_t start;
    size_t size;
    size_t requested;
};

class PhysicalMemory {
public:
    explicit PhysicalMemory(size_t total_size);

    size_t allocate(size_t size);
    bool deallocate(size_t id);

    AllocatedBlock get_block(size_t id) const;

    void dump() const;
    void stats() const;

    void set_strategy(FitStrategy s);

private:
    size_t total_size;
    std::vector<uint8_t> memory;
    std::list<MemoryBlock> free_list;
    std::map<size_t, AllocatedBlock> allocated;
    size_t next_id = 1;
    FitStrategy strategy = FitStrategy::FIRST_FIT;

    size_t total_requests = 0;
    size_t successful_requests = 0;
    size_t total_requested_bytes = 0;

    std::list<MemoryBlock>::iterator find_block(size_t size);
    void coalesce();
};
