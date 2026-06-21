#include <iostream>
#include <string>
#include <utility>

/**
 * Lvalue reference overload.
 * Used for objects with identity that we shouldn't modify or "steal" from.
 */
void process_value(const std::string& msg) {
    std::cout << "[Lvalue] Processing: " << msg << " (Address: " << &msg << ")\n";
}

/**
 * Rvalue reference overload.
 * Used for temporaries or objects explicitly marked as "expiring" (xvalues).
 */
void process_value(std::string&& msg) {
    std::cout << "[Rvalue] Processing: " << msg << " (Address: " << &msg << ")\n";
    // In a real scenario, we would move 'msg' into a permanent storage here.
    std::string internal_storage = std::move(msg);
    std::cout << "         Moved into storage. Original msg is now: '" << msg << "'\n";
}

int main() {
    std::cout << "--- C++ Value Categories: Basic Example ---\n\n";

    // 1. LVALUE: Has a name, addressable.
    std::string named_var = "Named Variable";
    std::cout << "1. Passing named_var (lvalue):\n";
    process_value(named_var); 
    std::cout << "\n";

    // 2. PRVALUE: Pure Rvalue, literal or temporary result.
    std::cout << "2. Passing literal (prvalue):\n";
    process_value("Temporary Literal");
    std::cout << "\n";

    // 3. XVALUE: Expiring value, cast via std::move.
    std::cout << "3. Passing std::move(named_var) (xvalue):\n";
    process_value(std::move(named_var));
    std::cout << "\n";

    return 0;
}
