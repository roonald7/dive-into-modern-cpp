#include <iostream>
#include <utility>
#include <cstdint>

/**
 * BufferManager: Manages a raw heap allocation.
 * Demonstrates basic move-only semantics.
 */
class BufferManager {
public:
    BufferManager(size_t size) : m_size(size), m_data(new uint8_t[size]) {
        std::cout << " [CTOR] Allocated " << m_size << " bytes at " << (void*)m_data << "\n";
    }

    ~BufferManager() {
        if (m_data) {
            std::cout << " [DTOR] Freeing " << m_size << " bytes from " << (void*)m_data << "\n";
            delete[] m_data;
        } else {
            std::cout << " [DTOR] Nothing to free (already moved)\n";
        }
    }

    // Move-only: Prevent duplication
    BufferManager(const BufferManager&) = delete;
    BufferManager& operator=(const BufferManager&) = delete;

    // Move Constructor
    BufferManager(BufferManager&& other) noexcept 
        : m_size(other.m_size), m_data(other.m_data) {
        other.m_data = nullptr; // Crucial: clear the source to avoid double-free
        other.m_size = 0;
        std::cout << " [MOVE CTOR] Transferred " << m_size << " bytes\n";
    }

    // Move Assignment
    BufferManager& operator=(BufferManager&& other) noexcept {
        std::cout << " [MOVE ASSIGN] Transferred " << other.m_size << " bytes\n";
        if (this != &other) {
            delete[] m_data; // Clean up current resources
            
            m_data = other.m_data;
            m_size = other.m_size;
            
            other.m_data = nullptr;
            other.m_size = 0;
        }
        return *this;
    }

private:
    size_t m_size;
    uint8_t* m_data;
};

int main() {
    std::cout << "--- Move Semantics: Basic Example ---\n\n";

    std::cout << "Creating b1...\n";
    BufferManager b1(1024);

    std::cout << "\nMoving b1 to b2...\n";
    BufferManager b2 = std::move(b1); // Triggers Move Constructor

    std::cout << "\nCreating b3 and moving b2 to it via assignment...\n";
    BufferManager b3(512);
    b3 = std::move(b2); // Triggers Move Assignment

    std::cout << "\nExiting scope...\n";
    return 0;
}
