#include <iostream>
#include <vector>
#include <span>
#include <array>

/**
 * Passing data by span to avoid copies and raw pointers.
 */
void analyze_buffer(std::span<const int> data) {
    if (data.empty()) return;
    
    std::cout << " [Span] Size: " << data.size() << " | First: " << data.front() << "\n";
    
    // Using subspan
    if (data.size() > 2) {
        auto slice = data.subspan(1, 2);
        std::cout << " [Span] Middle slice size: " << slice.size() << "\n";
    }
}

int main() {
    std::cout << "--- Views: Basic Example (std::span) ---\n\n";

    // 1. From std::vector
    std::vector<int> v = {10, 20, 30, 40, 50};
    analyze_buffer(v);

    // 2. From std::array
    std::array<int, 3> a = {100, 200, 300};
    analyze_buffer(a);

    // 3. From C-array
    int raw[] = {1, 1, 2, 3, 5, 8};
    analyze_buffer(raw);

    // 4. From Pointer + Size
    analyze_buffer({v.data(), 2}); // View only first 2 elements

    return 0;
}
