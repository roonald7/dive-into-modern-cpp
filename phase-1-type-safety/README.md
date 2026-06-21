# Phase 1, Topic 6: Type Safety Without Heap (`std::optional`, `std::variant`)

## 1. Title
Type-Safe Low-Latency Programming: `std::optional`, `std::variant`, and `std::any`

## 2. Motivation
In legacy C++, we often used "magic values" to indicate the absence of a value (e.g., returning `-1.0` for a missing price or `nullptr` for a missing object). This is error-prone and often forces unnecessary heap allocations (to use `nullptr`). Modern C++ provides stack-based, type-safe contains that tell the compiler exactly what the data represents, leading to better optimization and fewer crashes.

## 3. Problem Statement
In an Order Book, some orders might not have a "Stop Price." 
*   **Old way**: Use a `double* stop_price` (requires a heap allocation or complex pointer management) or a magic value `0.0` (ambiguous).
*   **New way**: Use `std::optional<double>`.
Similarly, a "Market Event" could be a Trade, a Quote, or a Gap. 
*   **Old way**: A base class `Event` with virtual functions (virtual call overhead).
*   **New way**: A `std::variant<Trade, Quote, Gap>` (Static polymorphism, stack-allocated, zero virtual overhead).

## 4. Core Concepts
*   **`std::optional<T>`**: Represents a value that may or may not exist. It contains the storage for `T` internally (on the stack).
*   **`std::variant<T, U, ...>`**: A type-safe `union`. It knows which type it currently holds.
*   **`std::visit`**: The standard way to "handle" a variant. It uses compile-time dispatch (often a jump table), which is much faster than `if/else` checks.
*   **`std::any`**: A type-safe container for *any* type. Note: Unlike the others, `std::any` usually triggers a heap allocation and is rarely used in ultra-hot paths.

## 5. Basic Example
```cpp
#include <iostream>
#include <optional>
#include <variant>

std::optional<double> get_mid_price(double bid, double ask) {
    if (bid <= 0 || ask <= 0) return std::nullopt;
    return (bid + ask) / 2.0;
}

int main() {
    auto mid = get_mid_price(150.0, 150.10);
    if (mid) std::cout << "Mid: " << *mid << "\n";

    std::variant<int, std::string> v = "Hello";
    std::cout << "Variant index: " << v.index() << "\n";
}
```

## 6. Production Example: Type-Safe Command Dispatcher
In HFT, we process different message types using a "Visitor" pattern to avoid virtual function overhead.

```cpp
#include <iostream>
#include <variant>
#include <vector>
#include <string>

struct NewOrder { uint64_t id; double price; };
struct CancelOrder { uint64_t id; };
struct ResetExchange { std::string reason; };

using Command = std::variant<NewOrder, CancelOrder, ResetExchange>;

struct CommandHandler {
    void operator()(const NewOrder& o) { std::cout << " [EXEC] New Order: " << o.id << "\n"; }
    void operator()(const CancelOrder& o) { std::cout << " [EXEC] Cancel: " << o.id << "\n"; }
    void operator()(const ResetExchange& r) { std::cout << " [SYS] Reset: " << r.reason << "\n"; }
};

int main() {
    std::vector<Command> queue;
    queue.emplace_back(NewOrder{101, 150.5});
    queue.emplace_back(CancelOrder{101});
    queue.emplace_back(ResetExchange{"End of Day"});

    CommandHandler handler;
    for (const auto& cmd : queue) {
        std::visit(handler, cmd);
    }
}
```

## 7. Performance Considerations
*   **Memory Layout**: `std::optional<T>` takes `sizeof(T) + alignof(T)` (roughly). It is always on the stack.
*   **Visitor Dispatch**: `std::visit` is typically implemented as a table of function pointers. For common variants, the compiler can often inline this into a simple `switch`.
*   **Alignment**: `std::variant` size is determined by the largest member plus some padding for the "type index."

## 8. Low-Latency Perspective
In the matching engine core, we use `std::variant` to implement the **State Machine** for orders (e.g., `New`, `PartiallyFilled`, `Filled`, `Rejected`). This keeps the order data contiguous in memory and avoids the instruction cache misses associated with virtual inheritance. We use `std::optional` for optional fields like `expire_time` to avoid pointer chasing.

## 9. Common Mistakes
1.  **Accessing `optional` without checking**: Using `*opt` when it is empty causes undefined behavior. Use `.value_or()` or check `.has_value()`.
2.  **Using `std::variant` for large hierarchies**: If you have 50 different message types, a variant might become very large, wasting memory for smaller messages.
3.  **`std::any` on Hot Path**: Never use `std::any` where latency matters. The type-erasure and heap allocation are significant bottlenecks.

## 10. Exercises
1.  **Safety First**: Implement a function that parses a string to an integer and returns `std::optional<int>`. Handle errors like non-numeric characters.
2.  **Static Polymorphism**: Create a `std::variant` of `Circle`, `Square`, and `Triangle`. Use `std::visit` to calculate the total area of a `std::vector` of these shapes.
3.  **Visitor Overloading**: Use the `overloaded` pattern (using `template<class... Ts> struct overloaded : Ts...`) to handle a variant with inline lambdas.

## 11. References
*   *Effective Modern C++*.
*   CppReference: `std::optional`, `std::variant`.
*   "Modern C++ Design Patterns" - Visitor with `std::variant`.
