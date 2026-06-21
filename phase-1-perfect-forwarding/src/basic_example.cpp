#include <iostream>
#include <utility>
#include <type_traits>

/**
 * The target functions that distinguish between lvalues and rvalues.
 */
void target(int& n) {
    std::cout << " [TARGET] Received LVALUE reference\n";
}

void target(int&& n) {
    std::cout << " [TARGET] Received RVALUE reference\n";
}

/**
 * WRONG behavior: Using std::move inside a wrapper.
 * This will cast EVERYTHING to an rvalue, even if it shouldn't be.
 */
template <typename T>
void bad_wrapper(T&& arg) {
    std::cout << " [WRAPPER] Inside bad_wrapper (using std::move)\n";
    target(std::move(arg)); // DANGEROUS!
}

/**
 * CORRECT behavior: Using std::forward.
 * This preserves the original value category.
 */
template <typename T>
void good_wrapper(T&& arg) {
    std::cout << " [WRAPPER] Inside good_wrapper (using std::forward)\n";
    target(std::forward<T>(arg)); 
}

int main() {
    std::cout << "--- Perfect Forwarding: Basic Example ---\n\n";

    int x = 42;

    std::cout << "1. Passing LVALUE (x) to good_wrapper:\n";
    good_wrapper(x); // Should call target(int&)

    std::cout << "\n2. Passing RVALUE (100) to good_wrapper:\n";
    good_wrapper(100); // Should call target(int&&)

    std::cout << "\n3. Passing LVALUE (x) to bad_wrapper:\n";
    bad_wrapper(x); // Oops! Calls target(int&&) - This would "steal" from x!

    return 0;
}
