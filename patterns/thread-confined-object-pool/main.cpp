#include "../ScopeBenchmarker.hpp"
#include "FixedObjectPool.hpp"
#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

using namespace ragc;

// Simulates a concrete task execution block passed across a pipeline
struct TaskToken
{
    uint64_t task_id;
    uint32_t priority;
    char payload[16];

    // Explicit constructor to handle perfect forwarding safely without narrowing or array-decay bugs
    TaskToken(uint64_t id, uint32_t prio, std::string_view plat_payload) noexcept
        : task_id(id)
        , priority(prio)
    {
        // Zero out the buffer safely
        std::fill(std::begin(payload), std::end(payload), '\0');

        // Copy string safely without running past bounds
        const size_t bytes_to_copy = std::min(plat_payload.size(), sizeof(payload) - 1);
        std::copy_n(plat_payload.data(), bytes_to_copy, payload);
    }
};

constexpr size_t TASKS_PER_THREAD = 100'000;
constexpr size_t THREAD_COUNT = 4;

// Prevent compiler from optimization-stripping tasks
inline void process_task(TaskToken *token) noexcept
{
#if defined(_MSC_VER)
    (void)token;
#else
    asm volatile("" : : "g"(token) : "memory");
#endif
}

// --- TEST 1: Traditional Shared Heap Arena ---
void run_heap_worker()
{
    for (size_t i = 0; i < TASKS_PER_THREAD; ++i)
    {
        // High lock contention point: threads hit global malloc concurrently
        TaskToken *task = new TaskToken{i, 1, "work payload"};
        process_task(task);
    }
}

void benchmark_traditional_heap_contention()
{
    std::vector<std::thread> workers;
    workers.reserve(THREAD_COUNT);

    {
        ScopeBenchmarker<BenchmarkMode::WallTimeNs> timer("Traditional Shared Heap Contention");

        for (size_t i = 0; i < THREAD_COUNT; ++i)
        {
            workers.emplace_back(run_heap_worker);
        }

        for (auto &worker : workers)
        {
            worker.join();
        }
    }
}

// --- TEST 2: Thread-Local Zero-Contention Object Pools ---
void run_pool_worker()
{
    // Thread-local pool ensures no other core will ever touch this allocation track
    thread_local FixedObjectPool<TaskToken, TASKS_PER_THREAD> local_pool;

    for (size_t i = 0; i < TASKS_PER_THREAD; ++i)
    {
        // Zero-allocation, zero-lock O(1) state shift
        TaskToken *task = local_pool.acquire(i, 1, "work_payload");
        process_task(task);
        local_pool.release(task);
    }
}

void benchmark_optimized_thread_local_pools()
{
    std::vector<std::thread> workers;
    workers.reserve(THREAD_COUNT);

    {
        ScopeBenchmarker<BenchmarkMode::WallTimeNs> timer("Optimized Thread-Local Pools");

        for (size_t i = 0; i < THREAD_COUNT; ++i)
        {
            workers.emplace_back(run_pool_worker);
        }

        for (auto &worker : workers)
        {
            worker.join();
        }
    }
}

int main()
{
    std::cout << "--- Executing Multi-Threaded Task Pipeline Benchmark ---\n";
    std::cout << "Total Active Parallel Threads: " << THREAD_COUNT << "\n";
    std::cout << "Tasks Transacted per Thread: " << TASKS_PER_THREAD << "\n\n";

    // Warm-up phase to ensure system caches are awake
    {
        thread_local FixedObjectPool<TaskToken, 10> warm_pool;
        auto *t = warm_pool.acquire(0, 0, "");
        warm_pool.release(t);
    }

    // Execute comparative runs
    benchmark_traditional_heap_contention();
    benchmark_optimized_thread_local_pools();

    return 0;
}
