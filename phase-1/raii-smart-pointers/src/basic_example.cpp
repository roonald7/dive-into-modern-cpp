#include <iostream>
#include <memory>
#include <vector>

/**
 * Simple RAII class for a raw buffer.
 */
class RawBuffer {
public:
    RawBuffer(size_t size) : m_size(size), m_data(new uint8_t[size]) {
        std::cout << " [Buffer] Allocated " << m_size << " bytes\n";
    }
    ~RawBuffer() {
        if (m_data) {
            std::cout << " [Buffer] Freeing " << m_size << " bytes\n";
            delete[] m_data;
        }
    }
    // Prevent copying to avoid double-free
    RawBuffer(const RawBuffer&) = delete;
    RawBuffer& operator=(const RawBuffer&) = delete;

private:
    size_t m_size;
    uint8_t* m_data;
};

void test_unique_ptr() {
    std::cout << "--- Testing std::unique_ptr ---\n";
    // make_unique is the modern standard (C++14+)
    auto p1 = std::make_unique<RawBuffer>(1024);
    
    // auto p2 = p1; // ERROR: unique_ptr is move-only
    auto p2 = std::move(p1); // Transfer ownership
    
    if (!p1) std::cout << " p1 is now null\n";
}

int main() {
    test_unique_ptr();
    std::cout << "\nExiting main, all resources should be freed.\n";
    return 0;
}
