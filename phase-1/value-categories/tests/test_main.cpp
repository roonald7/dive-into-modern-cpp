#include <iostream>
#include <cassert>
#include <vector>

/**
 * LatencyBuffer (Exercise 2)
 * Tracks copy and move operations to verify zero-copy logic.
 */
class LatencyBuffer {
public:
    static inline int copy_count = 0;
    static inline int move_count = 0;

    LatencyBuffer() = default;

    // Copy constructor
    LatencyBuffer(const LatencyBuffer&) {
        copy_count++;
    }

    // Move constructor
    LatencyBuffer(LatencyBuffer&&) noexcept {
        move_count++;
    }

    static void reset() {
        copy_count = 0;
        move_count = 0;
    }
};

/**
 * A function that processes the buffer.
 * To achieve zero copies when passed a temporary, it should take by value and move.
 */
void process_buffer(LatencyBuffer buf) {
    static std::vector<LatencyBuffer> storage;
    storage.push_back(std::move(buf));
}

int main() {
    std::cout << "--- Exercise: Zero-Copy Verification ---\n";

    // Goal: Perform operations resulting in 0 copies.
    LatencyBuffer::reset();

    // 1. Pass a prvalue
    process_buffer(LatencyBuffer());

    // 2. Pass an xvalue
    LatencyBuffer b2;
    process_buffer(std::move(b2));

    std::cout << "Moves: " << LatencyBuffer::move_count << "\n";
    std::cout << "Copies: " << LatencyBuffer::copy_count << "\n";

    assert(LatencyBuffer::copy_count == 0);
    std::cout << "Assertion passed: 0 copies detected!\n";

    return 0;
}
