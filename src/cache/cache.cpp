#include "cache/cache.h"

Cache::Cache() {}

void Cache::configure(size_t csize, size_t bsize, size_t a)
{
    cache_size = csize;
    block_size = bsize;
    assoc = a;

    size_t num_lines = cache_size / block_size;
    num_sets = num_lines / assoc;

    sets.assign(num_sets, std::vector<CacheLine>(assoc));
    fifo_queues.assign(num_sets, std::queue<size_t>());

    hits = misses = 0;
}

bool Cache::access(size_t address)
{
    if (cache_size == 0)
        return false;

    size_t block_addr = address / block_size;
    size_t set_index = block_addr % num_sets;
    size_t tag = block_addr / num_sets;

    auto &set = sets[set_index];

    for (auto &line : set)
    {
        if (line.valid && line.tag == tag)
        {
            hits++;
            return true;
        }
    }

    misses++;

    // Find empty line
    for (size_t i = 0; i < assoc; ++i)
    {
        if (!set[i].valid)
        {
            set[i] = {true, tag};
            fifo_queues[set_index].push(i);
            return false;
        }
    }

    // FIFO replacement
    size_t victim = fifo_queues[set_index].front();
    fifo_queues[set_index].pop();

    set[victim] = {true, tag};
    fifo_queues[set_index].push(victim);

    return false;
}

size_t Cache::get_hits() const { return hits; }
size_t Cache::get_misses() const { return misses; }
size_t Cache::get_accesses() const { return hits + misses; }
size_t Cache::get_latency() const { return latency; }

void Cache::reset()
{
    hits = 0;
    misses = 0;
    sets.clear();
    fifo_queues.clear();
    cache_size = 0;
    block_size = 0;
    assoc = 0;
    num_sets = 0;
}
