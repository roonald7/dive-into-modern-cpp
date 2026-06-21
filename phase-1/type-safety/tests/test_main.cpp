#include <iostream>
#include <optional>
#include <variant>
#include <vector>
#include <cassert>
#include <cmath>

/**
 * Exercise 2: Static Polymorphism (Shapes)
 */
struct Circle { double radius; };
struct Square { double side; };

using Shape = std::variant<Circle, Square>;

struct AreaCalculator {
    double operator()(const Circle& c) const { return M_PI * c.radius * c.radius; }
    double operator()(const Square& s) const { return s.side * s.side; }
};

void test_shapes() {
    std::cout << "--- Exercise 2: Shapes Area ---\n";
    std::vector<Shape> shapes;
    shapes.emplace_back(Circle{10.0});
    shapes.emplace_back(Square{10.0});

    double total_area = 0;
    AreaCalculator calc;
    for (const auto& s : shapes) {
        total_area += std::visit(calc, s);
    }
    
    std::cout << " Total Area: " << total_area << "\n";
    assert(total_area > 414.0 && total_area < 415.0);
}

int main() {
    test_shapes();
    std::cout << "All tests passed!\n";
    return 0;
}
