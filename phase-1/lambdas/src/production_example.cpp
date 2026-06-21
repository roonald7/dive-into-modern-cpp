#include <iostream>
#include <vector>
#include <functional>
#include <chrono>

/**
 * HotPathProcessor: Simulates an engine that needs to call a user-defined
 * reaction millions of times per second.
 */
class HotPathProcessor {
public:
    // Pattern A: std::function wrapper (Easier API, but heavy)
    void register_callback_std(std::function<void(int)> cb) {
        m_std_cb = std::move(cb);
    }

    // Pattern B: Template-based (Zero Overhead, Static Dispatch)
    template <typename Func>
    void run_with_template(int data, Func&& f) {
        // Here, the compiler sees the EXACT lambda and can inline it!
        f(data);
    }

    void run_std(int data) {
        if (m_std_cb) m_std_cb(data);
    }

private:
    std::function<void(int)> m_std_cb;
};

int main() {
    std::cout << "--- production_example: Lambdas vs std::function ---\n\n";

    HotPathProcessor engine;
    int state = 0;

    // A. Using std::function
    // Triggers virtual call and potential heap allocation
    engine.register_callback_std([&state](int val) {
        state += val;
    });
    engine.run_std(10);

    // B. Using Template (Standard in HFT Feed Handlers)
    // The lambda type is unique; compiler can bypass function pointers.
    engine.run_with_template(20, [&state](int val) {
        state += val;
    });

    std::cout << "Final State: " << state << "\n";
    std::cout << "Note: In hot loops, always prefer Templates/Lambdas over std::function.\n";

    return 0;
}
