# dive-into-modern-cpp

> A structured, production-grade deep-dive into **C++17 and C++20** through the lens of low-latency systems engineering.

---

## Purpose

This repository is a personal engineering curriculum targeting the architecture, implementation, and optimization techniques required to build production-grade real-time execution platforms. Every topic is contextualized against the hardware realities of modern systems:

- **High-Frequency Trading (HFT) Engines & Financial Exchanges**
- **Quantitative Infrastructure & Market Data Feed Handlers**
- **Telecom Switching, Low-Latency Middleware & Distributed Systems**
- **Embedded Firmware, RTOS & Edge Processing**

No topic is treated as purely academic. Every concept is mapped to CPU cache behavior, memory layout, instruction-level costs, and measurable latency impact.

---

## Prerequisites

Before diving in, you should already be comfortable with:

| Skill | Assumed Level |
|---|---|
| C++ (general) | Professional |
| CMake build system | Professional |
| Networking: TCP/IP, Sockets | Working knowledge |
| Serialization: gRPC, Protobuf, XML | Working knowledge |
| Linux development environment | Working knowledge |

The curriculum **skips introductory syntax** and goes straight to the engineering depth required in production environments.

---

## Repository Layout

```
/dive-into-modern-cpp
├── GEMINI.md               ← System instructions & full roadmap definition
├── README.md               ← This file
├── phase-1/                ← Modern C++ Foundations      [DONE]
│   ├── value-categories/
│   ├── move-semantics/
│   ├── perfect-forwarding/
│   ├── raii-smart-pointers/
│   ├── lambdas/
│   ├── type-safety/
│   ├── views/
│   ├── coroutines/
│   └── modules/
├── phase-2/                ← Compile-Time Programming    [NEXT]
├── phase-3/                ← Hardware-Aware Memory       [PENDING]
├── phase-4/                ← Core Concurrent Programming [PENDING]
├── phase-5/                ← Ultra Low-Latency Queues   [PENDING]
├── phase-6/                ← High-Performance Networking [PENDING]
├── phase-7/                ← Systems Performance Eng.   [PENDING]
├── phase-8/                ← Financial Infrastructure   [PENDING]
├── phase-9/                ← High-Speed Matching Engines[PENDING]
└── phase-10/               ← Consolidated HFT Platform  [PENDING]
```

Each topic follows this internal structure:

```
/topic-name
├── include/        ← Public header interfaces (.hpp)
├── src/            ← Implementation files (.cpp)
├── tests/          ← Unit and integration tests (Catch2 / GTest)
├── benchmarks/     ← Google Benchmark suites
├── docs/           ← Architecture notes, diagrams, assembly analysis
├── README.md       ← Full topic breakdown (theory + code + analysis)
└── CMakeLists.txt  ← Self-contained build configuration
```

---

## The 10-Phase Roadmap

### Phase 1 — Modern C++ Foundations `[DONE]`

Establishing the language mechanics that everything else builds on. Zero fluff — every concept is examined through the lens of zero-copy data movement and resource lifetime precision.

| Topic | Key Concepts |
|---|---|
| **Value Categories** | lvalues, rvalues, prvalues, xvalues — the grammar of object identity |
| **Move Semantics** | Move constructors, move assignment, move-only types |
| **Perfect Forwarding** | `std::forward`, universal references, forwarding patterns |
| **RAII & Smart Pointers** | `unique_ptr`, `shared_ptr`, custom deleters, resource safety |
| **Lambdas** | Capture mechanics, closure state, inline vs. `std::function` cost |
| **Type Safety Without Heap** | `std::optional`, `std::variant`, `std::any` — stack-local sum types |
| **View Topologies** | `std::span`, Ranges — non-owning contiguous data access |
| **Coroutines** | Async state machines, stackless coroutine mechanics |
| **Modules** | Compilation unit layout, interface partitions, build optimizations |

---

### Phase 2 — Compile-Time Programming `[NEXT]`

Moving computation to the compiler. Zero-overhead abstractions, type-driven validation, and policy-based design all rooted in `constexpr` machinery.

- `constexpr`, `consteval`, `constinit` mechanics
- `static_assert`, Type Traits, `std::integral_constant`
- Template Metaprogramming, Variadic Templates, Fold Expressions
- SFINAE, Detection Idiom, C++20 Concepts & Constraints
- CRTP and Static Polymorphism

**Deliverables:** Compile-time lookup tables, zero-overhead validation layouts, policy-based designs.

---

### Phase 3 — Hardware-Aware Memory Management `[PENDING]`

Understanding how your data layout maps to silicon. Cache line alignment, placement new, and deterministic custom allocators.

- Physical Layout: memory alignment, cache line padding, packing rules
- Placement New and pre-allocated initialization
- Arena Allocators, Fixed-Size Pool Allocators, Object Pools
- Polymorphic Memory Resources (PMR)

**Deliverables:** Production-grade cache-friendly Arena Allocator and Fixed-Size Object Pool.

---

### Phase 4 — Core Concurrent Programming `[PENDING]`

The full cost model of synchronization — from OS thread overhead to atomic memory ordering semantics baked into hardware store buffers.

- OS Threads vs. lightweight execution contexts
- Mutex structures, RAII locks, Condition Variables
- Futures, Promises, Packaged Tasks
- Atomics, store buffers, invalidation queues, C++ Memory Model
- Lock-Free vs. Wait-Free algorithm design

**Deliverables:** High-throughput Thread Pool and Work-Stealing Task Scheduler.

---

### Phase 5 — Ultra Low-Latency Concurrency Structures `[PENDING]`

Eliminating lock overhead entirely. Mechanical sympathy applied to ring buffer design.

- SPSC lock-free queue mechanics
- MPSC layouts for shared event paths
- Cache-line padding to eliminate false sharing
- Atomic read/write head separation

**Deliverables:** Lock-free Ring Buffer, ultra-fast SPSC Queue, Event Dispatcher.

---

### Phase 6 — High-Performance Networking & IO `[PENDING]`

From raw socket API to `io_uring`. Non-blocking IO multiplexing, zero-copy reads, and event-driven protocol parsers.

- TCP, UDP, Raw Socket API optimization
- `epoll` architecture vs. `io_uring`
- Boost.Asio, Reactor and Proactor Patterns

**Deliverables:** Non-blocking TCP server, Binary Protocol Parser, Market Data Feed Handler.

---

### Phase 7 — Deep Systems Performance Engineering `[PENDING]`

Numbers don't lie. Profiling instrumentation, assembly inspection, and hardware counter analysis to validate every optimization claim.

- `perf`, Valgrind, Flame Graphs
- x86-64 assembly reading, branch misprediction tracking
- CPU performance counters: cache misses, IPC, instructions retired

**Deliverables:** Systematic Cache and Allocation Benchmark Suite.

---

### Phase 8 — Financial Infrastructure Systems `[PENDING]`

Domain-specific engineering. From FIX protocol parsing to pre-trade risk pipelines.

- Market Microstructure, Order Books, Price-Time priority
- FIX Protocol messages and Binary Exchange Protocols
- Pre-trade Risk Checks, Tick-to-Trade profiling

**Deliverables:** High-speed FIX Parser, Pre-Trade Risk Router, Exchange Feed Simulator.

---

### Phase 9 — High-Speed Matching Engines `[PENDING]`

Building the core of an exchange. Zero-allocation order management with strict price-time priority execution.

- Array-backed, contiguous Limit Order Book
- Zero-allocation order insertion, updates, cancellations
- Price-Time priority matching core

---

### Phase 10 — Consolidated HFT Platform Simulation `[CAPSTONE]`

All phases chained into a full end-to-end local trading environment:

1. Asynchronous Zero-Allocation Logger
2. Low-Latency Task Thread Pool
3. Lock-Free SPSC/MPSC Communication Rings
4. Custom `epoll`/`io_uring` Event Loop
5. Binary Market Data Feed Handler
6. Fast Array-Backed Limit Order Book
7. Price-Time Priority Matching Engine
8. High-Fidelity Exchange Simulator
9. Inline Tick-to-Trade Execution Pipeline

---

## Engineering Principles Applied Throughout

Every implementation in this repository is evaluated against the following hardware metrics:

| Metric | Criteria |
|---|---|
| **Cache Behavior** | 64-byte line alignment, contiguous layouts, false sharing prevention, NUMA awareness |
| **Execution Path** | Instruction pipelining, `noexcept`/`constexpr` hints, branch reduction, zero-overhead idioms |
| **Allocation Budget** | Zero heap allocations on hot paths, stack size tracking, no lock contention |
| **Atomic Ordering** | Explicit `relaxed`/`acquire-release`/`seq_cst` — never implicit |
| **Profiling Targets** | Latency, Throughput, P99/P99.9 Tail Latency |

---

## Building a Topic

Each topic is a self-contained CMake project. From the topic's directory:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Run tests
./build/tests/<topic>_tests

# Run benchmarks
./build/benchmarks/<topic>_bench --benchmark_format=console
```

> Compiler: **GCC 13+** or **Clang 17+** recommended. Standard: `-std=c++20`.

---

## Profiling & Analysis

Performance claims are never left on theoretical ground. The workflow for every benchmark is:

```bash
# Assembly inspection (GCC)
g++ -std=c++20 -O2 -S -o output.s src/main.cpp

# Hardware counters (Linux)
perf stat -e cache-misses,cache-references,instructions,cycles ./build/bench

# Flame graph generation
perf record -g ./build/bench
perf script | stackcollapse-perf.pl | flamegraph.pl > flame.svg
```

---

## References

Core reference material used throughout the curriculum:

- **C++ Reference**: [cppreference.com](https://en.cppreference.com)
- **Books**: *Effective Modern C++* (Meyers) · *C++ Concurrency in Action* (Williams) · *The Art of Writing Efficient Programs* (Pirkelbauer)
- **Talks**: CppCon · code::dive · ACCU Conference (YouTube)
- **Papers**: ISO C++ Standards Committee Working Papers ([open-std.org](https://www.open-std.org))
- **HFT Domain**: *Trading and Exchanges* (Harris) · Mechanical Sympathy Blog (Martin Thompson)

---

*Built as a structured career pivot into low-latency systems engineering. Every line of code here has a reason grounded in hardware.*
