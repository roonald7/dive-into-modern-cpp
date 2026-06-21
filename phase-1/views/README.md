# Phase 1, Topic 7: View Topologies (`std::span` and Ranges)

## 1. Title
High-Performance Views: Zero-Copy Memory Access with `std::span` and Ranges

## 2. Motivation
In low-latency systems, we often receive large blocks of data (e.g., a 10KB network buffer) but only need to process small chunks of it (e.g., a 64-byte order header). Traditionally, this either required passing raw pointers and sizes separately (`ptr, size`) or creating expensive sub-copies. `std::span` (C++20) provides a type-safe, bounds-aware view of any contiguous memory, enabling zero-copy pipelines.

## 3. Problem Statement
An HFT network handler receives a raw binary packet from the socket.
*   **Old way**: Pass `const char* data, size_t len`. If you miss the size, you get a buffer overflow. If you want to use STL algorithms, you must copy into a `std::vector`.
*   **New way**: Pass a `std::span<const uint8_t>`. It behaves like a container but **owns nothing**. It's just a pointer and a size wrapped in a single, safe object.

## 4. Core Concepts
*   **`std::span<T>`**: A non-owning view of a contiguous sequence of objects.
    *   Works with `std::vector`, `std::array`, C-arrays, and raw pointers.
    *   Allows sub-views: `s.subspan(offset, count)`.
*   **`std::ranges`**: A new way to compose algorithms (C++20).
    *   Instead of `std::sort(v.begin(), v.end())`, use `std::ranges::sort(v)`.
    *   Supports **Views** (lazy evaluation): `v | views::filter(...) | views::transform(...)`.
*   **Contiguity**: `std::span` only works on memory that is physically adjacent (like an array). It won't work on a `std::list`.

## 5. Basic Example
```cpp
#include <iostream>
#include <vector>
#include <span>
#include <numeric>

void print_magnitude(std::span<const int> data) {
    std::cout << "Processing " << data.size() << " elements...\n";
    for (int x : data) std::cout << x << " ";
    std::cout << "\n";
}

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};
    int arr[] = {10, 20, 30};

    print_magnitude(v);          // Implicit conversion from vector
    print_magnitude(arr);        // Implicit conversion from C-array
    print_magnitude({v.data(), 3}); // Custom view of first 3 elements
}
```

## 6. Production Example: Zero-Copy Binary Parser
A typical market data handler parses a header, then a body, then a checksum, all without ever copying the data from the initial buffer.

```cpp
#include <iostream>
#include <span>
#include <vector>
#include <cstdint>

struct Header { uint16_t msg_type; uint16_t length; };

void process_payload(std::span<const uint8_t> payload) {
    std::cout << " [Parser] Processing payload of size: " << payload.size() << "\n";
}

void handle_packet(std::span<const uint8_t> packet) {
    if (packet.size() < sizeof(Header)) return;

    // View the header at the start
    const Header* h = reinterpret_cast<const Header*>(packet.data());
    std::cout << " [Parser] Msg Type: " << h->msg_type << "\n";

    // Create a subspan for the payload (zero copy!)
    auto payload = packet.subspan(sizeof(Header), h->length);
    process_payload(payload);
}

int main() {
    // Simulated network buffer
    std::vector<uint8_t> buffer = {0x01, 0x00, 0x04, 0x00, 0xAA, 0xBB, 0xCC, 0xDD};
    handle_packet(buffer);
}
```

## 7. Performance Considerations
*   **Size**: `std::span` is exactly the size of two pointers (16 bytes on 64-bit). It is passed in registers (extremely fast).
*   **Safety**: While non-owning, it provides `[]` access and `size()`, making it much safer than raw pointers.
*   **No Allocation**: Creating a `subspan` or a `range view` never triggers a heap allocation.

## 8. Low-Latency Perspective
In the "Tick-to-Trade" path, `std::span` is used to pass slices of the "Receive Buffer" into the "Risk Check" and "Match Logic" modules. This ensures the CPU never wastes cycles moving data between buffers. We use **Static Spans** (`std::span<T, N>`) when the size is known at compile-time to enable further compiler optimizations.

## 9. Common Mistakes
1.  **Dangling Spans**: Accessing a span after the underlying container (`vector`) has been destroyed or reallocated.
2.  **Modifying via Const Span**: Trying to write into a `std::span<const T>`.
3.  **Out of Bounds**: Even though it's safer than a pointer, `span[i]` can still crash if `i >= size()`.

## 10. Exercises
1.  **Safety Wrapper**: Write a function that takes a `std::span<int>` and reverses only the middle half of the data.
2.  **Range Pipeline**: Use `std::views` to take a vector of prices, filter those above $100, and multiply them by a tax rate—all in a single lazy-evaluated expression.
3.  **Fixed-size Span**: Implement a function that only accepts a `std::span<int, 4>` (explicitly 4 elements) and verify that it won't compile with other sizes.

## 11. References
*   CppReference: `std::span`, `std::ranges`.
*   "Modern C++ for Low-Latency Systems" - Optimization with Views.
*   ISO C++20 Standard.
