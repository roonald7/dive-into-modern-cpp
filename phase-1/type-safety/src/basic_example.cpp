#include <iostream>
#include <optional>
#include <variant>
#include <string>

/**
 * Basic usage of std::optional.
 */
std::optional<int> string_to_int(const std::string& s) {
    try {
        return std::stoi(s);
    } catch (...) {
        return std::nullopt; // Explicitly return "nothing"
    }
}

int main() {
    std::cout << "--- Type Safety: Basic Example ---\n\n";

    // 1. std::optional
    auto val = string_to_int("123");
    if (val) {
        std::cout << "Parsed value: " << *val << "\n";
    }

    auto empty = string_to_int("abc");
    std::cout << "Parsed 'abc': " << empty.value_or(-1) << " (fallback used)\n";

    // 2. std::variant
    std::variant<int, double, std::string> v = 42;
    std::cout << "Variant holds int: " << std::get<int>(v) << "\n";

    v = 3.14159;
    std::cout << "Variant holds double: " << std::get<double>(v) << "\n";

    v = "Modern C++";
    std::cout << "Variant holds string: " << std::get<std::string>(v) << "\n";

    return 0;
}
