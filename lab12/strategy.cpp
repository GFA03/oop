/*
 * Lab 12 — The STRATEGY Pattern
 *
 * Problem: An online store calculates the final price of an order
 *   after applying a discount. The kind of discount depends on the
 *   customer: regular, premium, employee, "Black Friday", … and the
 *   store keeps inventing new ones every quarter.
 *
 *   The algorithm used to compute the discount must be SWAPPABLE
 *   at runtime (the customer can become "premium" mid-session), and
 *   adding a new kind of discount should be easy.
 *
 * We will solve the problem twice:
 *   1) WITHOUT the pattern — one big function full of if/else that
 *      hard-codes every discount rule
 *   2) WITH the Strategy pattern — each rule is its own small class;
 *      the order holds a pointer to one of them and asks it to compute
 */

#include <iostream>
#include <memory>
#include <string>

using namespace std;

void section(const string& title) {
    cout << "\n══════════════════════════════════════════\n";
    cout << "  " << title << "\n";
    cout << "══════════════════════════════════════════\n";
}

// ============================================================
//  1. WITHOUT the pattern
//
//  All the discount rules live inside one class, behind an enum and
//  an if/else (or switch). Every new rule grows the same function.
//  Worst: this function probably gets duplicated wherever a discount
//  is computed (cart, checkout, invoice, refund...).
// ============================================================

namespace without_pattern {

enum class CustomerKind { Regular, Premium, Employee, BlackFriday };

class Order {
    double total;
    CustomerKind kind;
public:
    Order(double total, CustomerKind k) : total(total), kind(k) {}

    double finalPrice() const {
        // Every rule baked into one function.
        if (kind == CustomerKind::Regular) {
            return total;                     // no discount
        }
        if (kind == CustomerKind::Premium) {
            return total * 0.90;              // 10% off
        }
        if (kind == CustomerKind::Employee) {
            return total * 0.70;              // 30% off
        }
        if (kind == CustomerKind::BlackFriday) {
            return total < 100 ? total * 0.85
                               : total * 0.75;
        }
        return total;
    }
};

void demo() {
    section("1. WITHOUT Strategy — every rule baked into one function");

    Order a(120, CustomerKind::Regular);
    Order b(120, CustomerKind::Premium);
    Order c(120, CustomerKind::Employee);
    Order d(120, CustomerKind::BlackFriday);

    cout << "    Regular     pays " << a.finalPrice() << "\n";
    cout << "    Premium     pays " << b.finalPrice() << "\n";
    cout << "    Employee    pays " << c.finalPrice() << "\n";
    cout << "    BlackFriday pays " << d.finalPrice() << "\n";

    cout << "  Adding a new discount means editing Order::finalPrice.\n";
    cout << "  Order ends up knowing every business rule in the company.\n";
}

} // namespace without_pattern

// ============================================================
//  2. WITH the Strategy pattern
//
//  Define a DiscountStrategy interface with one method, compute().
//  Each rule is its own tiny class. The Order owns a pointer to a
//  strategy and just delegates. You can change the strategy at run-
//  time with setStrategy(). Adding a new rule is one new class.
// ============================================================

namespace with_pattern {

class DiscountStrategy {
public:
    virtual ~DiscountStrategy() = default;
    virtual double compute(double total) const = 0;
};

class NoDiscount      : public DiscountStrategy { public: double compute(double t) const override { return t; } };
class PremiumDiscount : public DiscountStrategy { public: double compute(double t) const override { return t * 0.90; } };
class EmployeeDiscount: public DiscountStrategy { public: double compute(double t) const override { return t * 0.70; } };
class BlackFriday     : public DiscountStrategy {
public:
    double compute(double t) const override {
        return t < 100 ? t * 0.85 : t * 0.75;
    }
};

class Order {
    double total;
    unique_ptr<DiscountStrategy> strategy;
public:
    Order(double total, unique_ptr<DiscountStrategy> s)
        : total(total), strategy(std::move(s)) {}

    // The whole point: strategy can change at runtime.
    void setStrategy(unique_ptr<DiscountStrategy> s) {
        strategy = std::move(s);
    }

    double finalPrice() const {
        return strategy->compute(total);
    }
};

void demo() {
    section("2. WITH Strategy — one rule per class, swappable at runtime");

    Order o(120, make_unique<NoDiscount>());
    cout << "    Regular     pays " << o.finalPrice() << "\n";

    o.setStrategy(make_unique<PremiumDiscount>());
    cout << "    Premium     pays " << o.finalPrice() << "\n";

    o.setStrategy(make_unique<EmployeeDiscount>());
    cout << "    Employee    pays " << o.finalPrice() << "\n";

    o.setStrategy(make_unique<BlackFriday>());
    cout << "    BlackFriday pays " << o.finalPrice() << "\n";

    cout << "  The SAME order changed strategy four times, no new Order\n";
    cout << "  objects needed. To add a 'Student' discount, write one new\n";
    cout << "  class — Order itself never changes.\n";
}

} // namespace with_pattern

// ============================================================
//  Why the Strategy version is better
//   - Each rule is isolated in its own class. Easy to read, easy to
//     test in isolation, easy to swap.
//   - Order is closed for modification: adding a new rule does not
//     touch it.
//   - The "which rule?" decision is data, not control flow — you can
//     load it from config, from a database, or change it mid-session.
//   - This is the same idea as passing a comparator into std::sort:
//     the algorithm is fixed, but the strategy that compares two
//     elements is provided from the outside.
// ============================================================

int main() {
    without_pattern::demo();
    with_pattern::demo();
    return 0;
}
