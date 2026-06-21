# Phase 1, Topic 4: Resource Management (RAII & Smart Pointers)

## 1. Title
RAII (Resource Acquisition Is Initialization) and Modern Smart Pointers

## 2. Motivation
In low-latency and deterministic systems, memory leaks and "hanging" resources (like unclosed sockets) are fatal. RAII is the core philosophy of C++: binding the lifecycle of a resource to the lifecycle of an object. Smart pointers are the standard implementation of this philosophy for heap memory, ensuring that memory is freed exactly when it's no longer needed, without manual `delete`.

## 3. Problem Statement
Manual memory management (`new`/`delete`) is error-prone. In complex execution paths (e.g., if an exception is thrown or a function returns early), a `delete` call might be skipped, leading to a leak. In a trading engine running for weeks, even a tiny 64-byte leak per order will eventually crash the system. We need deterministic, automated cleanup that doesn't sacrifice performance.

## 4. Core Concepts
*   **RAII**: The constructor acquires the resource; the destructor releases it. This guarantees cleanup via stack unwinding.
*   **`std::unique_ptr<T>`**: Exclusive ownership. Zero runtime overhead compared to a raw pointer. Move-only.
*   **`std::shared_ptr<T>`**: Shared ownership via reference counting. Incurs atomic overhead for increments/decrements.
*   **`std::weak_ptr<T>`**: A non-owning observer of a `shared_ptr`. Prevents circular dependencies (leaks).
*   **Custom Deleters**: Allowing smart pointers to manage resources other than memory (e.g., calling `fclose`, `close()`, or `pclose`).

## 5. Basic Example: Manual RAII
```cpp
#include <iostream>

class FileWrapper {
public:
    FileWrapper(const char* filename) {
        m_file = std::fopen(filename, "w");
        if (!m_file) throw std::runtime_error("Failed to open file");
        std::cout << " [RAII] File opened\n";
    }

    ~FileWrapper() {
        if (m_file) {
            std::fclose(m_file);
            std::cout << " [RAII] File closed via destructor\n";
        }
    }

    void write(const char* text) { std::fputs(text, m_file); }

private:
    std::FILE* m_file;
};

int main() {
    {
        FileWrapper fw("test.txt");
        fw.write("Hello RAII");
    } // Desctructor called automatically here
}
```

## 6. Production Example: Managed Network Handler
In a production system, we often have a `Session` object that must be shared between a `Logger` and an `ExecutionEngine`. 

```cpp
#include <iostream>
#include <memory>
#include <vector>

struct Session {
    int id;
    Session(int i) : id(i) { std::cout << " [Session] " << id << " created\n"; }
    ~Session() { std::cout << " [Session] " << id << " destroyed\n"; }
};

class Logger {
public:
    void add_session(std::shared_ptr<Session> s) { m_sessions.push_back(s); }
private:
    std::vector<std::shared_ptr<Session>> m_sessions;
};

int main() {
    auto logger = std::make_unique<Logger>();
    
    {
        // make_shared is preferred: 1 allocation instead of 2 (metadata + object)
        auto session = std::make_shared<Session>(1001);
        logger->add_session(session);
        std::cout << " Session use count: " << session.use_count() << "\n";
    } // 'session' goes out of scope, but 'logger' still holds a reference

    std::cout << " End of inner scope.\n";
    logger.reset(); // Logger destroyed -> Session destroyed
}
```

## 7. Performance Considerations
*   **`unique_ptr`**: **Zero overhead.** The generated assembly is identical to a raw pointer with a manual `delete`.
*   **`shared_ptr`**: Incurs **Atomic Reference Counting**. Every copy/destruction of a `shared_ptr` triggers an `atomic_fetch_add` or `sub`. In high-contention multi-threaded paths, this can cause cache-line bouncing (False Sharing).
*   **`make_shared` vs `new`**: `std::make_shared` allocates the object and the control block in a single contiguous memory block. This is better for cache locality and reduces the number of calls to the heap allocator.

## 8. Low-Latency Perspective
In the "Hot Path" of an HFT engine, we **avoid `shared_ptr`** whenever possible due to atomic overhead. We prefer `unique_ptr` or, even better, objects stored in a **pre-allocated Array/Pool** (RAII at the pool level). However, during "System Init" or "Post-Trade Processing," smart pointers are essential for stability.

## 9. Common Mistakes
1.  **Circular References**: Two `shared_ptr` pointing at each other will never be destroyed. Use `weak_ptr` to break the cycle.
2.  **`shared_ptr` from `this`**: Use `std::enable_shared_from_this` if a class needs to return a `shared_ptr` to itself.
3.  **Manual `new` with Smart Pointers**: Always prefer `std::make_unique` and `std::make_shared`. They are safer (exception safety) and faster.
4.  **Over-sharing**: Passing `shared_ptr` by value when a `const T&` would suffice. This triggers unnecessary atomic increments.

## 10. Exercises
1.  **Custom Deleter**: Create a `unique_ptr` that manages a `void*` returned by `malloc` and uses `free` as a custom deleter.
2.  **Cycle Breaker**: Implement two classes `Parent` and `Child`. Give them `shared_ptr` to each other and observe the leak. Then, fix it using `weak_ptr`.
3.  **Scoped Timer**: Implement an RAII class `ScopedTimer` that records the time in the constructor and prints the elapsed time in the destructor. Use it to measure a short function.

## 11. References
*   *Effective Modern C++* (Items 18-22).
*   *C++ Core Guidelines* (R: Resource Management).
*   ISO C++ Standard (Section [util.smartptr]).
