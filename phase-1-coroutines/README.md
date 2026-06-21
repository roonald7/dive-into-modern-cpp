# Phase 1, Topic 8: Coroutines & Async State Machines

## 1. Title
Modern C++ Coroutines: Generators, Awaitables, and Zero-Allocation State Machines

## 2. Motivation
Asynchronous programming in legacy C++ required complex callback chains ("Callback Hell") or excessive thread context-switching. Coroutines (C++20) provide a way to pause and resume execution without blocking a thread. This enables high-concurrency systems (like a gateway handling thousands of connections) to maintain readable code with the performance of a manually optimized state machine.

## 3. Problem Statement
Consider an Exchange Gateway waiting for an "Order Acknowledgment" from a socket.
*   **Old way (Blocking)**: The thread sleeps, wasting CPU cycles and memory for the stack.
*   **Old way (Event-based)**: Split the logic into two functions: `send_order()` and `on_ack_received()`. This fragments the code and makes error handling difficult.
*   **New way (Coroutine)**: `co_await socket.receive()`. The function "pauses" and saves its state on the heap (or stack if optimized), freeing the thread to do other work. When the packet arrives, the function "resumes" exactly where it left off.

## 4. Core Concepts
*   **`co_await`**: Pauses the coroutine until an asynchronous operation completes.
*   **`co_yield`**: Pauses and returns a value (used for Generators).
*   **`co_return`**: Returns a final value and finishes the coroutine.
*   **Promise Object**: Defines how the coroutine behaves (how it starts, stops, and handles results).
*   **Coroutine Handle**: A pointer-like object used to control the coroutine from the outside.
*   **Static vs. Heap**: While coroutines can trigger heap allocations (`operator new`), in high-performance C++, we aim for **Halo Optimization** (Heap Allocation Elision) to keep them zero-overhead.

## 5. Basic Example: A simple Generator
```cpp
#include <iostream>
#include <coroutine>

struct Generator {
    struct promise_type {
        int current_value;
        Generator get_return_object() { return Generator{handle_type::from_promise(*this)}; }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(int value) {
            current_value = value;
            return {};
        }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    using handle_type = std::coroutine_handle<promise_type>;
    handle_type h;

    Generator(handle_type h) : h(h) {}
    ~Generator() { if (h) h.destroy(); }

    int next() {
        h.resume();
        return h.promise().current_value;
    }
};

Generator sequence() {
    for (int i = 0; i < 3; ++i) co_yield i;
}

int main() {
    auto gen = sequence();
    std::cout << gen.next() << "\n"; // 0
    std::cout << gen.next() << "\n"; // 1
}
```

## 6. Production Example: Async Resource Loader
In a simulation or trading backtester, we might load historical data asynchronously.

```cpp
#include <iostream>
#include <coroutine>
#include <future>
#include <thread>

// Simplified Task wrapper
struct Task {
    struct promise_type {
        Task get_return_object() { return {}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() {}
    };
};

struct Awaiter {
    bool await_ready() { return false; }
    void await_suspend(std::coroutine_handle<>) {
        // Simulate async work in a thread
        std::thread([]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout << " [Async] IO Complete\n";
        }).detach();
    }
    void await_resume() {}
};

Task async_work() {
    std::cout << "Starting work...\n";
    co_await Awaiter{};
    std::cout << "Resuming after IO...\n";
}
```

## 7. Performance Considerations
*   **State Size**: The "Coroutine Frame" contains all local variables. Large stacks lead to large frames.
*   **Allocation**: By default, coroutine frames are on the heap. In HFT, we use custom allocators or rely on compiler elision (HALO).
*   **Resume Overhead**: Resuming a coroutine is essentially a function call through a pointer (low overhead, but not zero).

## 8. Low-Latency Perspective
We use coroutines for **Market Data Replay** systems. Instead of complex state machines with dozens of `enum class State` variables, we write a sequential coroutine. This reduces the cognitive load for developers while maintaining the "pause/resume" performance needed to process millions of historical ticks without blocking the sim-thread.

## 9. Common Mistakes
1.  **Dangling Handles**: Forgetting to destroy the coroutine handle, causing a memory leak.
2.  **Yielding References to Locals**: Just like regular functions, yielding a reference to a local variable that will be destroyed on suspension is a bug.
3.  **Assuming Serial Execution**: Coroutines can resume on different threads depending on the "Executor" or "Awaiter" logic.

## 10. Exercises
1.  **Infinite Generator**: Implement a coroutine that yields Fibonacci numbers indefinitely.
2.  **Wait for All**: Implement a simple "wait-all" logic where one coroutine awaits multiple asynchronous tasks.
3.  **Manual Resume**: Write a test that manually resumes a coroutine handle and inspects the internal state of the promise.

## 11. References
*   CppReference: Coroutines.
*   "C++20 Coroutines" by Rainer Grimm.
*   "Coroutines: A Negative-Overhead Abstraction" (CppCon).
*   ISO C++20 Standard.
