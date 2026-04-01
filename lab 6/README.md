# Lab 6 — Inheritance and Polymorphism

> **Course:** Object-Oriented Programming
> **Topics:** Inheritance ("is-a") · `protected` · Constructor chaining · Pure virtual functions · Abstract classes · `virtual` & dynamic dispatch · `override` · Virtual destructor · Polymorphic collections

---

## Table of Contents

1. [What's new in Lab 6?](#1-whats-new-in-lab-6)
2. ["Is-a" vs. "Has-a"](#2-is-a-vs-has-a)
3. [Inheritance Syntax](#3-inheritance-syntax)
4. [The `protected` Access Specifier](#4-the-protected-access-specifier)
5. [Constructor Chaining](#5-constructor-chaining)
6. [Pure Virtual Functions and Abstract Classes](#6-pure-virtual-functions-and-abstract-classes)
7. [The `override` Keyword](#7-the-override-keyword)
8. [Virtual Functions and Dynamic Dispatch](#8-virtual-functions-and-dynamic-dispatch)
9. [The Virtual Destructor](#9-the-virtual-destructor)
10. [Copy Constructor in Derived Classes](#10-copy-constructor-in-derived-classes)
11. [Polymorphic Collections](#11-polymorphic-collections)
12. [Lab Code Walkthrough](#12-lab-code-walkthrough)
13. [Common Mistakes](#13-common-mistakes)
14. [Quick Recap](#14-quick-recap)
15. [Diamond Inheritance](#15-diamond-inheritance)
16. [Further Reading](#16-further-reading)

---

## 1. What's new in Lab 6?

Back in Lab 2, we introduced the four fundamental principles of OOP — encapsulation, abstraction, inheritance, and polymorphism — and said the last two would come in later labs. This is that lab.

Labs 2–4 taught you how to model one thing well (a bank account, an animal, a person). Lab 6 asks a different question: **what do a Circle, a Rectangle, and a Triangle have in common?** They are all shapes. They all have a color, an area, and a perimeter. Instead of writing three separate, unrelated classes, we capture that shared structure once in a **base class** and let each concrete shape **inherit** from it.

New concepts introduced in this lab:

- **Inheritance** — deriving a new class from an existing one, reusing its data and behavior.
- **`protected`** — a third access level, visible to the class and its children but not to the outside world.
- **Constructor chaining** — calling the base class constructor from a derived class.
- **Pure virtual functions** — methods the base class declares but deliberately leaves unimplemented, forcing derived classes to provide them.
- **Abstract classes** — classes with at least one pure virtual function; they cannot be instantiated directly.
- **`virtual` and dynamic dispatch** — the mechanism that makes a base-class pointer call the correct derived-class method at runtime.
- **The `override` keyword** — a safety annotation that tells the compiler "I intend to override a virtual method".
- **The virtual destructor** — without it, deleting a derived object through a base pointer is undefined behavior.
- **Polymorphic collections** — storing mixed derived types in a single `vector<Base*>`.

---

## 2. "Is-a" vs. "Has-a"

Lab 4 introduced **composition** — a `Person` *has-a* `Animal`. The animal is a member of the person; they are independent things that happen to be associated.

Lab 6 introduces **inheritance** — a `Circle` *is-a* `Shape`. A circle is not just associated with a shape; it *is one*. Everywhere you can use a `Shape`, you can use a `Circle` instead — they share the same interface.

| Relationship | Keyword | Example |
|---|---|---|
| Has-a (composition) | member pointer / value | `Person` has an `Animal* pet` |
| Is-a (inheritance) | `: public Base` | `Circle` is a `Shape` |

The "is-a" test: substitute the derived type into a sentence about the base. "A Circle is a Shape" ✅. "A Person is an Animal" ❌ — so inheritance would be the wrong tool there.

---

## 3. Inheritance Syntax

To say that `Circle` inherits from `Shape`, write:

```cpp
class Circle : public Shape {
    double radius;
public:
    Circle(double radius, const string& color);
    double area()      const override;
    double perimeter() const override;
    string typeName()  const override;
};
```

The `: public Shape` part is the **access specifier for inheritance**. With `public` inheritance (the most common kind):

- `public` members of `Shape` remain `public` in `Circle`.
- `protected` members of `Shape` remain `protected` in `Circle`.
- `private` members of `Shape` still exist inside a `Circle` object, but `Circle`'s own code **cannot access them directly**; only `Shape`'s own methods can.

Think of `Circle` as `Shape` plus extra. Every `Circle` object contains everything a `Shape` object contains, plus its own `radius`. When you call `circle.getColor()`, it uses the method defined in `Shape` — `Circle` did not have to re-implement it.

```
Memory layout of a Circle object:
┌──────────────────────────────┐
│  Shape subobject             │
│  ├── int id                  │
│  └── string color            │
├──────────────────────────────┤
│  Circle's own data           │
│  └── double radius           │
└──────────────────────────────┘
```

---

## 4. The `protected` Access Specifier

We now have three access levels:

| Specifier | Accessible from | Use case |
|---|---|---|
| `public` | Anywhere | The class's outward interface |
| `protected` | The class itself + derived classes | Data/helpers that derived classes need |
| `private` | Only the class itself | Internal implementation detail |

In `Shape`:

```cpp
class Shape {
protected:
    static int noShapes;
    const int id;
    string color;
    ...
};
```

`color` is `protected` because derived classes like `Circle` may legitimately need to read or use it (for example, when printing). If it were `private`, `Circle` could not touch it at all — every access would have to go through a getter. If it were `public`, any code anywhere could modify it freely, breaking encapsulation.

`protected` is the middle ground: it says *"this is an internal detail, but I trust my subclasses to use it responsibly"*.

---

## 5. Constructor Chaining

When a `Circle` is created, a `Shape` is also created inside it — and the `Shape` part needs its own constructor to run. You trigger this explicitly in the **member initializer list**:

```cpp
Circle::Circle(double radius, const string& color)
    : Shape(color), radius(radius) {}
```

The `: Shape(color)` part calls `Shape`'s constructor before `Circle`'s body executes. This is mandatory — the base class part of the object **must** be initialized before the derived class can use it.

If you omit the base constructor call, the compiler tries to call `Shape`'s **default constructor** (the one with no arguments). If no such constructor exists, you get a compile error.

The chain with three levels would look like this:

```
Circle constructor called
  → Shape constructor runs first (initializes id, color)
  → Circle constructor body runs (initializes radius)
```

The same pattern applies to the copy constructor:

```cpp
Circle::Circle(const Circle& other)
    : Shape(other), radius(other.radius) {}
```

`Shape(other)` calls `Shape`'s copy constructor, which copies `color` and assigns a **new `id`**. Then `radius` is copied. The result is a fully independent `Circle` with its own identity.

---

## 6. Pure Virtual Functions and Abstract Classes

A `Shape` on its own is incomplete — it has no meaningful implementation of `area()`. The formula for a circle's area is completely different from a rectangle's. So instead of inventing a placeholder formula in `Shape`, we declare the method as **pure virtual**:

```cpp
class Shape {
public:
    virtual double area()      const = 0;
    virtual double perimeter() const = 0;
    virtual string typeName()  const = 0;
};
```

The `= 0` at the end means: *"I declare this method exists, but I provide no implementation. Any concrete class inheriting from me must implement it."*

A class with at least one pure virtual function is called an **abstract class**. You cannot create objects of an abstract class directly:

```cpp
Shape s("red");   // ERROR: cannot instantiate abstract class
```

This is intentional and useful — `Shape` is a concept, not a concrete thing. You can never hold "a shape" in your hand; you hold a circle, a rectangle, or a triangle. The abstract class enforces this at compile time.

Derived classes that implement all pure virtual methods are called **concrete classes** and can be instantiated:

```cpp
Circle c(5.0, "red");   // OK — Circle implements area(), perimeter(), typeName()
```

If a derived class fails to implement even one pure virtual method, it too becomes abstract and cannot be instantiated.

---

## 7. The `override` Keyword

When a derived class provides its own version of a virtual function, it **overrides** it. Marking this explicitly with `override` is strongly recommended:

```cpp
class Circle : public Shape {
    double area()      const override;
    double perimeter() const override;
    string typeName()  const override;
};
```

`override` instructs the compiler to **verify** that the method actually overrides a virtual method in the base class. Without it, two easy mistakes go undetected:

**Mistake 1 — typo in the name:**
```cpp
double Cirlce::aera() const { ... }   // no override → new unrelated method, base version still called
double Circle::area() const override { ... }   // override → compiler catches the typo
```

**Mistake 2 — wrong signature:**
```cpp
double Circle::area() { ... }           // missing const → different signature, not an override
double Circle::area() const override;   // override → compiler catches the mismatch
```

`override` has zero runtime cost — it is a compile-time check only. Always use it.

---

## 8. Virtual Functions and Dynamic Dispatch

This is the core of polymorphism. Consider this code:

```cpp
Shape* s = new Circle(5.0, "red");
cout << s->area();
```

`s` is a `Shape*`, but it points to a `Circle` object. Which `area()` is called — `Shape`'s or `Circle`'s?

**Without `virtual`:** the compiler decides at compile time based on the **declared type** of the pointer (`Shape*`). It would call `Shape::area()` — the wrong one.

**With `virtual`:** the compiler generates a runtime lookup. When `s->area()` executes, the program checks what the pointer actually points to (a `Circle`) and calls `Circle::area()`. This is called **dynamic dispatch**.

```cpp
// Without virtual — STATIC dispatch (wrong)
class Shape {
    double area() const { return 0; }   // no virtual
};
Shape* s = new Circle(5.0, "red");
s->area();   // calls Shape::area() → returns 0. Wrong!

// With virtual — DYNAMIC dispatch (correct)
class Shape {
    virtual double area() const = 0;
};
Shape* s = new Circle(5.0, "red");
s->area();   // calls Circle::area() → returns 78.54. Correct!
```

The demo in the lab makes this contrast explicit with `BaseNoVirtual` / `DerivedNoVirtual` vs. `BaseVirtual` / `DerivedVirtual`:

```cpp
BaseNoVirtual* p1 = new DerivedNoVirtual();
BaseVirtual*   p2 = new DerivedVirtual();

p1->whoAmI();   // prints "Base (no virtual)"    ← static dispatch
p2->whoAmI();   // prints "Derived (virtual)"    ← dynamic dispatch
```

**How does dynamic dispatch work internally?**

Every class with virtual functions gets a hidden **vtable** (virtual function table) — an array of function pointers, one per virtual method. Every object of that class gets a hidden pointer (the **vptr**) to its class's vtable. When `s->area()` is called, the program follows the vptr to the vtable and calls whatever function is stored there. For a `Circle` object, that entry points to `Circle::area`.

```
Circle object in memory:
┌──────────────────────┐
│  vptr ──────────────────→ Circle vtable
│  id                  │       [0] Circle::area
│  color               │       [1] Circle::perimeter
│  radius              │       [2] Circle::typeName
└──────────────────────┘       [3] Circle::~Circle
```

---

## 9. The Virtual Destructor

This is a critical rule that trips up almost every programmer the first time:

> **If a class has any virtual functions, its destructor must also be virtual.**

Here is why. Suppose `Shape`'s destructor is not virtual:

```cpp
class Shape {
    ~Shape() {}   // NOT virtual
};

Shape* s = new Circle(5.0, "red");
delete s;
```

When `delete s` runs, the compiler looks at the **static type** of the pointer (`Shape*`) and calls `Shape::~Shape`. `Circle::~Circle` is never called. If `Circle` had dynamically allocated members, they would never be freed — a memory leak. In general, this is **undefined behavior**.

The fix is one word:

```cpp
class Shape {
    virtual ~Shape() {}   // virtual — derived destructors will always be called
};
```

Now when `delete s` runs, the runtime looks up the vtable, finds `Circle::~Circle`, calls it (which in turn calls `Shape::~Shape` automatically), and everything is cleaned up correctly.

**Rule of thumb:** if you write a class intended to be used as a base class, always declare its destructor `virtual`, even if the body is empty.

---

## 10. Copy Constructor in Derived Classes

When you copy a derived object, you must also copy the base part. The derived copy constructor must explicitly call the **base copy constructor** in its initializer list:

```cpp
Circle::Circle(const Circle& other)
    : Shape(other), radius(other.radius) {}
//    ^^^^^^^^^^^
//    calls Shape(const Shape& other) — copies color, assigns new id
```

If you write `: radius(other.radius)` and omit `Shape(other)`, the compiler calls `Shape`'s default constructor instead, which would reset `color` to an empty string and assign a new id anyway — but the `color` would be wrong.

Notice that the copy constructor in `Shape` assigns a **new `id`**:

```cpp
Shape::Shape(const Shape& other)
    : id(++noShapes), color(other.color) {}
```

This is a deliberate design choice: a copied shape is a new, independent object with its own identity. It should not share an id with the original.

```
Circle original(7.0, "pink");   // id = 8 (for example)
Circle copy = original;          // id = 9  ← new id, independent object
```

---

## 11. Polymorphic Collections

The most powerful application of inheritance and virtual functions is the ability to store objects of different derived types in a single container:

```cpp
vector<Shape*> shapes;
shapes.push_back(new Circle    (5.0,        "red"   ));
shapes.push_back(new Rectangle (4.0, 6.0,   "blue"  ));
shapes.push_back(new Triangle  (3.0, 4.0, 5.0, "green"));
```

The vector holds `Shape*` pointers, but each pointer actually points to a `Circle`, `Rectangle`, or `Triangle`. When you iterate:

```cpp
for (const Shape* s : shapes)
    cout << s->area() << '\n';
```

each call to `s->area()` uses dynamic dispatch to invoke the correct formula for that shape. The loop does not need to know what type each element is — it just calls `area()` and the right implementation runs automatically.

This is the payoff of polymorphism: you can write **generic algorithms** (find the largest, sum all areas, print all shapes) that work on any current or future derived type without modification. Adding a new shape type — `Pentagon`, `Ellipse` — requires no changes to the loop at all; you just add the new class.

**Cleanup:** because you `new`'d each shape, you must `delete` each one. Storing pointers in a vector does not free them automatically:

```cpp
for (Shape* s : shapes)
    delete s;   // virtual destructor ensures the correct derived destructor runs
shapes.clear();
```

---

## 12. Lab Code Walkthrough

### The Shape hierarchy

```
Shape  (abstract)
 ├── Circle      (radius)
 ├── Rectangle   (width, height)
 └── Triangle    (a, b, c)
```

`Shape` holds the shared data (`id`, `color`) and declares the interface (`area()`, `perimeter()`, `typeName()`). Each derived class stores its own geometry and implements the interface.

---

### Abstract class enforcement

```cpp
// Shape s("red");   // uncomment → compile error
```

Attempting to create a `Shape` directly fails at compile time. The comment in `main` invites students to try it and read the error message.

---

### Static vs. dynamic dispatch side by side

```cpp
BaseNoVirtual* p1 = new DerivedNoVirtual();
BaseVirtual*   p2 = new DerivedVirtual();

p1->whoAmI();   // "Base (no virtual)"    — static dispatch
p2->whoAmI();   // "Derived (virtual)"    — dynamic dispatch
```

Both pointers hold a derived object. The only difference is whether `whoAmI` is `virtual`. This isolates the effect of `virtual` from all other concerns.

---

### Polymorphic iteration

```cpp
for (const Shape* s : shapes)
    cout << *s << '\n';
```

`operator<<` is defined for `Shape` and calls `s.typeName()`, `s.area()`, and `s.perimeter()` — all virtual. The correct derived implementations run for each element.

---

### Finding the largest area

```cpp
Shape* largest = shapes[0];
for (Shape* s : shapes)
    if (s->area() > largest->area())
        largest = s;
```

Classic linear search over a polymorphic collection. `area()` is called virtually — no `if`/`switch` on type needed.

---

### Copy constructor demonstration

```cpp
Circle original(7.0, "pink");
Circle copy = original;
```

Both are independent `Circle` objects with different ids, identical color and radius. Modifying one does not affect the other.

---

### Cleanup

```cpp
for (Shape* s : shapes)
    delete s;
```

Because `Shape::~Shape` is `virtual`, each `delete` calls the correct derived destructor (`Circle::~Circle`, etc.), then the base destructor. No resources are leaked.

---

## 13. Common Mistakes

| Mistake | Description | Fix |
|---|---|---|
| Non-virtual destructor in base class | `delete base_ptr` only calls base destructor; derived destructor skipped | Always declare `virtual ~Base() {}` in any class meant to be inherited |
| Forgetting `override` | Typo or signature mismatch silently creates a new unrelated method | Always write `override` on every method you intend to override |
| Omitting base constructor in derived initializer list | Base part initialized with default constructor; data may be wrong | Write `: Base(args)` explicitly in derived constructor |
| Trying to instantiate an abstract class | Compile error if any pure virtual method is unimplemented | Implement all `= 0` methods in the derived class |
| Accessing base `private` members from derived class | Compile error even though the member "is inside" the object | Change to `protected` or use a getter |
| Forgetting to `delete` vector elements | Pointers in `vector` are NOT freed when the vector is destroyed | Loop and `delete` each pointer, or use smart pointers |
| Copying a derived object without chaining copy constructor | Base part not properly copied (color reset, wrong id logic) | Write `: Base(other)` in the derived copy constructor |

---

## 14. Quick Recap

| Concept | What it is | Where it appears in code |
|---|---|---|
| **Inheritance** | A derived class extends a base class, reusing its data and behavior | `class Circle : public Shape` |
| **"is-a"** | The relationship expressed by inheritance | A Circle is a Shape |
| **`protected`** | Accessible inside the class and by derived classes; not from outside | `protected: string color;` |
| **Constructor chaining** | Derived constructor explicitly calls base constructor | `: Shape(color)` in initializer list |
| **Pure virtual function** | `= 0` — declared but not implemented; must be overridden | `virtual double area() const = 0;` |
| **Abstract class** | Has at least one pure virtual method; cannot be instantiated | `Shape` — `Shape s;` is a compile error |
| **`override`** | Compiler-verified annotation that a method overrides a base virtual | `double area() const override;` |
| **`virtual`** | Enables dynamic dispatch — correct derived method called at runtime | `virtual double area() const = 0;` |
| **Dynamic dispatch** | Runtime lookup of the correct method via vtable | `shape_ptr->area()` calls the right type's `area()` |
| **Static dispatch** | Compile-time resolution based on pointer type (no `virtual`) | Wrong method called when polymorphism is needed |
| **Virtual destructor** | Ensures correct derived destructor is called when deleting via base pointer | `virtual ~Shape() {}` |
| **Polymorphic collection** | `vector<Base*>` holding mixed derived types | `vector<Shape*> shapes` |
| **Heron's formula** | `sqrt(s*(s-a)*(s-b)*(s-c))` where `s = (a+b+c)/2` | `Triangle::area()` |

---

## 15. Diamond Inheritance

### The problem

When two intermediate classes both inherit from the same base, and a third class inherits from both, the hierarchy forms a **diamond**:

```
         Shape  (abstract)
        /      \
  Outlined     Filled
        \      /
      FilledCircle
```

Without any special treatment, `FilledCircle` would inherit `Shape` **twice** — once through `Outlined` and once through `Filled`. This creates two copies of every `Shape` member (`id`, `color`, vptr). Any attempt to access them is **ambiguous**:

```cpp
FilledCircle fc(...);
fc.getId();   // ERROR: ambiguous — which Shape::getId()? Outlined's or Filled's?
```

### The fix: `virtual` inheritance

Mark the inheritance of `Shape` as `virtual` in both intermediate classes:

```cpp
class Outlined : virtual public Shape { ... };
class Filled   : virtual public Shape { ... };
```

The `virtual` keyword tells the compiler: *"if multiple paths lead to this base, share one subobject instead of duplicating it."* `FilledCircle` then contains exactly **one** `Shape` subobject, accessed unambiguously from anywhere in the hierarchy.

### Memory layout comparison

**Without** `virtual` inheritance:

```
FilledCircle object
├── Outlined subobject
│   └── Shape subobject  ← copy 1 (id, color, vptr)
├── Filled subobject
│   └── Shape subobject  ← copy 2 (id, color, vptr)  DUPLICATE
└── double radius
```

**With** `virtual` inheritance:

```
FilledCircle object
├── Shape subobject       ← single shared copy (id, color, vptr)
├── Outlined subobject    (strokeWidth)
├── Filled subobject      (fillPattern)
└── double radius
```

### Rule: the most-derived class initializes the virtual base

With virtual inheritance, the **most-derived class** (`FilledCircle`) is always responsible for calling the virtual base's constructor directly. The intermediate classes' calls to `Shape(...)` are **skipped** when they are used as an intermediate, not as the most-derived class:

```cpp
FilledCircle::FilledCircle(double radius, const string& color,
                            double strokeWidth, const string& fillPattern)
    : Shape(color),                   // virtual base — only this call runs
      Outlined(color, strokeWidth),   // Shape(color) inside Outlined is skipped
      Filled(color, fillPattern),     // Shape(color) inside Filled is skipped
      radius(radius) {}
```

Intermediate classes must still list the virtual base in their own initializer lists (for when they are the most-derived class), but those calls are bypassed when a further-derived class takes over.

### Demonstration

```cpp
FilledCircle fc(3.0, "cyan", 1.5, "hatched");

Outlined* op = &fc;
Filled*   fp = &fc;

cout << op->getId();   // e.g. 9
cout << fp->getId();   // 9 — same id, single shared base
```

Both intermediate pointers agree on the id because they refer to the same `Shape` subobject. Without virtual inheritance they would disagree — or the code would not compile at all.

---

## 16. Further Reading

### Official References
- [cppreference — Derived classes](https://en.cppreference.com/w/cpp/language/derived_class)
- [cppreference — Virtual functions](https://en.cppreference.com/w/cpp/language/virtual)
- [cppreference — Abstract classes](https://en.cppreference.com/w/cpp/language/abstract_class)
- [cppreference — override specifier](https://en.cppreference.com/w/cpp/language/override)

### Tutorials & Guides
- [LearnCpp.com — Introduction to inheritance](https://www.learncpp.com/cpp-tutorial/introduction-to-inheritance/)
- [LearnCpp.com — Virtual functions](https://www.learncpp.com/cpp-tutorial/virtual-functions/)
- [LearnCpp.com — Pure virtual functions, abstract base classes](https://www.learncpp.com/cpp-tutorial/pure-virtual-functions-abstract-base-classes-and-interface-classes/)
- [LearnCpp.com — Virtual destructors](https://www.learncpp.com/cpp-tutorial/virtual-destructors-virtual-assignment-and-overriding-virtualization/)

### Videos
- [The Cherno — Inheritance in C++](https://www.youtube.com/watch?v=X8nYM8wdNRE)
- [The Cherno — Virtual Functions in C++](https://www.youtube.com/watch?v=oIV2KchSyGQ)
- [The Cherno — Pure Virtual Functions (Interfaces) in C++](https://www.youtube.com/watch?v=UWAdd13EfM8)

### Books
- *C++ Primer* (Lippman, Lajoie, Moo) — Chapter 15: Object-Oriented Programming
- *A Tour of C++* (Bjarne Stroustrup) — Chapter 4: Classes
