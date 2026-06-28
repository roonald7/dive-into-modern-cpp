# Low-Latency & High-Performance C++ Systems Engineering: Master Curriculum

A comprehensive roadmap designed for systems developers targeting high-frequency trading (HFT), market-making engines, custom networking stacks, and real-time infrastructure.

---

## 1. Hardware Architecture & Mechanical Sympathy

To write code that executes in nanoseconds, you must design for the silicon, not the compiler abstraction.

* **Processor Microarchitecture & Pipelining**
    * Instruction-level parallelism (ILP), superscalar execution, and out-of-order execution (OoO).
    * Speculative execution, branch prediction units (BPU), and avoiding branch misprediction penalties.
    * *System Design Concept:* Instruction cache (I-Cache) pollution. Keeping the hot execution path small enough to fit inside L1I (32KB).
* **The Memory Hierarchy & Cache Coherency**
    * L1D, L2, and L3 cache access latencies (1 ns to ~20 ns) vs. Main Memory (>60 ns).
    * Cache lines (64 bytes), spatial and temporal locality, and hardware data prefetchers.
    * The MESI/MOESI cache coherency protocols. Cost of cache line invalidation across cores.
    * False sharing: Mitigation using `alignas(std::hardware_destructive_interference_size)`.
* **Virtual Memory & Translation Lookaside Buffers (TLB)**
    * Page walks, TLB hits vs. TLB misses, and Page Fault resolution.
    * Configuring and using HugePages (2MB or 1GB) to map large memory arenas without TLB pressure.
* **NUMA (Non-Uniform Memory Access)**
    * Interconnect topologies (Intel UPI, AMD Infinity Fabric) and cross-socket latency penalties.
    * NUMA-aware memory allocation (`numa_alloc_onnode`, `mmap` with `MPOL_BIND`).

---

## 2. Advanced C++ Optimization (C++17/20/23)

Zero runtime overhead, compile-time evaluation, and tight layout control.

* **Compile-Time Execution & Metaprogramming**
    * `constexpr`, `consteval`, and `constinit` for zero-cost runtime execution.
    * Compile-time string parsing (e.g., mapping FIX tags to internal enums at compile time).
    * Template Metaprogramming (TMP), SFINAE, and C++20 Concepts for type-safe, static polymorphism instead of virtual tables (`vptr`/`vtable` tracking).
* **Memory Layout Control & Realignment**
    * Data-Oriented Design (DOD): Transforming Array of Structures (AoS) to Struct of Arrays (SoA) for SIMD and cache alignment.
    * Padding, packing, and memory alignment rules (`alignas`, `alignof`).
* **Move Semantics & Elision**
    * Rvalue references, perfect forwarding (`std::forward`), and guaranteed Copy Elision (RVO/NRVO).
    * Eliminating implicit allocations and hidden copies in complex processing pipelines.
* **Micro-Optimizations & Compiler Control**
    * Branch hinting via C++20 `[[likely]]` and `[[unlikely]]` attributes.
    * Compiler intrinsics (`__builtin_assume_aligned`, `_mm_prefetch`).
    * Inspecting compiler output via Assembly (`-O3`, `-march=native`, `-flto`).

---

## 3. Custom Memory Management (Zero-Allocation Paths)

The hot-path rule: **Never invoke the OS allocator (`malloc`/`new`) after initialization.**

* **High-Performance Allocator Design**
    * Arena/Monotonic allocators: Single block sequential pointer increments for linear lifetimes.
    * Fixed-Size Object Pools: Free-list implementations for constant time O(1) allocations/deallocations.
    * C++17 Polymorphic Memory Resources (`std::pmr`) for isolating container allocations.
* **Lifetime Management**
    * Placement `new` directly into pre-allocated memory pools.
    * Explicit destructor calls without releasing backing raw memory.
    * Tuning `std::unique_ptr` and `std::shared_ptr` (and understanding why standard `std::make_shared` allocations are banned on hot paths due to tracking blocks).

---

## 4. Concurrency, Lock-Free Programming & Synchronization

Thread synchronization without invoking kernel sleep states or bus-locking contentions.

* **Memory Models & Sequential Consistency**
    * Atomic operations (`std::atomic`) and atomic fences (`std::atomic_thread_fence`).
    * The C++ Memory Model: `memory_order_relaxed`, `memory_order_acquire`, `memory_order_release`, and `memory_order_seq_cst`.
    * Acquire-Release semantics for unidirectional and bidirectional thread handoffs.
* **Lock-Free & Wait-Free Data Structures**
    * Single-Producer Single-Consumer (SPSC) lock-free ring buffers (The backbone of HFT pipelines).
    * Multi-Producer Multi-Consumer (MPMC) lock-free queues (e.g., using Compare-And-Swap `std::atomic::compare_exchange_strong`).
    * The LMAX Disruptor pattern: Single-writer principle to eliminate write contention.
* **Spinlocks & Low-Latency Thread Coordination**
    * Implementing custom user-space spinlocks with exponential backoff and PAUSE instructions (`_mm_pause`).
    * Thread pinning via `pthread_setaffinity_np` to isolate threads onto dedicated hardware cores.

---

## 5. Low-Latency Networking, Protocols, & IPC

Bypassing the standard Linux networking stack to eliminate microsecond bottlenecks.

* **Kernel Bypass Architectures**
    * Solarflare EF_VI and OpenOnload: Directly mapping NIC ring buffers into user-space memory.
    * DPDK (Data Plane Development Kit): PMD (Poll Mode Drivers) for high-throughput packet capture.
    * `io_uring` for asynchronous zero-copy system calls.
* **High-Performance Event Demultiplexing**
    * The **Reactor Pattern**: Non-blocking sockets polled via edge-triggered `epoll` or custom hardware polling loops.
    * The **Proactor Pattern**: Completion-based network notification models.
* **Inter-Process Communication (IPC)**
    * POSIX Shared Memory (`shm_open`, `mmap`) for sub-100 nanosecond cross-process telemetry.
    * Unix Domain Sockets (UDS) vs. Shared Memory for local control planes.
* **Financial Protocol Parsing & Zero-Copy Architecture**
    * **Simple Binary Encoding (SBE)**: Parsing by casting network byte streams directly to packed C-structs.
    * **FIX Protocol**: Highly optimized tag-value processing using vector lookups and static string views (`std::string_view`).
    * **ITCH/OUCH**: Binary exchange protocols for market data and order entry.

---

## 6. Linux OS Tuning & Jitter Mitigation

Isolating the operating system so your application controls the hardware completely.

* **CPU Isolation & Core Provisioning**
    * Kernel boot parameters: `isolcpus`, `nohz_full`, and `rcu_nocbs` to evict OS background ticks from engine cores.
    * Disabling Hyper-Threading (SMT) to ensure deterministic execution per physical core.
* **Interrupt Routing & Jitter Reduction**
    * IRQ Affinity: Forcing Network Interface Card (NIC) hardware interrupts away from engine cores via `/proc/irq/`.
    * Mitigating C-states and P-states: Locking CPU frequencies to maximum performance via `cpupower` and disabling power-saving throttling.
    * Eliminating TLB invalidation overshoots caused by page migration or KSM (Kernel Samepage Merging).

---

## 7. Recommended Resources & Benchmark Artifacts

### Core Reading Material
1. **What Every Programmer Should Know About Memory** – *Ulrich Drepper* (The definitive paper on caches, hardware, and memory behavior).
2. **C++ Concurrency in Action** – *Anthony Williams* (Essential for mastering memory orders and atomics).
3. **Systems Performance: Enterprise and the Cloud** – *Brendan Gregg* (The bible for profiling, trace analysis, and kernel bottleneck tracking).
4. **Data-Oriented Design** – *Richard Fabian* (The core philosophy for cache-friendly systems architecture).

### Open-Source Projects to Analyze
* **`facebook/folly`**: Study `ProducerConsumerQueue.h`, `AtomicHashMap.h`, and `small_vector.h`.
* **`LMAX-Exchange/disruptor`**: Read the core architecture whitepaper explaining ring buffers and cache line separation.
* **`envoyproxy/envoy`**: Examine the network filter chain, event loop, and buffer implementations.
* **`Aeron-IO/aeron`**: The industry gold-standard for ultra-low-latency UDP messaging and IPC.
