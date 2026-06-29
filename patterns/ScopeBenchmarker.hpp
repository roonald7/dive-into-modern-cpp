#pragma once

#include <chrono>
#include <iostream>
#include <string_view>
#include <type_traits>

// Platform & Architecture Detection Matrix
#if defined(_MSC_VER)
#include <intrin.h> // MSVC Intrinsics for both x86 and ARM64
#elif defined(__x86_64__) || defined(_M_X64)
#include <x86intrin.h> // GCC/Clang Intel Intrinsics
#elif defined(__APPLE__) && defined(__aarch64__)
#include <mach/mach_time.h> // Fallback or direct macOS Mach timers
#endif

// Universal Structural Compiler Barrier
#if defined(_MSC_VER)
#define COMPILER_BARRIER() _ReadWriteBarrier()
#else
#define COMPILER_BARRIER() asm volatile("" ::: "memory")
#endif

namespace ragc
{

enum class BenchmarkMode
{
    WallTimeNs,
    CpuCycles
};

template <BenchmarkMode Mode>
class ScopeBenchmarker
{
private:
    std::string_view name_;

    using TimePoint = std::chrono::steady_clock::time_point;
    [[no_unique_address]] typename std::conditional_t<Mode == BenchmarkMode::WallTimeNs, TimePoint, uint64_t> start_point_;

    // Universal Serialization Clock Read
    static inline uint64_t read_hardware_counter() noexcept
    {
        // --- 1. Windows (MSVC) Implementation ---
#if defined(_MSC_VER)
        return __rdtsc();
        // --- 2. Linux / GCC / Clang (x86_64) Implementation ---
#elif defined(__x86_64__) || defined(_M_X64)
        unsigned int aux;
        return __rdtsc(&aux);
        // --- 3. ARM64 (Apple Silicon / Linux ARM) Implementation ---
#elif defined(__aarch64__)
        uint64_t vct;
        // Accessing the Virtual Count Register directly from EL0 (User Space)
        asm volatile("mrs %0, cntvct_el0" : "=r"(vct));
        return vct;
        // --- 4. Fallback Architecture Layer ---
#else
        // Fallback to steady clock if hardware cycles are unsupported on the target architecture
        return static_cast<uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());
#endif
    }

public:
    explicit ScopeBenchmarker(std::string_view name) noexcept
        : name_(name)
    {
        if constexpr (Mode == BenchmarkMode::WallTimeNs)
        {
            COMPILER_BARRIER();
            start_point_ = std::chrono::steady_clock::now();
            COMPILER_BARRIER();
        }
        else
        {
            COMPILER_BARRIER();
            start_point_ = read_hardware_counter();
            COMPILER_BARRIER();
        }
    }

    ~ScopeBenchmarker()
    {
        if constexpr (Mode == BenchmarkMode::WallTimeNs)
        {
            COMPILER_BARRIER();
            auto end_time = std::chrono::steady_clock::now();
            COMPILER_BARRIER();

            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_point_).count();
            std::cout << "[PERF] " << name_ << ": " << duration << " ns\n";
        }
        else
        {
            COMPILER_BARRIER();
            uint64_t end_cycles = read_hardware_counter();
            COMPILER_BARRIER();

            uint64_t total_cycles = end_cycles - start_point_;

#if defined(__aarch64__)
            // Explicit platform warning: ARM ticks are reference counter units, not CPU clock cycles
            std::cout << "[PERF] " << name_ << ": " << total_cycles << " ARM ticks\n";
#else
            std::cout << "[PERF] " << name_ << ": " << total_cycles << " cycles\n";
#endif
        }
    }

    ScopeBenchmarker(const ScopeBenchmarker &) = delete;
    ScopeBenchmarker &operator=(const ScopeBenchmarker &) = delete;
    ScopeBenchmarker(ScopeBenchmarker &&) = delete;
    ScopeBenchmarker &operator=(ScopeBenchmarker &&) = delete;
};

} // namespace ragc
