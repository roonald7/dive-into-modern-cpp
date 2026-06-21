#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::cout << "--- Lambdas: Basic Example ---\n\n";

    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int divisor = 3;

    // 1. Simple Predicate with Capture
    // [divisor] captures by value.
    auto it = std::find_if(numbers.begin(), numbers.end(), [divisor](int n) {
        return (n % divisor) == 0;
    });

    if (it != numbers.end()) {
        std::cout << "First number divisible by " << divisor << " is: " << *it << "\n";
    }

    // 2. Stateful Lambda (mutable)
    // By default, capture-by-value is const. 'mutable' allows internal state modification.
    auto counter = [count = 0]() mutable {
        return ++count;
    };

    std::cout << "Counter: " << counter() << "\n";
    std::cout << "Counter: " << counter() << "\n";
    std::cout << "Counter: " << counter() << "\n";

    return 0;
}
