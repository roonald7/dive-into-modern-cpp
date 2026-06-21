#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <string>

/**
 * PriceUpdate: A more complex struct.
 * Sorting this involves multiple comparisons.
 */
struct PriceUpdate {
    char symbol[8];
    double price;
    uint64_t timestamp;
    int volume;

    // A slightly complex comparison logic
    static bool compare(const PriceUpdate& a, const PriceUpdate& b) {
        if (a.price != b.price) return a.price < b.price;
        return a.timestamp < b.timestamp;
    }
};

// Global function pointer target
bool price_compare_func(const PriceUpdate& a, const PriceUpdate& b) {
    return PriceUpdate::compare(a, b);
}

void run_bench(const std::string& label, size_t size) {
    std::vector<PriceUpdate> data(size);
    
    // Initialize with randomish data
    std::mt19937 g(42); 
    for(size_t i=0; i<size; ++i) {
        data[i].price = static_cast<double>(g() % 1000);
        data[i].timestamp = g();
    }

    auto start = std::chrono::high_resolution_clock::now();
    
    if (label.find("Lambda") != std::string::npos) {
        std::sort(data.begin(), data.end(), [](const PriceUpdate& a, const PriceUpdate& b) {
            if (a.price != b.price) return a.price < b.price;
            return a.timestamp < b.timestamp;
        });
    } else {
        std::sort(data.begin(), data.end(), price_compare_func);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << " Sort with " << label << ": " << elapsed.count() << " ms (Size: " << size << ")\n";
}

int main() {
    const size_t N = 1000000;
    std::cout << "--- Benchmark: Lambda vs Function Pointer (Complex Struct) ---\n";
    std::cout << " Sorting " << N << " PriceUpdate objects...\n\n";

    // 1. Warmup
    run_bench("Warmup (Lambda)", 1000);
    
    // 2. Real Tests
    run_bench("Function Pointer", N);
    run_bench("Lambda          ", N);

    return 0;
}
