/*
 * Lab 12 — The ADAPTER Pattern
 *
 * Problem: Our application talks to payment processors through a
 *   common interface, IPaymentProcessor::pay(amount). We want to
 *   integrate a third-party library, LegacyBank, that already works
 *   but has a totally different API:
 *
 *       legacy.connect();
 *       legacy.sendMoney("USD", amountInCents);
 *       legacy.disconnect();
 *
 *   We are NOT allowed to modify LegacyBank (it ships as a binary).
 *
 * We will solve the problem twice:
 *   1) WITHOUT the pattern — every call site checks "is it the
 *      legacy one?" and uses the alternate API by hand
 *   2) WITH the Adapter pattern — one small class wraps LegacyBank
 *      and exposes it as an IPaymentProcessor
 */

#include <iostream>
#include <string>
#include <vector>

using namespace std;

void section(const string& title) {
    cout << "\n══════════════════════════════════════════\n";
    cout << "  " << title << "\n";
    cout << "══════════════════════════════════════════\n";
}

// ============================================================
//  The interface our application is written against
// ============================================================
class IPaymentProcessor {
public:
    virtual ~IPaymentProcessor() = default;
    virtual void pay(double amount) = 0;
};

// A modern processor that already implements the interface natively
class StripeProcessor : public IPaymentProcessor {
public:
    void pay(double amount) override {
        cout << "    [Stripe] charged $" << amount << "\n";
    }
};

// ============================================================
//  The "incompatible" third-party class.
//  Pretend this is in a binary library — you can't change it.
// ============================================================
class LegacyBank {
public:
    void connect()                                  { cout << "    [LegacyBank] connecting...\n"; }
    void sendMoney(const string& ccy, int cents)    { cout << "    [LegacyBank] sent " << cents << " " << ccy << " cents\n"; }
    void disconnect()                               { cout << "    [LegacyBank] disconnecting...\n"; }
};

// ============================================================
//  1. WITHOUT the pattern
//
//  The application code grows aware of both APIs. Every place that
//  could see either processor needs an if-branch and has to remember
//  the connect/disconnect ritual. Add a third processor with yet
//  another API and the branches multiply.
// ============================================================

namespace without_pattern {

// Pseudo-union: hold one or the other, plus a tag.
struct Processor {
    IPaymentProcessor* modern = nullptr;
    LegacyBank*        legacy = nullptr;
};

void processPayment(const Processor& p, double amount) {
    if (p.modern) {
        p.modern->pay(amount);
    } else if (p.legacy) {
        // Caller of processPayment has to remember the whole protocol.
        p.legacy->connect();
        p.legacy->sendMoney("USD", static_cast<int>(amount * 100));
        p.legacy->disconnect();
    }
}

void demo() {
    section("1. WITHOUT Adapter — call sites know about both APIs");

    StripeProcessor stripe;
    LegacyBank      bank;

    Processor a; a.modern = &stripe;
    Processor b; b.legacy = &bank;

    vector<Processor> processors = { a, b };
    for (auto& p : processors)
        processPayment(p, 9.99);

    cout << "  Every function that handles a payment must know both\n";
    cout << "  APIs. Adding another legacy provider means more branches.\n";
}

} // namespace without_pattern

// ============================================================
//  2. WITH the Adapter pattern
//
//  Write ONE small class — LegacyBankAdapter — that:
//    - inherits IPaymentProcessor (so the app can use it normally)
//    - holds a LegacyBank inside it
//    - translates pay(amount) into the legacy protocol
//
//  After that, the rest of the application doesn't even know
//  LegacyBank exists.
// ============================================================

namespace with_pattern {

class LegacyBankAdapter : public IPaymentProcessor {
    LegacyBank& bank;
public:
    LegacyBankAdapter(LegacyBank& b) : bank(b) {}

    void pay(double amount) override {
        bank.connect();
        bank.sendMoney("USD", static_cast<int>(amount * 100));
        bank.disconnect();
    }
};

void processPayment(IPaymentProcessor& p, double amount) {
    p.pay(amount);   // one line, regardless of the underlying provider
}

void demo() {
    section("2. WITH Adapter — both processors look identical to clients");

    StripeProcessor   stripe;
    LegacyBank        bank;
    LegacyBankAdapter wrappedBank(bank);

    vector<IPaymentProcessor*> processors = { &stripe, &wrappedBank };
    for (auto* p : processors)
        processPayment(*p, 9.99);

    cout << "  processPayment() is now ONE line, identical for both.\n";
    cout << "  The translation lives inside LegacyBankAdapter only.\n";
}

} // namespace with_pattern

// ============================================================
//  Why the Adapter version is better
//   - The legacy protocol (connect → send → disconnect) is captured
//     ONCE, in the adapter. Forgetting `disconnect()` is no longer
//     possible at the call site.
//   - The rest of the application depends on IPaymentProcessor only.
//     It works with code we don't own without contaminating itself.
//   - Adding another oddly-shaped provider means writing one more
//     adapter — and nothing else changes.
//
//  This is exactly how the C++ standard library treats e.g. iterators:
//  std::back_inserter is an adapter that makes a container look like
//  an output iterator.
// ============================================================

int main() {
    without_pattern::demo();
    with_pattern::demo();
    return 0;
}
