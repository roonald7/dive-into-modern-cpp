#include <iostream>
#include <coroutine>

/**
 * IterGenerator: A basic C++20 Generator for integers.
 */
struct IterGenerator {
    struct promise_type {
        int current_value;
        IterGenerator get_return_object() {
            return IterGenerator{std::coroutine_handle<promise_type>::from_promise(*this)};
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

    IterGenerator(std::coroutine_handle<promise_type> handle) : h(handle) {}
    ~IterGenerator() { if (h) h.destroy(); }

    bool move_next() {
        h.resume();
        return !h.done();
    }

    int current() { return h.promise().current_value; }
};

IterGenerator count_to_five() {
    for (int i = 1; i <= 5; ++i) {
        co_yield i;
    }
}

int main() {
    std::cout << "--- Coroutines: Basic Generator ---\n\n";

    auto gen = count_to_five();
    while (gen.move_next()) {
        std::cout << " Yielded: " << gen.current() << "\n";
    }

    return 0;
}
