# 🧠 Memory Simulator Visualization

A graphical memory management simulator built with **C++ and Qt**, designed to help visualize and understand how physical memory allocation and cache hierarchies work in operating systems.

This tool allows users to experiment with allocation strategies, cache behavior, and memory access patterns through an interactive desktop application.

---

## 🚀 Features

### Physical Memory Simulation
- Initialize memory with any size
- Allocate and deallocate memory blocks
- Supports allocation strategies:
  - First Fit
  - Best Fit
  - Worst Fit
- Automatic merging (coalescing) of free blocks

### Cache Simulation
- Configurable L1 and L2 caches
- Adjustable:
  - Cache size
  - Block size
  - Associativity
- Tracks hits, misses, and total cycles

### Visualization
- Real-time memory map with block IDs
- Color-coded allocated and free blocks
- Visual updates on every operation

### Statistics Panel
- Used vs total memory
- Cache hit/miss counters
- Total memory access cycles

### Interactive GUI
- Windows desktop application
- Controls for initialize, malloc, free, access, cache config
- Console-style operation log
- Reset button to restart simulation

---

## 🖼️ Screenshot

*(Add screenshots here)*  
Example:
```md
![Memory Simulator](screenshots/main.png)
