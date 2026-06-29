#pragma once

#include <array>
#include <cstddef>
#include <new>
#include <stdexcept>
#include <utility>

namespace ragc
{

template <typename T, std::size_t Size>
class FixedObjectPool
{
private:
    union Node
    {
        T storage;
        std::size_t next_free_idx;

        Node() {};
        ~Node() {};
    };

    alignas(64) std::array<Node, Size> pool_; // Align to cache line boundary
    std::size_t next_available_idx_{0};

public:
    FixedObjectPool()
    {
        // Initialize the free list linking each element to the next index
        for (std::size_t i = 0; i < Size - 1; ++i)
        {
            pool_[i].next_free_idx = i + 1;
        }

        pool_[Size - 1].next_free_idx = static_cast<std::size_t>(-1); // Sentinel value for End
    }

    template <typename... Args>
    T *acquire(Args &&...args)
    {
        if (next_available_idx_ == static_cast<std::size_t>(-1)) [[unlikely]]
        {
            return nullptr;
        }

        std::size_t current_idx = next_available_idx_;
        Node &node = pool_[current_idx];

        // Advance the free list head before constructing the object
        next_available_idx_ = node.next_free_idx;

        // Construct object in-place over the union slot
        T *obj = ::new (static_cast<void *>(&node.storage)) T(std::forward<Args>(args)...);
        return obj;
    }

    void release(T *obj)
    {
        if (!obj) [[unlikely]]
        {
            return;
        }
        // Pointer arithmetic to deduce index without metadata overhead
        Node *node_ptr = reinterpret_cast<Node *>(obj);

        std::size_t idx = node_ptr - pool_.data();

        // Call explicit destructor without releasing backing slot memory
        obj->~T();

        // Push slot back to the head of the free list
        node_ptr->next_free_idx = next_available_idx_;
        next_available_idx_ = idx;
    }
};

} // namespace ragc
