#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <cassert>
#include <vector>

/**
 * Exercise 1: Custom Deleter for malloc
 */
void test_custom_deleter() {
    std::cout << "--- Exercise 1: Custom Deleter ---\n";
    {
        std::unique_ptr<int, void(*)(void*)> p(
            static_cast<int*>(std::malloc(sizeof(int))), 
            std::free
        );
        *p = 42;
        std::cout << " Value via malloc: " << *p << "\n";
    } 
    std::cout << " Custom deleter test passed.\n\n";
}

/**
 * Exercise 3: Scoped Timer (RAII)
 */
class ScopedTimer {
public:
    ScopedTimer(std::string name) 
        : m_name(std::move(name)), m_start(std::chrono::high_resolution_clock::now()) {}

    ~ScopedTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - m_start;
        std::cout << " [Timer] " << m_name << " took " << elapsed.count() << " ms\n";
    }

private:
    std::string m_name;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
};

void test_scoped_timer() {
    std::cout << "--- Exercise 3: Scoped Timer ---\n";
    {
        ScopedTimer timer("Work_Simulation");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

/**
 * Exercise 2: Cycle Breaker (Weak Pointers)
 */
struct Child;

struct Parent {
    std::string name;
    std::shared_ptr<Child> child;
    Parent(std::string n) : name(std::move(n)) {}
    ~Parent() { std::cout << "  [RAII] Parent '" << name << "' destroyed\n"; }
};

struct Child {
    std::string name;
    // weak_ptr is the "observer". It doesn't participate in ownership.
    std::weak_ptr<Parent> parent; 
    Child(std::string n) : name(std::move(n)) {}
    ~Child() { std::cout << "  [RAII] Child '" << name << "' destroyed\n"; }
};

void test_cycle_leak() {
    std::cout << "\n--- Exercise 2: Cycle Breaker (Weak Pointers) ---\n";
    
    {
        std::cout << " Creating Parent/Child relationship...\n";
        auto father = std::make_shared<Parent>("John");
        auto son = std::make_shared<Child>("Junior");

        // Link them
        father->child = son;
        son->parent = father; // Bound to a weak_ptr

        std::cout << " Parent Use Count: " << father.use_count() << "\n"; // Still 1!
        std::cout << " Child Use Count: " << son.use_count() << "\n";   // 1
        
        // Note: son->parent is weak, so it doesn't keep 'father' alive.
        // If we want to use the parent from the child, we must "lock" it:
        if (auto p_ptr = son->parent.lock()) {
            std::cout << " Child's view: My parent is " << p_ptr->name << "\n";
        }
    } 

    std::cout << " Scope ended. If weak_ptr worked, destructors should have printed above.\n";
}

int main() {
    test_custom_deleter();
    test_scoped_timer();
    test_cycle_leak();
    return 0;
}
