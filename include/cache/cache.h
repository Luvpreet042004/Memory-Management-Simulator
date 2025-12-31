#pragma once
#include <vector>
#include <cstddef>
#include <queue>

struct CacheLine {
    bool valid = false;
    size_t tag = 0;
};

class Cache {
public:
    Cache();

    void configure(size_t cache_size, size_t block_size, size_t assoc);

    bool access(size_t address);

    size_t get_hits() const;
    size_t get_misses() const;

    size_t get_accesses() const;
    size_t get_latency() const;

private:
    size_t cache_size = 0;
    size_t block_size = 0;
    size_t assoc = 1;
    size_t num_sets = 0;
    size_t latency = 0;

    std::vector<std::vector<CacheLine>> sets;
    std::vector<std::queue<size_t>> fifo_queues;

    size_t hits = 0;
    size_t misses = 0;
};
