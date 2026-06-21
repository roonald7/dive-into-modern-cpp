# Phase 1, Topic 2: Move Semantics & Move-Only Types

## 1. Title
Move Semantics and Move-Only Types in Modern C++

## 2. Motivation
In low-latency systems, we often need to ensure strict ownership of resources (like memory buffers, socket handles, or file descriptors). Coping these resources is either impossible or extremely expensive. Move semantics allow us to transfer ownership efficiently without copying the underlying data, making it possible to use types that are "move-only."

## 3. Problem Statement
Consider an HFT Matching Engine that receives `Order` objects. If these orders contain pointer-based metadata or strings, copying them between threads or into a history log introduces latency and pressure on the heap allocator. Furthermore, some resources—like a unique connection to an exchange—should never be duplicated. We need a way to pass these around while guaranteeing that only one part of the system owns them at a time.

## 4. Core Concepts
*   **Move Constructor**: A constructor that takes an rvalue reference (`T&&`) and "steals" the resources of the original object.
*   **Move Assignment Operator**: Similar to the constructor, but for existing objects.
*   **Move-Only Types**: Classes that delete the copy constructor and copy assignment operator (e.g., `std::unique_ptr`, `std::thread`, `std::fstream`).
*   **Self-Assignment Check**: Critical in move assignment to prevent resource leaks or crashes when an object is moved to itself.
*   **noexcept**: Move operations should almost always be marked `noexcept` to allow STL containers (like `std::vector`) to use them during reallocations instead of falling back to copies.

## 5. Basic Example
```cpp
#include <iostream>
#include <utility>

class ResourceManager {
public:
    ResourceManager(int id) : m_id(id) { std::cout << "Allocating " << m_id << "\n"; }
    ~ResourceManager() { if(m_id != -1) std::cout << "Freeing " << m_id << "\n"; }

    // Move-only: Delete copies
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // Move Constructor
    ResourceManager(ResourceManager&& other) noexcept : m_id(other.m_id) {
        other.m_id = -1; // Leave in a valid but "empty" state
        std::cout << "Moving " << m_id << "\n";
    }

    // Move Assignment
    ResourceManager& operator=(ResourceManager&& other) noexcept {
        if (this != &other) {
            m_id = other.m_id;
            other.m_id = -1;
        }
        return *this;
    }

private:
    int m_id;
};

int main() {
    ResourceManager res1(101);
    // ResourceManager res2 = res1; // Error: use of deleted function
    ResourceManager res2 = std::move(res1); // Success: Move constructor
}
```

## 6. Production Example: Move-Only Message Queue
In a trading system, messages are often passed between a "Parser" thread and a "Matching" thread. Using `std::unique_ptr` ensures zero-copy and strict ownership transition.

```cpp
#include <iostream>
#include <memory>
#include <vector>
#include <string>

struct InternalMessage {
    uint64_t id;
    std::string payload;

    InternalMessage(uint64_t i, std::string p) : id(i), payload(std::move(p)) {}
};

class Dispatcher {
public:
    // Takes ownership of the message
    void dispatch(std::unique_ptr<InternalMessage> msg) {
        std::cout << "Dispatching Message ID: " << msg->id << "\n";
        m_history.push_back(std::move(msg)); // Move into storage
    }

private:
    std::vector<std::unique_ptr<InternalMessage>> m_history;
};

int main() {
    Dispatcher dispatcher;
    auto msg = std::make_unique<InternalMessage>(42, "BUY 100 AAPL 150.00");

    // dispatcher.dispatch(msg); // ERROR: unique_ptr cannot be copied
    dispatcher.dispatch(std::move(msg)); // OK: ownership transferred
    
    if (!msg) {
        std::cout << "Main: msg is now null, as expected.\n";
    }
}
```

## 7. Performance Considerations
*   **Cost**: A move is typically the cost of copying a few pointers and integers (O(1)).
*   **The `noexcept` Rule**: If your move constructor isn't `noexcept`, `std::vector` will use the copy constructor during reallocation to maintain strong exception guarantees. This is a common performance trap.

### Benchmark Results (`noexcept_bench.cpp`)
Measured with 10,000 iterations and a payload of ~800 bytes:
| Move Category | Time (ms) | Speedup |
| :--- | :--- | :--- |
| **Non-noexcept** (Copy Fallback) | ~8.3 ms | 1.0x |
| **noexcept** (True Move) | **~3.4 ms** | **~2.4x** |

*   **Insight**: In the non-noexcept case, every reallocation triggers fresh heap allocations for every element in the vector. In low-latency systems, this causes significant jitter.
*   **Small Object Optimization (SOO)**: Some types (like `std::string` or `std::function`) might not use the heap for small payloads. Moving them might involve a small `memcpy` rather than just a pointer swap.

## 8. Low-Latency Perspective
In ultra-low latency scenarios (e.g., Tick-to-Trade), we avoid even `std::unique_ptr` if it implies dynamic allocation on the hot path. However, move semantics are still used with **Stack-Allocated** or **Arena-Allocated** objects to pass them through state machines. Move semantics represent "Ownership Handover" which is a logical concept that costs zero instructions if the compiler can inline the transfer.

## 9. Common Mistakes
1.  **Forgetting `noexcept`**: Causes performance regressions in containers.
2.  **Not Nulling Out/Resetting**: Leaving the "source" object with pointers to the resources you just stole (Double Free risk).
3.  **Moving Const Objects**: `std::move(const_obj)` results in a **copy**, not a move, because you can't "steal" (modify) a const object.
4.  **Redundant `std::move` on Return**: Returning a local object by value is already optimized by RVO/NRVO. Adding `std::move` can actually prevent these optimizations.

## 10. Exercises
1.  **Hand-rolled Move**: Create a class `TcpSocket` that manages a simulated integer file descriptor. Make it move-only and ensure the socket is "closed" exactly once.
2.  **Vector Performance**: Write a benchmark comparing `std::vector<T>` reallocations for a class with a `noexcept` move constructor versus one where it is missing.
3.  **Ownership Chain**: Implement a 3-stage pipeline (Source -> Filter -> Sink) where a `unique_ptr` travels through each stage. Print the address of the managed object at each step to prove it never changes.

## 11. References
*   *Effective C++* by Scott Meyers.
*   *C++ Templates: The Complete Guide* (Chapter on Move Semantics).
*   CPPReference: Move Constructors / Move Assignment.
