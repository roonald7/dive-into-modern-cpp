#include <iostream>
#include <string>

/**
 * HeavyObject: Tracks its own creation/copy/move.
 */
struct HeavyObject {
    std::string name;
    
    HeavyObject(std::string n) : name(std::move(n)) {
        std::cout << " [CTOR] Constructed '" << name << "'\n";
    }
    
    HeavyObject(const HeavyObject& other) : name(other.name) {
        std::cout << " [COPY] Copied '" << name << "'\n";
    }
    
    HeavyObject(HeavyObject&& other) noexcept : name(std::move(other.name)) {
        std::cout << " [MOVE] Moved '" << name << "'\n";
    }
    
    ~HeavyObject() {
        if (!name.empty()) {
            std::cout << " [DTOR] Destroyed '" << name << "'\n";
        }
    }
};

/**
 * Function returning by value (triggers RVO).
 */
HeavyObject create_by_rvo() {
    std::cout << "--- Inside create_by_rvo ---\n";
    return HeavyObject("RVO_Object"); // prvalue returned
}

/**
 * Function returning a named local (triggers NRVO).
 */
HeavyObject create_by_nrvo() {
    std::cout << "--- Inside create_by_nrvo ---\n";
    HeavyObject obj("NRVO_Object");
    return obj; // Named lvalue returned, but eligible for NRVO
}

int main() {
    std::cout << "--- Copy Elision Demo (C++17 Guaranteed) ---\n\n";

    {
        std::cout << "1. Testing RVO (Return Value Optimization):\n";
        HeavyObject my_obj = create_by_rvo();
        std::cout << " Object name: " << my_obj.name << "\n\n";
    }

    {
        std::cout << "2. Testing NRVO (Named Return Value Optimization):\n";
        HeavyObject my_obj = create_by_nrvo();
        std::cout << " Object name: " << my_obj.name << "\n\n";
    }

    return 0;
}
