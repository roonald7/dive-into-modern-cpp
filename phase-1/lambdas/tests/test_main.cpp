#include <iostream>
#include <memory>
#include <cassert>
#include <functional>

/**
 * Exercise 1: Move-Only Capture
 */
void test_move_capture() {
    std::cout << "--- Exercise 1: Move-Only Capture ---\n";
    
    auto ptr = std::make_unique<int>(1337);
    
    // C++14 Generalized Lambda Capture: [local_name = std::move(source)]
    auto lambda = [my_ptr = std::move(ptr)]() {
        return *my_ptr;
    };

    assert(ptr == nullptr); // Check that ptr was moved
    assert(lambda() == 1337);
    
    std::cout << " Value inside lambda: " << lambda() << "\n";
    std::cout << " Move-only capture test passed.\n\n";
}

int main() {
    test_move_capture();
    return 0;
}
