#include <iostream>
#include <variant>
#include <vector>
#include <cstdint>
#include <string>

// Helper for the "overloaded" pattern (C++20/17)
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

/**
 * Modern State Machine using std::variant
 * No virtual functions, no heap, no pointer chasing.
 */

struct StateConnected { std::string remote_ip; };
struct StateAuthenticating { int attempts; };
struct StateReady { uint64_t session_id; };
struct StateDisconnected { std::string reason; };

using ConnectionState = std::variant<StateConnected, StateAuthenticating, StateReady, StateDisconnected>;

class ConnectionManager {
public:
    void process_state(const ConnectionState& state) {
        // Dispatch using the overloaded lambdas pattern
        std::visit(overloaded {
            [](const StateConnected& s) { 
                std::cout << " [State] Connected to " << s.remote_ip << "\n"; 
            },
            [](const StateAuthenticating& s) { 
                std::cout << " [State] Authenticating (Attempt " << s.attempts << ")\n"; 
            },
            [](const StateReady& s) { 
                std::cout << " [State] READY. Session: " << s.session_id << "\n"; 
            },
            [](const StateDisconnected& s) { 
                std::cout << " [State] DISCONNECTED: " << s.reason << "\n"; 
            }
        }, state);
    }
};

int main() {
    std::cout << "--- production_example: Variant State Machine ---\n\n";

    std::vector<ConnectionState> history;
    history.emplace_back(StateConnected{"192.168.1.10"});
    history.emplace_back(StateAuthenticating{1});
    history.emplace_back(StateReady{998877});
    history.emplace_back(StateDisconnected{"TCP Timeout"});

    ConnectionManager manager;
    for (const auto& s : history) {
        manager.process_state(s);
    }

    return 0;
}
