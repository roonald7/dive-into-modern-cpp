#include <iostream>
#include <vector>
#include <cstdint>
#include <chrono>

/**
 * MarketDataPacket: Represents a burst of price levels.
 * This struct owns a vector, so copying it is O(N) where N is price_levels.size().
 * In low-latency systems, we want to move this through our pipeline.
 */
struct MarketDataPacket {
    uint64_t sequence_number;
    std::vector<double> prices;

    // Normal constructor
    MarketDataPacket(uint64_t seq, size_t count) 
        : sequence_number(seq), prices(count, 0.0) {
        // Simulating some data
        for(size_t i=0; i<count; ++i) prices[i] = static_cast<double>(seq + i);
    }

    // Default Copy Constructor (Expensive)
    MarketDataPacket(const MarketDataPacket& other) 
        : sequence_number(other.sequence_number), prices(other.prices) {
        std::cout << " [COPY] Packet #" << sequence_number << " copied (prices size: " << prices.size() << ")\n";
    }

    // Move Constructor (Cheap - Pointer Swaps)
    MarketDataPacket(MarketDataPacket&& other) noexcept
        : sequence_number(other.sequence_number), prices(std::move(other.prices)) {
        std::cout << " [MOVE] Packet #" << sequence_number << " moved\n";
    }

    // Disable assignment for simplicity in this demo
    MarketDataPacket& operator=(const MarketDataPacket&) = delete;
    MarketDataPacket& operator=(MarketDataPacket&&) = delete;
};

/**
 * MatchingEngine: Final destination for the packet.
 */
class MatchingEngine {
public:
    void on_market_data(MarketDataPacket packet) {
        m_archive.push_back(std::move(packet));
    }

    size_t processed_count() const { return m_archive.size(); }
    size_t capacity() const { return m_archive.capacity(); }
    size_t size() const { return m_archive.size(); }

private:
    std::vector<MarketDataPacket> m_archive;
};

int main() {
    std::cout << "--- production_example: Low-Latency Pipeline ---\n\n";
    MatchingEngine engine;

    // SCENARIO 1: Temporary (PRVALUE)
    std::cout << "Scenario 1: Passing a prvalue (temporary)\n";
    std::cout << " Pre-Capacity: " << engine.capacity() << " Size: " << engine.size() << "\n";
    engine.on_market_data(MarketDataPacket(1, 100));
    std::cout << " Post-Capacity: " << engine.capacity() << " Size: " << engine.size() << "\n";

    // SCENARIO 2: Persistent Object (LVALUE)
    std::cout << "\nScenario 2: Passing an lvalue (named variable)\n";
    MarketDataPacket p2(2, 500);
    std::cout << " Pre-Capacity: " << engine.capacity() << " Size: " << engine.size() << "\n";
    engine.on_market_data(p2); // Deep copy occurs
    std::cout << " Post-Capacity: " << engine.capacity() << " Size: " << engine.size() << "\n";

    // SCENARIO 3: Expiring Object (XVALUE)
    std::cout << "\nScenario 3: Passing an xvalue (std::move)\n";
    MarketDataPacket p3(3, 1000);
    std::cout << " Pre-Capacity: " << engine.capacity() << " Size: " << engine.size() << "\n";
    engine.on_market_data(std::move(p3)); // Move occurs
    std::cout << " Post-Capacity: " << engine.capacity() << " Size: " << engine.size() << "\n";

    std::cout << "\nPipeline completed. Total packets in engine: " << engine.processed_count() << "\n";

    return 0;
}
