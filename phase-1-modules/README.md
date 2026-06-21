# Phase 1, Topic 9: Compilation Units (Modules Layout)

## 1. Title
C++20 Modules: Compilation Speed, Encapsulation, and Binary Interoperability

## 2. Motivation
The C++ compilation model hasn't fundamentally changed since the 1970s. `#include` is a preprocessor hack that results in billions of lines of redundant parsing (e.g., `#include <iostream>` in every file). Modules (C++20) introduce a proper component system that isolates code, prevents macro leakage, and significantly speeds up "Clean Builds."

## 3. Problem Statement
In large-scale low-latency projects, compilation times can reach hours. 
*   **Old way**: Headers + `.cpp` files. Changing a utility header triggers a massive recompilation of everything that includes it. Macros in one header can break code in another (ODR violations).
*   **New way**: `export module MyEngine`. The interface is separated from the implementation. Only changes to the `export` section trigger recompilations of dependents. Internal details are completely hidden.

## 4. Core Concepts
*   **Module Interface Unit**: File containing `export module name;`. Defines what the outside world sees.
*   **Module Implementation Unit**: File containing `module name;`. Contains the private logic.
*   **`export` keyword**: Marks classes, functions, or templates as visible to importers.
*   **Global Module Fragment**: Used for legacy `#include` inside a module.
*   **Private Module Fragment**: Allows putting the whole module in one file while still hiding implementation details.

## 5. Basic Example
```cpp
// math_module.ixx (or .cppm)
export module Math;

export int add(int a, int b) {
    return a + b;
}

// main.cpp
import Math;
#include <iostream>

int main() {
    std::cout << add(5, 10) << "\n";
}
```

## 6. Production Example: Core Network Module
A production module often separates the API from the heavy dependencies.

```cpp
// network.ixx
export module Network;
import <string>; // Standard library as a module

export namespace Network {
    struct Config {
        std::string ip;
        int port;
    };

    class Socket {
    public:
        export void connect(const Config& conf);
    private:
        int m_fd = -1;
    };
}
```

## 7. Performance Considerations
*   **Compilation Time**: Clean builds are faster because the compiler doesn't re-parse the same headers. Incremental builds are faster because of better dependency tracking.
*   **Instruction Cache**: Modules don't change the generated assembly, but better encapsulation often leads to better inlining opportunities as the compiler has a clearer view of the module boundary.

## 8. Low-Latency Perspective
In HFT, we use modules to isolate the **Math Library** and **Message Protocols**. This prevents the "Namespace Pollution" that often happens with large legacy codebases. It also allows us to strictly control what is exposed to the matching loop, ensuring that only "Inlined, Hot-Path" functions are exported.

## 9. Common Mistakes
1.  **Macro Leakage**: You cannot export macros. If your project relies on preprocessor flags for configuration, you must still use some traditional headers.
2.  **Circular Imports**: Module A cannot import Module B if Module B imports Module A. This forces better architectural design.
3.  **Tooling Support**: Not all build systems (like older CMake) or IDEs support C++20 modules perfectly yet.

## 10. Exercises
1.  **Module Migration**: Take a small class with a header/cpp pair and convert it into a single-file module using a `private module fragment`.
2.  **Partitioning**: Split a large module into "Sub-Modules" (Partitions) using `export module MyMod:Part1;`.
3.  **Encapsulation Test**: Try to access a non-exported function from a module and verify the compiler error.

## 11. References
*   CppReference: Modules.
*   "Standard C++20 Modules" - Bill Hoffman (CMake).
*   ISO C++20 Standard.
