# MemSysSim — Memory System Simulator

MemSysSim is a modular C++ simulator for studying **memory allocation strategies**, **cache hierarchies**, and **memory system behavior** in operating systems.  
The project is designed to be extensible, with scope for adding **virtual memory** components in the future.

---

## Features

### Memory Allocation

- First Fit
- Best Fit
- Worst Fit
- Buddy Allocation
- Block splitting and coalescing
- Internal and external fragmentation tracking

### Cache Simulation

- Set-associative cache
- Multi-level cache hierarchy (L1 → L2 → Memory)
- Replacement policies:
  - FIFO
  - LRU
  - LFU
- Cache invalidation on memory deallocation

### Statistics & Reporting

- Allocation success/failure rates
- Memory utilization
- Internal and external fragmentation
- Cache hit/miss counts and hit ratio per level

---

## Project Structure

```txt
MemSysSim/
├── include/            # Header files
│   ├── memory.h
│   └── cache.h
├── src/                # Source files
│   ├── memory.cpp
│   ├── cache.cpp
│   └── main.cpp
├── bin/                # Generated executable (ignored by git)
├── Makefile
├── report.md           # Design documentation
├── report.pdf          # Project report
└── .gitignore
```

---

## Build Instructions

### Requirements

- GNU Make
- `g++` (C++14 compatible)
- Windows (PowerShell / CMD) **or** Unix-like environment (Linux / MSYS2 / Git Bash)

---

### Build

```bash
make
```

The executable will be generated at:

```bash
bin/memsim.exe
```

---

### Run

```bash
make run
```

---

### Clean

```bash
make clean
```

This removes the generated executable.

---

## Notes on Cross-Platform Support

- The Makefile supports both **Windows** and **Unix-like** environments using OS-specific commands internally.
- The `bin/` directory and generated binaries are excluded via `.gitignore`.
- Editor-specific configuration files are not tracked to keep the repository clean.

---

## Usage (Interactive Commands)

After launching the simulator, the following commands are available:

- `malloc SIZE` — Allocate a memory block
- `free ID` — Free an allocated block
- `access ADDRESS` — Access a memory address (cache lookup)
- `dump` — Display memory layout
- `stats` — Show memory and cache statistics
- `set cache POLICY` — Set cache replacement policy (`fifo`, `lru`, `lfu`)
- `set memory POLICY` — Set memory allocator (`first_fit`, `best_fit`, `worst_fit`)
- `reinit` — Reinitialize the entire system
- `help` — Display command help
- `exit` — Exit the simulator

---

## Design Overview

- Main memory is modeled as a **contiguous address space**
- Non-buddy allocation uses a **linked list of blocks**
- Buddy allocator manages memory in **power-of-two blocks**
- Cache uses **set-associative mapping** with configurable replacement policies
- Cache lines are invalidated when underlying memory regions are freed

Detailed design explanations are available in `report.md`.

---

## Future Work

- Virtual memory simulation (paging, page tables, TLB)
- Write-back / write-through cache policies
- Dirty bit handling
- Timing and latency modeling
- Multi-threaded access simulation

---

## License

This project is licensed under the [MIT License](LICENSE).
