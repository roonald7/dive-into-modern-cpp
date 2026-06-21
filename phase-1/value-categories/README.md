# Phase 1, Topic 1: Value Categories

## 1. Title
Value Categories in Modern C++ (lvalues, rvalues, prvalues, xvalues)

## 2. Motivation
C++11 and subsequent standards redefined how expressions are categorized to enable **Move Semantics**. In low-latency systems, we must distinguish between objects that have an identity (reside in a memory address we can refer to) and objects that are ephemeral (temporaries). By mastering value categories, we can write code that "steals" resources from temporaries instead of performing expensive copies.

## 3. Problem Statement
In HFT execution pipelines, an `Order` object might travel through multiple layers: Risk Checks -> Matching Gateways -> Network Handlers.
If each layer receives the order by value and creates a copy, the latency accumulates linearly. If we don't understand value categories, we might accidentally trigger copies of large structs (like Order Books or Market Data Snapshots) or fail to leverage move-only types like `std::unique_ptr`, which are essential for zero-overhead resource ownership.

## 4. Core Concepts
The C++ taxonomy of expressions:

*   **glvalue (generalized lvalue)**: An expression whose evaluation determines the identity of an object or function.
    *   **lvalue**: A glvalue that is not an xvalue. (e.g., named variables, functions returning lvalue references).
*   **rvalue**: An expression that can be moved from.
    *   **prvalue (pure rvalue)**: An expression whose evaluation initializes an object or computes a value. (e.g., literals `42`, results of functions returning non-references).
    *   **xvalue (eXpiring value)**: A glvalue that denotes an object whose resources can be reused (typically near the end of its lifetime). (e.g., result of `std::move(var)`).

**The Hierarchy:**
```text
      expression
      /      \
   glvalue   rvalue
   /    \    /    \
lvalue   xvalue   prvalue
```

## 5. Basic Example
```cpp
#include <iostream>
#include <string>
#include <utility>

void process_value(const std::string& msg) {
    std::cout << "Lvalue reference: " << msg << std::endl;
}

void process_value(std::string&& msg) {
    std::cout << "Rvalue reference (can be moved): " << msg << std::endl;
}

int main() {
    std::string text = "Market Data"; // 'text' is an lvalue
    
    process_value(text);               // Calls lvalue overload
    process_value("Price Update");     // "Price Update" is a prvalue -> rvalue overload
    process_value(std::move(text));    // std::move(text) is an xvalue -> rvalue overload
    
    return 0;
}
```

## 6. Production Example: Low-Latency Packet Parser
In a real-world scenario, we might parse a binary packet into a struct. We want to ensure that once the packet is parsed, it can be "moved" into the message queue without copying the internal buffer.

```cpp
#include <iostream>
#include <vector>
#include <cstdint>

struct MarketDataPacket {
    uint64_t timestamp;
    std::vector<double> price_levels; // Dynamic memory (heavy to copy)

    // Move constructor
    MarketDataPacket(MarketDataPacket&& other) noexcept
        : timestamp(other.timestamp), price_levels(std::move(other.price_levels)) {
        std::cout << "[MOVE] Reusing price_levels buffer\n";
    }

    // Copying explicitly discouraged in hot paths
    MarketDataPacket(const MarketDataPacket& other) = default;

    MarketDataPacket(uint64_t ts, size_t levels) : timestamp(ts), price_levels(levels, 0.0) {}
};

class PacketProcessor {
public:
    void submit(MarketDataPacket packet) {
        // Here, 'packet' is an lvalue within this scope, but it was moved/copied into the param
        m_queue.push_back(std::move(packet)); // Move into storage
    }

private:
    std::vector<MarketDataPacket> m_queue;
};

int main() {
    PacketProcessor processor;

    // 1. Passing a prvalue (temporary)
    processor.submit(MarketDataPacket(123456789, 10)); // Zero copy, direct move

    // 2. Passing an xvalue
    MarketDataPacket p(987654321, 5);
    processor.submit(std::move(p)); // Explicit move
}
```

## 7. Performance Considerations
*   **Copy Elision (NRVO/RVO)**: The compiler often eliminates the move/copy entirely when returning a prvalue. In low-latency C++, we rely on this (Guaranteed Copy Elision since C++17). See `src/copy_elision_example.cpp` for a live demonstration where objects are constructed directly in the caller's scope without even a move operation.
*   **Assembly Impact**: Moving a `std::vector` or `std::string` involves a few pointer swaps (O(1)). Copying involves `malloc`/`memcpy` (O(n) + syscall overhead), which is catastrophic in a 5-microsecond matching loop.
*   **Cache Locality**: While moving pointers is fast, the data they point to might be in different cache lines. However, move semantics allow us to keep the same memory address for the payload, maintaining temporal locality.
*   **Container Growth**: Note that even when moving into a `std::vector`, appending elements may trigger reallocations. If the object is move-constructible (and `noexcept`), the vector will move elements to the new block, preserving performance, but still incurring multiple move operations. In ultra-low latency, we prevent this by using `reserve()` or fixed-capacity containers.

## 8. Low-Latency Perspective
In HFT, we often use **Fixed-Size Buffers** or **Arena Allocators**. Even then, move semantics are vital for "transferring ownership" of a slice of memory (via `std::span` or custom views) without duplicating the metadata or the state machines handling those buffers.

## 9. Common Mistakes
1.  **Returning `std::move(local_var)`**: This prevents Named Return Value Optimization (NRVO). Just return the variable.
2.  **Using an object after `std::move`**: The object is in a "valid but unspecified state". In high-performance code, this leads to non-deterministic crashes.
3.  **Over-using `std::move` on primitive types**: Moving an `int` or `double` is just a copy. It adds no value and only makes the code noisier.

## 10. Exercises
1.  **Identify the Category**: In the expression `a = b + c`, identify the value category of `a`, `b`, `c`, and `b + c`.
2.  **Trace Moves**: Implement a class `LatencyBuffer` that tracks the number of times its copy and move constructors are called. Write a test that performs 3 operations but results in 0 copies. (Implemented in `tests/test_main.cpp`).
3.  **Validate Elision**: Use the code in `src/copy_elision_example.cpp`. Add a function that returns a `std::move(local_obj)`. Run it and observe how it **breaks** NRVO and forces a move operation.

## 11. References
*   *Effective Modern C++* by Scott Meyers (Items 23, 24).
*   *C++ Core Guidelines* (F.18: Move-only types, F.19: Copyable types).
*   ISO C++ Standard (Section [basic.lval]).
