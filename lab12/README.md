# Lab 12 — Design Patterns

> **Read this first.** This is your study guide. The `.cpp` files are the
> examples you should run and play with. Open them next to this README.

---

## What is a design pattern, really?

Imagine you and a friend are cooking. You say: *"Let's make a sandwich."*
You both immediately picture roughly the same thing — bread, filling,
bread. You did not have to explain the steps. The word **sandwich** is a
shortcut for a recipe everyone already knows.

**Design patterns are sandwiches for programmers.** They are *named
arrangements of classes* that experienced developers have seen so many
times that, instead of describing them every time, we gave them names:
*Singleton, Factory, Observer…*

Patterns are not:
- A new feature of the language. You already know everything you need.
- A magic spell that makes code "good".
- A rulebook to follow without thinking.

Patterns are:
- A **vocabulary** so you can describe an architecture in two words.
- A **toolbox** of solutions to problems you will meet in the wild.
- A way to **recognize** code structure when you read someone else's project.

The classic catalog has 23 patterns (Gang of Four, 1994). We will study
**six** of them — the ones you are most likely to meet in real code
during your first jobs.

---

## How to use this lab

Each pattern lives in its own `.cpp` file. Every file has the same shape:

```cpp
namespace without_pattern { void demo(); }   // the painful way
namespace with_pattern    { void demo(); }   // the same problem, solved with the pattern

int main() {
    without_pattern::demo();
    with_pattern::demo();
}
```

**The whole point** is reading the two namespaces **side by side** and
feeling how much cleaner the second one is.

Compile and run any pattern with:

```
g++ -std=c++17 singleton.cpp -o singleton && ./singleton
```

---

## The three families

The Gang of Four put their 23 patterns into three buckets. We have two
from each:

| Family         | Question it answers          | Patterns in this lab        |
|----------------|------------------------------|-----------------------------|
| **Creational** | How are objects *built*?     | Singleton, Factory, Abstract Factory |
| **Structural** | How do objects *fit together*? | Adapter                   |
| **Behavioral** | How do objects *collaborate*? | Observer, Strategy         |

Don't memorize the table. By the end of the lab it will feel obvious.

---

# 1. Singleton — "there can be only one"

📄 File: [`singleton.cpp`](singleton.cpp) · **Family:** Creational

### The everyday analogy

Your country has **one president**. There is no "second president". When
someone says *"call the president"*, there is no ambiguity about which
person they mean. Government employees don't carry a President-pointer
around in their pocket — they just say "the president" and everyone
knows.

A **Singleton** is the same idea for a class: *exactly one instance,
globally reachable, and the language itself makes it impossible to
create a second one by accident.*

### The problem we are solving

A program has a `Logger` that writes to a file. The WHOLE program must
share the SAME logger — otherwise you'd open the same file twice, the
log lines would interleave randomly, and debugging would become hell.

How do you guarantee there is exactly one?

### The naive attempt (without the pattern)

> *"I'll just make it a global variable."*

```cpp
Logger globalLogger("global");          // top of some file

void moduleA() { globalLogger.log("hi"); }
void moduleB() {
    Logger rogue("rogue");              // ← OOPS, perfectly legal!
    rogue.log("hi");
}
```

Three problems:
1. **Nothing stops a second one.** Anyone can type `Logger l;` and the
   compiler is happy.
2. **Initialization order is unpredictable** between files. Your global
   might not exist yet when another global tries to use it.
3. **Tests can't replace it.** It's hardcoded into every call.

> *"OK, I'll pass it around as a parameter."*

```cpp
void moduleA(Logger& log);
void moduleB(Logger& log);
void moduleC(Logger& log);   // every function in the call chain
                             // grows a Logger& parameter it doesn't care about
```

Now your signatures are polluted. You're carrying the logger through
ten functions just so one of them can use it. The "logger" detail has
infected the API of unrelated code.

### The pattern

Three ingredients:
1. **Private constructor** — outsiders cannot type `Logger l;`.
2. **Deleted copy and assignment** — no smuggling copies out.
3. **Static `getInstance()`** — the *only* way to reach the object.

```cpp
class Logger {
private:
    Logger() { /* ... */ }
public:
    Logger(const Logger&)            = delete;
    Logger& operator=(const Logger&) = delete;

    static Logger& getInstance() {
        static Logger instance;     // built ONCE, on first call
        return instance;
    }

    void log(const std::string& msg);
};

// Usage anywhere in the code:
Logger::getInstance().log("hello");
```

The `static Logger instance` inside the function is called a **Meyers
Singleton**. The C++ standard guarantees:
- It is constructed **on first call**, not at program start.
- It is **thread-safe** since C++11.
- It is destroyed cleanly at program shutdown.

### When to use it

✅ A genuinely unique resource: logger, configuration object, connection
pool, hardware handle.

### When NOT to use it (read this twice)

❌ As a shortcut to avoid passing parameters. Singletons are global state
wearing a disguise. Overusing them makes your code:
- **Hard to test** — you can't easily swap in a fake `Logger` for a unit test.
- **Hard to reason about** — any function might secretly depend on it.
- **Impossible to have two of**, even when you later realize you want two.

A senior dev once said: *"Singletons are the global variables of the
object-oriented world — convenient at first, regretted later."* Believe
them. Use sparingly.

---

# 2. Factory Method — "ask, don't build"

📄 File: [`factory.cpp`](factory.cpp) · **Family:** Creational

### The everyday analogy

You walk into a **pizzeria**. You don't go into the kitchen, choose the
flour, decide on the oven temperature, and assemble the pizza yourself.
You say *"one margherita, please"* and the kitchen hands you the
finished thing. The kitchen is a **factory**. You — the customer —
don't need to know any of the recipes.

A **Factory** in code is the same: client code says *"give me a
margherita"* and a single function in the kitchen knows how to actually
construct the object.

### The problem we are solving

Your drawing app reads a string from the user — `"circle"`, `"square"`,
`"triangle"` — and creates the matching object. Several places in the
codebase do this: the renderer, the SVG exporter, the undo system, the
unit tests…

### Without the pattern

Every place does the same if/else:

```cpp
void renderUserShape(const std::string& type) {
    Shape* s = nullptr;
    if      (type == "circle")   s = new Circle();
    else if (type == "square")   s = new Square();
    else if (type == "triangle") s = new Triangle();
    s->draw();
    delete s;
}

void exportToSVG(const std::string& type) {
    // EXACT SAME if/else, copy-pasted. 😱
    Shape* s = nullptr;
    if      (type == "circle")   s = new Circle();
    else if (type == "square")   s = new Square();
    else if (type == "triangle") s = new Triangle();
    /* ... */
}
```

Now the boss says: *"add a Hexagon"*. You must find and update **every**
copy of that if/else. Miss one — hexagons silently break in the
exporter. This is a recipe for bugs.

### The pattern

Move the "string → class" decision into **one function**:

```cpp
class ShapeFactory {
public:
    static std::unique_ptr<Shape> create(const std::string& type) {
        if (type == "circle")   return std::make_unique<Circle>();
        if (type == "square")   return std::make_unique<Square>();
        if (type == "triangle") return std::make_unique<Triangle>();
        throw std::runtime_error("unknown shape: " + type);
    }
};

// Anywhere in the app:
auto s = ShapeFactory::create("circle");
s->draw();
```

**Adding a new shape now means editing exactly ONE function** —
`ShapeFactory::create`. None of the call sites change. None of them
even need to recompile.

This is the **Open/Closed Principle**: your code is *open* for
extension (you can add Hexagon) but *closed* for modification (the
callers don't change).

### Bonus: combined with smart pointers

Notice `create` returns `std::unique_ptr<Shape>` — see [lab 11](../lab11/smart_pointers.md).
The factory hands you **ownership** of a new object. When the
`unique_ptr` goes out of scope, the shape deletes itself. No more
`delete` to forget. No more leaks.

### When to use it

✅ You construct many objects of the same family based on some runtime
input (string, enum, config value, network message…).
✅ The set of concrete classes grows over time.
✅ You want client code to depend on the interface (`Shape`), not the
concrete classes (`Circle`, `Square`).

### When NOT to use it

❌ You only have two concrete classes and a single `if` covers it. A
factory for two cases is overkill — it's a sandwich made of bread.

---

# 3. Abstract Factory — "families of things that go together"

📄 File: [`abstract_factory.cpp`](abstract_factory.cpp) · **Family:** Creational

### The everyday analogy

Think of **IKEA furniture lines**. The "Malm" line has a Malm bed, a
Malm dresser, a Malm nightstand. They all match — same wood, same
style, same handle shape. The "Hemnes" line has its own bed, dresser,
nightstand. You don't mix a Malm bed with a Hemnes dresser — it would
look terrible. The store shows you one **family at a time**.

An **Abstract Factory** does that in code: it builds entire **families
of related objects** that must go together.

### The problem we are solving

You are writing a cross-platform GUI. On Windows you want a
`WinButton` AND a `WinCheckbox`. On macOS, both Mac. Mixing them — a
WinButton next to a MacCheckbox — would look broken and behave wrong.

How do you guarantee the matching ones always come together?

### Without the pattern

```cpp
Button*   btn = nullptr;
Checkbox* cb  = nullptr;

if (platform == "windows") {
    btn = new WinButton();
    cb  = new MacCheckbox();   // ← TYPO, but the type system is happy 😱
} else if (platform == "macos") {
    btn = new MacButton();
    cb  = new MacCheckbox();
}
```

Look at that bug. A WinButton paired with a MacCheckbox. The compiler
doesn't complain — both inherit from the right base classes. The bug
only surfaces when QA opens the app on Windows and sees a Mac checkbox.

### The pattern

Define **one interface** that knows how to make a whole family:

```cpp
class GUIFactory {
public:
    virtual std::unique_ptr<Button>   createButton()   const = 0;
    virtual std::unique_ptr<Checkbox> createCheckbox() const = 0;
};

class WinFactory : public GUIFactory {
public:
    std::unique_ptr<Button>   createButton()   const override { return std::make_unique<WinButton>(); }
    std::unique_ptr<Checkbox> createCheckbox() const override { return std::make_unique<WinCheckbox>(); }
};

class MacFactory : public GUIFactory { /* ...returns MacXxx... */ };
```

The application picks a factory **once, at startup** — then asks it for
widgets. The platform check disappears from everywhere else:

```cpp
void renderLoginForm(const GUIFactory& gui) {   // ← knows NOTHING about platforms
    auto btn = gui.createButton();
    auto cb  = gui.createCheckbox();
    btn->render();
    cb->render();
}
```

A `WinFactory` **literally cannot** return a `MacCheckbox` — its method
body doesn't mention that class. Mixing families is now a compile-time
impossibility.

### Factory vs. Abstract Factory — what's the difference?

| | Factory | Abstract Factory |
|--|---------|------------------|
| Creates | **one** thing | a **family** of related things |
| Example | "give me a Shape" | "give me the Windows family of widgets" |
| Picked at | each call | once, at startup |

If you only need one product, use Factory. If you need several products
that **must match each other**, use Abstract Factory.

### When to use it

✅ Cross-platform UI toolkits.
✅ Multiple "themes" or "skins" where many objects must match.
✅ Game engines: a "fantasy" enemy factory builds matching swords,
armor, enemies; a "sci-fi" factory builds matching lasers, ships,
aliens.

### When NOT to use it

❌ There's only one family today and you have no plan for another. Wait
until the second family actually appears before introducing this.

---

# 4. Adapter — "the universal travel plug"

📄 File: [`adapter.cpp`](adapter.cpp) · **Family:** Structural

### The everyday analogy

You travel from Europe to the US. Your laptop charger has a European
plug, the wall has an American socket. The shapes don't match. You buy
a **travel adapter** — a small thing with European holes on one side
and American prongs on the other. It doesn't change the electricity;
it just **translates the shape**.

That is exactly what the Adapter pattern does in code.

### The problem we are solving

Your app talks to payment processors through one interface:

```cpp
class IPaymentProcessor {
public:
    virtual void pay(double amount) = 0;
};
```

You want to integrate a third-party library, `LegacyBank`, that
already works but has a **totally different API**:

```cpp
class LegacyBank {
public:
    void connect();
    void sendMoney(const std::string& ccy, int cents);
    void disconnect();
};
```

And — important constraint — **you cannot modify `LegacyBank`**. It
ships as a binary.

### Without the pattern

The application learns to speak both dialects:

```cpp
void processPayment(/* ... */) {
    if (using_stripe) {
        stripe->pay(amount);
    } else if (using_legacy_bank) {
        bank->connect();
        bank->sendMoney("USD", amount * 100);
        bank->disconnect();
    } else if (using_some_third_thing) {
        /* yet another shape */
    }
}
```

The "connect → send → disconnect" ritual leaks into your application.
Forget the `disconnect()` once and you've got a resource leak in
production. Add a third provider and you get a third branch.

### The pattern

Write **one tiny wrapper class** that:
- *implements* the interface you want (`IPaymentProcessor`),
- *contains* the incompatible object (`LegacyBank`),
- *translates* one shape into the other.

```cpp
class LegacyBankAdapter : public IPaymentProcessor {
    LegacyBank& bank;
public:
    LegacyBankAdapter(LegacyBank& b) : bank(b) {}

    void pay(double amount) override {       // ← speaks our language…
        bank.connect();                      // …but internally speaks legacy.
        bank.sendMoney("USD", amount * 100);
        bank.disconnect();
    }
};
```

Now to the rest of the application, the legacy bank looks **exactly
like** every other payment processor:

```cpp
IPaymentProcessor& p = wrappedBank;
p.pay(9.99);   // one line, regardless of the underlying provider
```

The connect/disconnect ritual is captured **once**, inside the
adapter. Forgetting it at the call site is no longer possible — there
*is* no call site.

### Real-world examples you've already used

- `std::back_inserter(vec)` — makes a `std::vector` look like an output
  iterator. That's an adapter.
- C++ STL **container adapters**: `std::stack`, `std::queue` are
  adapters on top of `deque`/`vector`.
- Any library wrapping a C API in a C++ class. That's an adapter.

### When to use it

✅ You need to use code you cannot change (third-party library, legacy
binary, OS API) but its shape doesn't match the rest of your codebase.

### When NOT to use it

❌ You own both sides and could simply change one. Don't write an
adapter when a rename would do.

---

# 5. Observer — "subscribe and be notified"

📄 File: [`observer.cpp`](observer.cpp) · **Family:** Behavioral

### The everyday analogy

You **subscribe** to a YouTube channel. When the creator uploads a new
video, YouTube notifies *every* subscriber automatically. The creator
does not know your name, your inbox, or your favorite genre. They just
press "publish". YouTube handles the distribution.

If you unsubscribe, you stop receiving notifications. The creator's
workflow does not change at all.

That is the Observer pattern. The creator is the **subject**, the
viewers are the **observers**.

### The problem we are solving

A `Stock` class holds a stock's current price. When the price changes,
several things must update:
- a console display,
- a chart,
- an alert system that flashes red if the price drops.

And another team will add an email notifier next semester. Maybe a
fifth thing the year after.

### Without the pattern

```cpp
class Stock {
    ConsoleDisplay* console;   // Stock now KNOWS about every type
    ChartDisplay*   chart;
    AlertSystem*    alerts;

public:
    void setPrice(double p) {
        if (console) console->show(p);
        if (chart)   chart->plot(p);
        if (alerts)  alerts->check(p);
    }
};
```

Problems:
- `Stock` knows about every display type. **Tight coupling.**
- To add an `EmailNotifier`, you must edit the `Stock` class.
- The `Stock` class can't even be compiled without all three displays
  existing somewhere.

### The pattern

Define an `IPriceObserver` interface. `Stock` holds a `vector` of them
and walks the list — without knowing or caring what they really are:

```cpp
class IPriceObserver {
public:
    virtual void onPriceChanged(double price) = 0;
};

class Stock {
    std::vector<IPriceObserver*> observers;
public:
    void subscribe(IPriceObserver* o)   { observers.push_back(o); }
    void unsubscribe(IPriceObserver* o) { /* remove from list */ }

    void setPrice(double p) {
        for (auto* o : observers)
            o->onPriceChanged(p);          // Stock doesn't care WHO is listening
    }
};
```

Each display implements the interface:

```cpp
class ConsoleDisplay : public IPriceObserver {
    void onPriceChanged(double price) override { /* print it */ }
};
class ChartDisplay   : public IPriceObserver { /* ... */ };
class EmailNotifier  : public IPriceObserver { /* ← new! */ };
```

Adding `EmailNotifier` requires **zero changes** to `Stock`. Just write
the class and `stock.subscribe(&email)`.

### You have already met Observer everywhere

- **Qt signals & slots** — Observer.
- **JavaScript `addEventListener('click', ...)`** — Observer.
- **React component state subscriptions** — Observer.
- **Python's `@property.setter` triggering callbacks** — Observer.
- **News feeds, Twitter follows, Discord notifications** — Observer.

Once you can name it, you'll see it daily.

### A trap to know about

If an observer is destroyed but forgets to `unsubscribe`, the Stock
still holds a dangling pointer. The next `setPrice` will crash.

Solutions in real code:
- The observer's destructor calls `unsubscribe`.
- The subject holds `std::weak_ptr<Observer>` instead of raw pointers.

### When to use it

✅ One object's state change should ripple to several other objects.
✅ The set of "interested parties" varies (subscribe / unsubscribe).
✅ You want loose coupling — the subject shouldn't know what the
observers do.

### When NOT to use it

❌ Only one thing ever cares about the change. Just call it directly.

---

# 6. Strategy — "pick your algorithm at runtime"

📄 File: [`strategy.cpp`](strategy.cpp) · **Family:** Behavioral

### The everyday analogy

You open **Google Maps** and search for a route. You see three buttons:
🚗 car, 🚌 transit, 🚶 walking. The destination is the same. The map is
the same. What changes is the **algorithm** that computes the route. You
tap a button — the algorithm swaps. Same screen, different rules.

That swap is the Strategy pattern.

### The problem we are solving

An online store calculates an order's final price after a discount:
- Regular customers: no discount.
- Premium: 10% off.
- Employees: 30% off.
- Black Friday: variable.

Marketing keeps inventing new discount types. AND — the customer's
status can change mid-session (they sign up for Premium during
checkout).

### Without the pattern

One big if/else inside `Order`:

```cpp
double finalPrice() const {
    if (kind == CustomerKind::Regular)     return total;
    if (kind == CustomerKind::Premium)     return total * 0.90;
    if (kind == CustomerKind::Employee)    return total * 0.70;
    if (kind == CustomerKind::BlackFriday) return total < 100 ? total * 0.85 : total * 0.75;
    return total;
}
```

Problems:
- `Order` now knows **every business rule in the company**.
- You can't test the BlackFriday rule in isolation — you must build a
  full `Order` first.
- You can't reuse the math elsewhere.
- New rule = edit this function. Again. And again.

### The pattern

Make each rule its **own tiny class**, behind an interface:

```cpp
class DiscountStrategy {
public:
    virtual double compute(double total) const = 0;
};

class NoDiscount       : public DiscountStrategy { double compute(double t) const override { return t; } };
class PremiumDiscount  : public DiscountStrategy { double compute(double t) const override { return t * 0.90; } };
class EmployeeDiscount : public DiscountStrategy { double compute(double t) const override { return t * 0.70; } };
class BlackFriday      : public DiscountStrategy { /* ... */ };
```

`Order` just **delegates**:

```cpp
class Order {
    double total;
    std::unique_ptr<DiscountStrategy> strategy;
public:
    void setStrategy(std::unique_ptr<DiscountStrategy> s) { strategy = std::move(s); }
    double finalPrice() const { return strategy->compute(total); }
};
```

Now the magic:

```cpp
Order o(120, std::make_unique<NoDiscount>());
o.finalPrice();                                       // → 120

o.setStrategy(std::make_unique<PremiumDiscount>());   // customer signed up!
o.finalPrice();                                       // → 108
```

The **same order** changed its discount rule at runtime. No new Order
needed. No if/else.

### Where you have seen Strategy already

```cpp
std::sort(v.begin(), v.end(), [](int a, int b) { return a > b; });
//                            ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//                            this comparator is a STRATEGY
```

The sorting **algorithm** is fixed. The **strategy** for comparing two
elements is something you plug in from the outside. That's Strategy.

Other examples:
- A game's AI difficulty (Easy/Medium/Hard) — different strategies for
  picking moves.
- File compression: pick `gzip`, `bzip2`, `lz4` at runtime.
- Authentication: password / OAuth / SSO — all the same "authenticate
  this user" idea, different algorithms.

### Strategy vs. Factory — don't confuse them

| | Factory | Strategy |
|--|---------|----------|
| Question | *Which class do I build?* | *Which algorithm do I run?* |
| Returns | a new object | nothing — it just *does* something |
| Lifetime | one-shot (the build) | lasts as long as the host object |

They sometimes work **together**: a Factory builds a Strategy and hands
it to a host object.

### When to use it

✅ You have several variants of "how to do X" and the choice depends on
runtime conditions or user choice.
✅ You want each algorithm testable in isolation.

### When NOT to use it

❌ There's only one algorithm and there will only ever be one. An
if/else is fine then.

---

# Putting it all together

By now, here is the only mental table you need:

| Pattern          | "What is allowed to vary?"               | "What stays fixed?"           |
|------------------|------------------------------------------|-------------------------------|
| Singleton        | Number of instances (always 1)           | The constructor is hidden     |
| Factory          | Which concrete class is built            | The interface returned        |
| Abstract Factory | Which **family** of classes is built     | The factory interface         |
| Adapter          | The shape of an external API             | The interface clients use     |
| Observer         | Who is listening                         | The notification mechanism    |
| Strategy         | Which algorithm is used                  | The class that uses it        |

Every pattern answers the same kind of question: *what should be easy
to change, and what should stay locked down?* That is **the whole point
of OOP design.**

---

## Two important warnings before you go pattern-crazy

### 1. Don't add a pattern until the pain shows up.

Three Shape classes don't need a Factory. *Three hundred do.* Patterns
are answers to **real problems**. If you don't have the problem yet,
don't add the solution — you're just adding complexity for free.

### 2. A pattern is a name, not a religion.

If a one-line `if/else` solves your problem, **write the if/else**.
You're not failing OOP. The whole point of patterns is to make code
clearer; a Strategy class for two cases makes it *less* clear.

The senior dev's rule of thumb:
> "Write the dumb version first. When you have to change it for the
> third time, the right pattern will be obvious."

---

## Things to try (homework ideas)

1. **Extend `factory.cpp`** — add a `Hexagon`. Notice how few places
   need to change.
2. **Extend `observer.cpp`** — add an `EmailNotifier`. Did you have to
   touch `Stock`? You shouldn't have to.
3. **Extend `strategy.cpp`** — add a `StudentDiscount` (15% off if total
   > 50, otherwise no discount). Plug it in at runtime.
4. **Write an Adapter** — imagine a third payment provider with this
   weird API: `OldPayPal::charge(string accountId, long microcents)`.
   Wrap it as an `IPaymentProcessor` without touching the rest of the
   app.

For each one, **don't just write the code** — write a 3-line comment
explaining *which pattern* you used and *why*.

---

## Further reading

- *Design Patterns: Elements of Reusable Object-Oriented Software* —
  Gamma, Helm, Johnson, Vlissides (1994). "The Gang of Four book". The
  original catalog of 23 patterns. Dense, but it's the source.
- *Head First Design Patterns* — Freeman & Robson. The friendly,
  illustrated version. Excellent for self-study.
- [refactoring.guru/design-patterns](https://refactoring.guru/design-patterns) —
  free online catalog with diagrams and examples in multiple
  languages. Great reference.

---

**Now open `singleton.cpp` and read it side by side with this section.
That's the real lesson.**
