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
├── .git/ 
├── .vscode/ 
│   ├── c_cpp_properities.json
│   └── c_cpp_properities.json.sample
├── bin/                # Generated executable (ignored by git)
│   └── memsim.exe
├── include/            # Header files
│   ├── cache.h
│   └── memory.h
├── src/                # Source files
│   ├── cache.cpp
│   ├── main.cpp
│   └── memory.cpp
├── tests/              # Sample input-output simulation
│   └── sample_input_output_workload.txt
├── Video/              # Video explaning project and test script
│   └── Demo.mp4
├──.gitignore
├── LICENSE
├── Makefile
├── README.md           
├── report.md           # Design documentation
└── report.pdf          # Project report
```

---

## Build Instructions

### Requirements

- GNU Make
- `g++` (C++17 compatible)
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

## Test Artifacts & Validation

This project includes a comprehensive interactive test workload that serves as the primary validation artifact for the simulator.

### Sample Input Workload

A complete end-to-end simulation transcript is provided, covering initialization, memory allocation, cache access, policy changes, and system reinitialization.

**File:** -> `tests/sample_input_workload_with_expected_output.txt`

### Purpose

- Simulates realistic memory allocation patterns
- Logs cache accesses using valid virtual addresses
- Demonstrates cache hit/miss behavior across multiple cache policies
- Exercises allocator edge cases and fragmentation scenarios
- Acts as a correctness reference via observed outputs

### Coverage

The sample workload exercises:

- Memory allocation strategies:
  - First Fit
  - Best Fit
  - Worst Fit
  - Buddy allocation
- Cache replacement policies:
  - FIFO
  - LRU
  - LFU
- Invalid and valid initialization scenarios
- Cache invalidation on memory deallocation
- Internal and external fragmentation behavior
- Full system reinitialization flows

### Expected Output / Correctness Criteria

- Successful and failed allocations match allocator constraints
- Cache hit/miss behavior is consistent with the active replacement policy
- Cache lines overlapping freed memory regions are invalidated
- Fragmentation statistics reflect allocator-specific behavior
- Reinitialization resets memory and cache state correctly

### Notes

- The workload is fully reproducible using the interactive CLI.
- Automated replay scripts may be added in future revisions.

---

### Demonstration Video

For convenience, a short demonstration video is also included under `Video/ folder` that shows how to run the simulator using the provided test workload/script and interpret the output.  
The video walks through the execution flow, highlights key commands, and explains how the observed behavior corresponds to the expected correctness criteria.

---

### VS Code IntelliSense Configuration (Red Squiggles Fix)

When using VS Code, you may see red squiggles under `#include "memory.h"` or `#include "cache.h"` even though the project compiles correctly using the Makefile.  
This happens because the Makefile specifies the `include/` directory during compilation, but VS Code’s IntelliSense does not automatically pick up compiler include paths.

#### How to Fix

1. Open the Command Palette: `Ctrl + Shift + P`
2. Select: `C/C++: Edit Configurations (JSON)`
3. In `c_cpp_properties.json`, add the following entry to `includePath`:

```json
"${workspaceFolder}/include/"
```

A sample c_cpp_properties.json file demonstrating this configuration is provided for reference.
File -> `.vscode/c_cpp_properties.json.sample`
This change only affects editor IntelliSense and has no impact on the actual build process.

## Future Work

- Virtual memory simulation (paging, page tables, TLB)
- Write-back / write-through cache policies
- Dirty bit handling
- Timing and latency modeling
- Multi-threaded access simulation

---

## License

This project is licensed under the [MIT License](LICENSE).
