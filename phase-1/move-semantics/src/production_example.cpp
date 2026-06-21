#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <thread>

/**
 * TradeSignal: A move-only object representing a financial signal.
 * We use std::unique_ptr to ensure that exactly one part of the system
 * owns the signal at any time.
 */
struct TradeSignal {
    uint64_t timestamp;
    std::string symbol;
    double price;
    char side; // 'B' or 'S'

    TradeSignal(uint64_t ts, std::string sym, double p, char s)
        : timestamp(ts), symbol(std::move(sym)), price(p), side(s) {}

    void print() const {
        std::cout << " [Signal] " << symbol << " at " << price << " (" << side << ")\n";
    }
};

/**
 * RiskEngine: Validates signals.
 */
class RiskEngine {
public:
    bool validate(const TradeSignal& signal) {
        // In a real system, checking limits, frequency, etc.
        return signal.price > 0 && signal.price < 1000000;
    }
};

/**
 * ExecutionGateway: Sends signals to the exchange.
 */
class ExecutionGateway {
public:
    void send_to_exchange(std::unique_ptr<TradeSignal> signal) {
        std::cout << ">>> Sending signal to exchange: ";
        signal->print();
        // Here, ownership ends as we might store it in a sent_log or just let it die.
        m_sent_log.push_back(std::move(signal));
    }

private:
    std::vector<std::unique_ptr<TradeSignal>> m_sent_log;
};

int main() {
    std::cout << "--- Move-Only Data Pipeline ---\n\n";

    RiskEngine risk;
    ExecutionGateway gateway;

    // 1. Create a signal
    auto signal = std::make_unique<TradeSignal>(1624281600, "AAPL", 150.50, 'B');
    std::cout << "Signal created. unique_ptr address: " << signal.get() << "\n";

    // 2. Pass to Risk (by const ref - no ownership transfer)
    if (risk.validate(*signal)) {
        std::cout << "Signal passed risk checks.\n";

        // 3. Pass to Gateway (by value/move - ownership transfers)
        std::cout << "Transferring ownership to Gateway...\n";
        gateway.send_to_exchange(std::move(signal));
    }

    // 4. Verify ownership
    if (!signal) {
        std::cout << "\nMain: Signal ownership successfully transferred. 'signal' is now null.\n";
    }

    return 0;
}
