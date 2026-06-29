#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <new>

namespace ragc
{

template <typename T, std::size_t Capacity>
class LowLatencyRingBuffer
{
    // Enforce power-of-two allocation at compile time
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of 2");

private:
    // Constants for hardware cache lines to prevent false sharing
    static constexpr std::size_t CacheLineSize = 64;

    // The data backing storage. Aligned to a cache line boundary.
    alignas(CacheLineSize) std::array<T, Capacity> storage_;

    // --- PRODUCER WRITER CONTEXT ---
    // Tail is modified ONLY by the producer thread.
    alignas(CacheLineSize) std::atomic<std::size_t> tail_{0};

    // Cache local copy of the head index to avoid reading the atomic head_ across caches on every push
    std::size_t cached_head_{0};

    // --- CONSUMER READER CONTEXT ---
    // Head is modified ONLY by the consumer thread.
    alignas(CacheLineSize) std::atomic<std::size_t> head_{0};
    // Cache local copy of the tail index to avoid reading the atomic tail_ across caches on every pop
    std::size_t cached_tail_{0};

    // Bitmask for fast wrapping: index & (Capacity - 1)
    static constexpr std::size_t Mask = Capacity - 1;

public:
    LowLatencyRingBuffer() = default;
    ~LowLatencyRingBuffer() = default;

    // Non-blocking emplace write operation (Zero-Copy Producer side)
    template<typename... Args>
    bool emplace(Args&&... args) {
        
    }

};

} // namespace ragc
