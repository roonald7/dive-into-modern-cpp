#include <iostream>
#include <cassert>
#include <utility>
#include <type_traits>

/**
 * Exercise 2: Detecting Value Category
 * Using type traits to verify what a universal reference bound to.
 */
template <typename T>
std::string identify_category(T&& arg) {
    if constexpr (std::is_lvalue_reference_v<T&&>) {
        return "LVALUE";
    } else {
        return "RVALUE";
    }
}

/**
 * Exercise 1: Variadic Logger
 * Perfectly forwards any number of arguments to a function.
 */
template <typename Func, typename... Args>
auto log_and_call(Func f, Args&&... args) {
    std::cout << " [Log] Forwarding " << sizeof...(args) << " arguments...\n";
    return f(std::forward<Args>(args)...);
}

// Test target for log_and_call
int sum(int a, int b) { return a + b; }
std::string concat(std::string a, std::string b) { return a + b; }

int main() {
    std::cout << "--- Phase 1 Topic 3: Forwarding Tests ---\n";

    // 1. Test Category Detection
    int x = 10;
    assert(identify_category(x) == "LVALUE");
    assert(identify_category(20) == "RVALUE");
    assert(identify_category(std::move(x)) == "RVALUE");
    std::cout << " Categorization tests passed.\n";

    // 2. Test Variadic Logger (Integers)
    int result = log_and_call(sum, 5, 10);
    assert(result == 15);

    // 3. Test Variadic Logger (Strings with Moves)
    std::string s1 = "Hello ";
    std::string s2 = "World";
    std::string combined = log_and_call(concat, std::move(s1), std::move(s2));
    assert(combined == "Hello World");
    assert(s1.empty() || s1 != "Hello "); // Depending on implementation, but likely moved
    
    std::cout << " Variadic forwarding tests passed.\n";
    std::cout << "All exercises complete!\n";

    return 0;
}
