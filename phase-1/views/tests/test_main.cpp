#include <iostream>
#include <span>
#include <vector>
#include <cassert>
#include <algorithm>

/**
 * Exercise 1: Middle Reversal
 */
void reverse_middle(std::span<int> data) {
    if (data.size() < 4) return;
    
    size_t quarter = data.size() / 4;
    auto middle = data.subspan(quarter, data.size() / 2);
    std::reverse(middle.begin(), middle.end());
}

/**
 * Exercise 3: Fixed-size Span
 * Function that only accepts exactly 4 elements at compile-time (if possible).
 */
void process_quad(std::span<int, 4> data) {
    std::cout << " Processing exactly 4 elements: ";
    for(int x : data) std::cout << x << " ";
    std::cout << "\n";
}

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8};
    
    std::cout << "--- Exercise 1: Reverse Middle ---\n";
    reverse_middle(v);
    // Middle 4 elements of 8 are {3, 4, 5, 6}. Reversed: {6, 5, 4, 3}
    assert(v[2] == 6 && v[5] == 3);
    std::cout << " Middle reversal test passed.\n";

    std::cout << "\n--- Exercise 3: Static Span ---\n";
    int arr[] = {10, 20, 30, 40};
    process_quad(arr); // OK
    
    // std::span<int, 4> s_fixed = v; // ERROR: size mismatch (compile-time safety)
    
    return 0;
}
