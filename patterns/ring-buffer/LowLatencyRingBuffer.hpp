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
    template <typename... Args>
    bool emplace(Args &&...args) noexcept
    {
        const std::size_t current_tail = tail_.load(std::memory_order_relaxed);

        if ((current_tail - cached_head_) == Capacity)
        {
            cached_head_ = head_.load(std::memory_order_acquire);

            if ((current_tail - cached_head_) == Capacity)
            {
                return false; // Structurally full, apply backpressure up-stream
            }
        }

        // Write directly into the contiguous slot using bitwise masking instead of modulo (%)
        storage_[current_tail & Mask] = T{std::forward<Args>(args)...};

        // Publish the item to the consumer.
        // memory_order_release guarantees the data writes are visible BEFORE the tail increments.
        tail_.store(current_tail + 1, std::memory_order_release);
        return true;
    }

    // Non-blocking pop read operation (Consumer side)
    bool pop(T &value) noexcept
    {
        const std::size_t current_head = head_.load(std::memory_order_relaxed);

        // Check if queue is empty using our local cached tail value first
        if (current_head == cached_tail_)
        {
            // Only read the true atomic tail_ across the cache bus if the local cache says we are empty
            cached_tail_ = tail_.load(std::memory_order_acquire);
            if (current_head == cached_tail_)
            {
                return false;
            }
        }

        // Retrieve data slot
        value = std::move(storage_[current_head & Mask]);
        // Signal to producer that the slot is now free.
        head_.store(current_head + 1, std::memory_order_relaxed);
        return true;
    }

    LowLatencyRingBuffer(const LowLatencyRingBuffer &) = delete;
    LowLatencyRingBuffer(LowLatencyRingBuffer &&) = delete;
};

} // namespace ragc
