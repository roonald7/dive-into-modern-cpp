#include <iostream>
#include <coroutine>
#include <vector>
#include <cassert>

/**
 * Exercise 1: Fibonacci Generator
 */
struct FibGenerator {
    struct promise_type {
        int current_value;
        FibGenerator get_return_object() {
            return FibGenerator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(int value) {
            current_value = value;
            return {};
        }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    std::coroutine_handle<promise_type> h;
    FibGenerator(std::coroutine_handle<promise_type> handle) : h(handle) {}
    ~FibGenerator() { if (h) h.destroy(); }

    int next() {
        h.resume();
        return h.promise().current_value;
    }
};

FibGenerator fibonacci() {
    int a = 0, b = 1;
    while (true) {
        co_yield a;
        int next = a + b;
        a = b;
        b = next;
    }
}

int main() {
    std::cout << "--- Coroutines: Fibonacci Test ---\n";
    auto gen = fibonacci();
    
    std::vector<int> expected = {0, 1, 1, 2, 3, 5, 8};
    for (int val : expected) {
        int actual = gen.next();
        std::cout << " Fib: " << actual << "\n";
        assert(actual == val);
    }

    std::cout << " Fibonacci test passed!\n";
    return 0;
}
