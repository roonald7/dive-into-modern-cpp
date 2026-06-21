#include <iostream>
#include <vector>
#include <span>
#include <ranges>
#include <algorithm>
#include <cstdint>

/**
 * MarketDataProcessor: Demonstrates zero-copy parsing and lazy range processing.
 */

struct Trade {
    uint32_t price;
    uint32_t volume;
};

void process_large_trades(std::span<const Trade> trades) {
    std::cout << "--- Filtering Large Trades (via Ranges) ---\n";
    
    // Using C++20 Ranges to filter and view without copying
    auto large_trades = trades 
                      | std::views::filter([](const Trade& t) { return t.volume >= 100; })
                      | std::views::transform([](const Trade& t) { return t.price; });

    for (auto price : large_trades) {
        std::cout << " [Trade] Found large trade at price: " << price << "\n";
    }
}

int main() {
    std::cout << "--- production_example: View Topologies ---\n\n";

    // 1. Simulated large buffer from exchange
    std::vector<Trade> market_burst = {
        {150, 10}, {151, 500}, {149, 5}, {152, 200}, {150, 10}
    };

    // 2. Pass a span to the processor (zero copy)
    process_large_trades(market_burst);

    return 0;
}
