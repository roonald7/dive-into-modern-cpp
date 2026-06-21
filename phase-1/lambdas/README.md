# Phase 1, Topic 5: Lambdas (Captures & Inlining)

## 1. Title
C++ Lambdas: Closures, Capture Mechanics, and Zero-Cost Inlining

## 2. Motivation
Lambdas are one of the most powerful tools in modern C++ for writing clean, expressive, and local code. In low-latency systems, they are essential for defining callbacks, predicates for algorithms, and asynchronous task units. Unlike old-style function pointers, lambdas can "capture" local state and are significantly easier for the compiler to inline.

## 3. Problem Statement
In a real-time system, we often need to bind logic to specific data. For example, an `OrderManager` might want to trigger a specific "Reaction" when a symbol hits a price. Using traditional classes (Functors) for every small piece of logic leads to boilerplate bloat. Using function pointers avoids boilerplate but prevents the compiler from seeing through the pointer, inhibiting inlining and optimization. We need a solution that is both expressive and high-performance.

## 4. Core Concepts
*   **Closure Object**: When you define a lambda, the compiler generates a unique, unnamed class (the functor). The lambda you write is an instance of this class.
*   **Capture Clause `[]`**:
    *   `[=]`: Capture by value (creates copies).
    *   `[&]`: Capture by reference (be careful with lifetimes!).
    *   `[this]`: Capture the current class instance.
    *   **Generalized Lambda Captures (C++14)**: `[ptr = std::move(p)]` - allows moving into a lambda.
*   **`mutable`**: Allows modifying variables captured by value inside the lambda.
*   **Generic Lambdas (C++14)**: `[](auto x) { ... }` - the `operator()` is a template.
*   **Lambda Inlining**: Because every lambda has a unique type, the compiler can directly call the `operator()` without a pointer redirection, enabling aggressive inlining.

## 5. Basic Example
```cpp
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    int threshold = 100;
    std::vector<int> prices = {95, 110, 80, 150};

    // Lambda as a predicate
    auto it = std::find_if(prices.begin(), prices.end(), [threshold](int p) {
        return p > threshold;
    });

    if (it != prices.end()) {
        std::cout << "First price above threshold: " << *it << "\n";
    }
}
```

## 6. Production Example: Market Event Callback
In an HFT engine, we often want to "attach" a specific reaction to a market update.

```cpp
#include <iostream>
#include <functional>
#include <vector>
#include <string>

struct MarketUpdate {
    std::string symbol;
    double price;
};

class PriceMonitor {
public:
    using Callback = std::function<void(const MarketUpdate&)>;

    void on_update(Callback cb) { m_callbacks.push_back(std::move(cb)); }

    void trigger(const MarketUpdate& update) {
        for (auto& cb : m_callbacks) cb(update);
    }

private:
    std::vector<Callback> m_callbacks;
};

int main() {
    PriceMonitor monitor;
    int trigger_count = 0;

    // Capturing by reference to track state
    monitor.on_update([&trigger_count](const MarketUpdate& u) {
        std::cout << " [Event] Received " << u.symbol << " at " << u.price << "\n";
        trigger_count++;
    });

    monitor.trigger({"AAPL", 150.50});
    std::cout << " Total triggers: " << trigger_count << "\n";
}
```

## 7. Performance Considerations
*   **Functor Inlining**: Compilers treat lambdas as structs with an `operator()`. In `std::sort`, using a lambda is often **faster** than a function pointer because the compiler can inline the comparison logic directly into the sort loop.

### Benchmark Results (`bench_main.cpp`)
Sorting **1,000,000** `PriceUpdate` structs (Complex comparison: Price -> Timestamp):
| Comparison Method | Time (ms) | Improvement |
| :--- | :--- | :--- |
| **Function Pointer** | ~340 ms | 1.0x |
| **Lambda** (Inlined) | **~305 ms** | **~1.12x (12% faster)** |

*   **Analysis**: For 1M operations, the lambda saves ~35ms of CPU time. This is because the compiler inlines the lambda's closure directly into the sort algorithm, whereas the function pointer requires a `CALL` instruction and a jump for every single comparison. Use this for all critical-path price-time priority matching logic!
*   **`std::function` Overhead**: In the production example above, `std::function` is a "Heavy" wrapper. It uses **Type Erasure** and often triggers a heap allocation (Small Object Optimization applies, but only for small captures). In ultra-hot paths, we use templates or raw lambdas to avoid `std::function`.
*   **Capture Size**: Every variable you capture by value increases the size of the closure object. Keep captures lean.

## 8. Low-Latency Perspective
In the "Hot Path," avoid `std::function`. Instead, use **Template-based Callbacks** or **Static Dispatch**. Lambdas allow us to write "inline" logic that effectively becomes part of the calling function after the compiler finishes its pass. We also use generic lambdas (`auto`) to avoid unnecessary type conversions.

## 9. Common Mistakes
1.  **Dangling References**: Capturing local variables by reference (`[&]`) and then executing the lambda after the variables have gone out of scope.
2.  **Unnecessary `std::function`**: Using `std::function` when a simple template would suffice, leading to avoidable heap allocations and virtual call overhead.
3.  **Capturing `[=]` by Default**: This can accidentally copy large objects or `this` pointers, leading to hidden performance costs. Be explicit: `[x, &y]`.

## 10. Exercises
1.  **Move-Only Capture**: Create a `std::unique_ptr<int>` and capture it into a lambda using C++14 generalized capture. The lambda should print the value and be executed from a different scope.
2.  **Inlining Benchmark**: Compare the performance of `std::sort` using a lambda vs. a raw function pointer for a large vector of integers.
3.  **Stateful Lambda**: Implement a lambda that acts as a "Counter" (returns 1, 2, 3...) using the `mutable` keyword.

## 11. References
*   *Effective Modern C++* (Items 31-34).
*   ISO C++ Standard (Section [expr.prim.lambda]).
*   CppCon: "Lambdas: From C++11 to C++20" by Arthur O'Dwyer.
