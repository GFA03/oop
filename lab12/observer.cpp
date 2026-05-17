/*
 * Lab 12 — The OBSERVER Pattern
 *
 * Problem: A stock-ticker class holds the current price of a stock.
 *   When the price changes, several different "displays" must update:
 *     - a console display
 *     - a chart
 *     - an alert system that emails the user if the price drops
 *
 *   New display types will be added later, by other teams. The stock
 *   class must NOT need to be edited every time a new display appears.
 *
 * We will solve the problem twice:
 *   1) WITHOUT the pattern — the stock hard-codes a call to each
 *      display. Tight coupling everywhere.
 *   2) WITH the Observer pattern — the stock keeps a list of
 *      generic "observers" and notifies them all. It does not know
 *      or care what they are.
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

void section(const string& title) {
    cout << "\n══════════════════════════════════════════\n";
    cout << "  " << title << "\n";
    cout << "══════════════════════════════════════════\n";
}

// ============================================================
//  1. WITHOUT the pattern
//
//  Stock has direct pointers to every display class. Every time
//  someone adds a new display, Stock itself has to be modified —
//  a new member, a new line in the setter. Tight coupling, brittle.
// ============================================================

namespace without_pattern {

class ConsoleDisplay {
public:
    void show(double price) { cout << "    [console] price = " << price << "\n"; }
};

class ChartDisplay {
public:
    void plot(double price) { cout << "    [chart  ] plot point " << price << "\n"; }
};

class AlertSystem {
public:
    void check(double price) {
        if (price < 10.0)
            cout << "    [alert  ] price dropped below 10!\n";
    }
};

class Stock {
    double price = 0.0;

    // Stock now knows about EVERY display type. Coupling explodes.
    ConsoleDisplay* console = nullptr;
    ChartDisplay*   chart   = nullptr;
    AlertSystem*    alerts  = nullptr;

public:
    void attach(ConsoleDisplay* c) { console = c; }
    void attach(ChartDisplay*   c) { chart   = c; }
    void attach(AlertSystem*    a) { alerts  = a; }

    void setPrice(double p) {
        price = p;
        // Every dependent is called by name.
        if (console) console->show(p);
        if (chart)   chart->plot(p);
        if (alerts)  alerts->check(p);
    }
};

void demo() {
    section("1. WITHOUT Observer — Stock knows every display by name");

    ConsoleDisplay c;
    ChartDisplay   ch;
    AlertSystem    a;

    Stock s;
    s.attach(&c);
    s.attach(&ch);
    s.attach(&a);

    s.setPrice(42.0);
    s.setPrice(8.5);

    cout << "  To add a fourth display, Stock itself would have to change.\n";
}

} // namespace without_pattern

// ============================================================
//  2. WITH the Observer pattern
//
//  Define an Observer interface with one method, onPriceChanged().
//  Stock keeps a list of Observer*, without knowing what they really
//  are. notify() walks the list. Adding a new display only requires
//  writing a new class that implements Observer — Stock is untouched.
// ============================================================

namespace with_pattern {

class IPriceObserver {
public:
    virtual ~IPriceObserver() = default;
    virtual void onPriceChanged(double price) = 0;
};

class Stock {
    double price = 0.0;
    vector<IPriceObserver*> observers;
public:
    void subscribe(IPriceObserver* o)   { observers.push_back(o); }
    void unsubscribe(IPriceObserver* o) {
        observers.erase(remove(observers.begin(), observers.end(), o),
                        observers.end());
    }

    void setPrice(double p) {
        price = p;
        for (auto* o : observers)
            o->onPriceChanged(p);
    }
};

class ConsoleDisplay : public IPriceObserver {
public:
    void onPriceChanged(double price) override {
        cout << "    [console] price = " << price << "\n";
    }
};

class ChartDisplay : public IPriceObserver {
public:
    void onPriceChanged(double price) override {
        cout << "    [chart  ] plot point " << price << "\n";
    }
};

class AlertSystem : public IPriceObserver {
public:
    void onPriceChanged(double price) override {
        if (price < 10.0)
            cout << "    [alert  ] price dropped below 10!\n";
    }
};

// Adding a new observer is now trivial — Stock doesn't change at all.
class AuditLog : public IPriceObserver {
public:
    void onPriceChanged(double price) override {
        cout << "    [audit  ] recorded " << price << "\n";
    }
};

void demo() {
    section("2. WITH Observer — Stock just notifies a list");

    Stock s;
    ConsoleDisplay c;
    ChartDisplay   ch;
    AlertSystem    a;
    AuditLog       audit;  // brand-new observer, Stock didn't change

    s.subscribe(&c);
    s.subscribe(&ch);
    s.subscribe(&a);
    s.subscribe(&audit);

    s.setPrice(42.0);
    s.setPrice(8.5);

    cout << "  Unsubscribing the chart and updating again:\n";
    s.unsubscribe(&ch);
    s.setPrice(15.0);
}

} // namespace with_pattern

// ============================================================
//  Why the Observer version is better
//   - Stock depends ONLY on the IPriceObserver interface. Adding a
//     new display means writing one new class — Stock is closed for
//     modification but open for extension.
//   - Subjects and observers can be wired together at runtime
//     (subscribe / unsubscribe), not bolted in at compile time.
//   - This is the same model that powers GUI event listeners,
//     pub/sub messaging, Qt signals & slots, etc.
//
//  Things to mention to students:
//   - Be careful with dangling pointers: if an observer dies before
//     unsubscribing, the subject still has its pointer. Real code
//     uses weak_ptr or auto-unsubscribe in the destructor.
//   - Don't let observers do heavy work — notification is synchronous.
// ============================================================

int main() {
    without_pattern::demo();
    with_pattern::demo();
    return 0;
}
