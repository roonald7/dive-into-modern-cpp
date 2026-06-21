#include <iostream>
#include <string>
#include <memory>
#include <utility>

/**
 * TraceableString: A simple wrapper to track copy vs move on the internal buffer.
 */
struct TraceableString {
    std::string val;

    TraceableString(const char* s) : val(s) {}
    TraceableString(std::string s) : val(std::move(s)) {}

    // Copy Constructor
    TraceableString(const TraceableString& other) : val(other.val) {
        std::cout << "     >>> [STRING COPY] Duplicating buffer for '" << val << "'\n";
    }

    // Move Constructor
    TraceableString(TraceableString&& other) noexcept : val(std::move(other.val)) {
        std::cout << "     >>> [STRING MOVE] Stealing buffer for '" << val << "'\n";
    }
};

struct Order {
    uint32_t id;
    TraceableString symbol;
    double price;

    // Constructor taking by-value to allow moves
    Order(uint32_t i, TraceableString sym, double p)
        : id(i), symbol(std::move(sym)), price(p) {
        std::cout << " [Order] Constructed ID=" << id << " Sym=" << symbol.val << "\n";
    }
};

class EventLogger {
public:
    template <typename T, typename... Args>
    static T create_and_log(Args&&... args) {
        std::cout << " [Logger] Intercepting creation for audit logs...\n";
        
        // Perfect Forwarding happens here:
        // If an rvalue symbol is passed, it is forwarded as rvalue and 'MOVED' into Order.
        T obj(std::forward<Args>(args)...);
        
        std::cout << " [Logger] Audit log complete.\n";
        return obj;
    }
};

int main() {
    std::cout << "--- production_example: Universal Event Factory (Deep Trace) ---\n\n";

    // Scenario A: Passing a temporary (prvalue)
    // The temporary TraceableString is forwarded as an rvalue.
    std::cout << "Scenario A: String literal -> prvalue\n";
    Order o1 = EventLogger::create_and_log<Order>(101, TraceableString("BTCUSD"), 65000.0);
    std::cout << "\n";

    // Scenario B: Passing a named variable (lvalue)
    // We expect a COPY here because 'my_sym' must remain valid for later use.
    std::cout << "Scenario B: Named lvalue (must be copied)\n";
    TraceableString my_sym("ETHUSD");
    Order o2 = EventLogger::create_and_log<Order>(102, my_sym, 3500.0);
    std::cout << " (my_sym is still: " << my_sym.val << ")\n\n";

    // Scenario C: Explicit move (xvalue)
    // We expect a MOVE here because we told the factory we are done with 'expiring_sym'.
    std::cout << "Scenario C: Explicit move (std::move)\n";
    TraceableString expiring_sym("SOLUSD");
    Order o3 = EventLogger::create_and_log<Order>(103, std::move(expiring_sym), 140.0);
    std::cout << " (expiring_sym is now empty/invalid: '" << expiring_sym.val << "')\n";

    return 0;
}
