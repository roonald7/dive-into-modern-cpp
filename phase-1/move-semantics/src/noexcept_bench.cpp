#include <iostream>
#include <vector>
#include <chrono>
#include <string>

/**
 * HeavyPayload: Simulates a large object (like a deep order book snapshot).
 */
struct HeavyPayload {
    std::vector<double> data;
    HeavyPayload() : data(100, 0.0) {} // 800 bytes of data
    
    // Copy Constructor (Expensive: Heap Allocation + Memcpy)
    HeavyPayload(const HeavyPayload& other) : data(other.data) {}
    
    // Move Constructor (Cheap: Pointer Swap)
    HeavyPayload(HeavyPayload&& other) noexcept : data(std::move(other.data)) {}
};

/**
 * MoveOnlySlow: Move constructor is NOT noexcept.
 */
struct MoveOnlySlow {
    HeavyPayload payload;
    MoveOnlySlow() = default;
    
    // Non-noexcept move: Forces std::vector to use Copy Constructor during reallocation
    MoveOnlySlow(MoveOnlySlow&& other) : payload(std::move(other.payload)) {}

    // Copy constructor must be defined for vector to fall back to it
    MoveOnlySlow(const MoveOnlySlow& other) : payload(other.payload) {}
};

/**
 * MoveOnlyFast: Move constructor IS noexcept.
 */
struct MoveOnlyFast {
    HeavyPayload payload;
    MoveOnlyFast() = default;

    // noexcept move: Allows std::vector to use Move Constructor during reallocation
    MoveOnlyFast(MoveOnlyFast&& other) noexcept : payload(std::move(other.payload)) {}

    MoveOnlyFast(const MoveOnlyFast& other) : payload(other.payload) {}
};

template<typename T>
void run_reallocation_test(const std::string& label, int iterations) {
    std::vector<T> vec;
    auto start = std::chrono::high_resolution_clock::now();
    
    for(int i=0; i<iterations; ++i) {
        vec.emplace_back(); // Force reallocations
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << label << " took " << elapsed.count() << " ms\n";
}

int main() {
    const int N = 10000; // Lower iterations because Copying is now very expensive
    std::cout << "--- Performance: noexcept Move vs Copy (Heavy Payload) --- \n";
    std::cout << "Iterations: " << N << " | Payload size: ~800 bytes\n\n";
    
    // Warm up
    { std::vector<MoveOnlyFast> warm; for(int i=0; i<100; ++i) warm.emplace_back(); }

    run_reallocation_test<MoveOnlySlow>("Non-noexcept Move (Vector uses COPIES)", N);
    run_reallocation_test<MoveOnlyFast>("noexcept Move (Vector uses MOVES)    ", N);

    return 0;
}
