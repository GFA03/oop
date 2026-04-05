# Lab 7 — Type Casting, Exception Handling, and Interfaces

> **Course:** Object-Oriented Programming
> **Topics:** C++ named casts · `static_cast` · `dynamic_cast` · `const_cast` · `reinterpret_cast` · Exception hierarchy · `throw` / `try` / `catch` · Stack unwinding · Interfaces (pure abstract contracts)

---

## Table of Contents

1. [What's new in Lab 7?](#1-whats-new-in-lab-7)
2. [Why C-style casts are dangerous](#2-why-c-style-casts-are-dangerous)
3. [Overview of C++ named casts](#3-overview-of-c-named-casts)
4. [`static_cast`](#4-static_cast)
   - [Numeric conversions](#41-numeric-conversions)
   - [Upcasting](#42-upcasting-derived--base)
   - [Downcasting — the danger](#43-downcasting--the-danger)
5. [`dynamic_cast`](#5-dynamic_cast)
   - [Pointer form](#51-pointer-form)
   - [Reference form](#52-reference-form)
   - [How it works — RTTI](#53-how-it-works--rtti)
6. [`const_cast`](#6-const_cast)
7. [`reinterpret_cast`](#7-reinterpret_cast)
8. [Exception Handling](#8-exception-handling)
   - [throw, try, catch](#81-throw-try-catch)
   - [The standard exception hierarchy](#82-the-standard-exception-hierarchy)
   - [Custom exception classes](#83-custom-exception-classes)
   - [Order of catch blocks](#84-order-of-catch-blocks)
   - [Throwing from a constructor](#85-throwing-from-a-constructor)
   - [Stack unwinding](#86-stack-unwinding)
9. [Interfaces](#9-interfaces)
   - [Interface vs. abstract base class](#91-interface-vs-abstract-base-class)
   - [Satisfying multiple interfaces](#92-satisfying-multiple-interfaces)
   - [Programming to an interface](#93-programming-to-an-interface)
10. [Lab Code Walkthrough](#10-lab-code-walkthrough)
11. [Common Mistakes](#11-common-mistakes)
12. [Quick Recap](#12-quick-recap)

---

## 1. What's new in Lab 7?

Lab 6 gave you a polymorphic `vector<Shape*>` containing circles, rectangles, and triangles side by side. That raises two immediate questions:

1. **"I know I stored a Circle in there — how do I safely get it back out as a Circle and call `getRadius()`?"** — that is what casting answers.
2. **"What happens if I create a shape with an invalid radius of -5?"** — that is what exceptions answer.

And a design question that has been lurking since Lab 6:

3. **"How do I write a function that can render *any* drawable thing, not just shapes?"** — that is what interfaces answer.

New concepts introduced in this lab:

- **C++ named casts** — `static_cast`, `dynamic_cast`, `const_cast`, `reinterpret_cast`.
- **Exception handling** — `throw`, `try`, `catch`, custom exception classes, stack unwinding.
- **Interfaces** — pure abstract classes with no data, used purely as capability contracts.

---

## 2. Why C-style casts are dangerous

Before C++, you cast types with C syntax: `(TargetType)expression`. This still works in C++, but it is silently dangerous on class hierarchies:

```cpp
Shape* s = new Rectangle(4.0, 6.0, "blue");
Circle* c = (Circle*)s;   // compiles without a warning
c->getRadius();            // undefined behaviour — reads garbage memory
```

The compiler cannot warn you because C-style casts bypass all type checking. The four C++ named casts replace this with explicit, auditable, and often compiler-checked operations. Each one does a specific job — their names make the *intent* clear and the *risk* visible.

---

## 3. Overview of C++ named casts

| Cast | Checked at | Primary use |
|---|---|---|
| `static_cast` | Compile time | Numeric conversions; safe upcasts; unchecked downcasts |
| `dynamic_cast` | Runtime (RTTI) | Safe, checked downcast in polymorphic hierarchies |
| `const_cast` | Compile time | Remove `const` to call legacy APIs |
| `reinterpret_cast` | Compile time | Raw bit reinterpretation; low-level / systems code |

---

## 4. `static_cast`

`static_cast` is resolved entirely at compile time. It performs conversions the compiler can verify as *potentially* meaningful, but it cannot guarantee correctness at runtime.

### 4.1 Numeric conversions

The most common use — converting between arithmetic types explicitly:

```cpp
double d = 3.99;
int    i = static_cast<int>(d);   // i = 3 — truncates, never rounds
```

This is preferable to an implicit conversion because it makes the truncation deliberate and visible. Without the cast, most compilers issue a warning; with it, the intent is clear.

A critical pattern — integer division:

```cpp
int x = 7, y = 2;
int    wrong  = x / y;                          // 3 — integer division
double correct = static_cast<double>(x) / y;    // 3.5 — one operand is double, so floating-point division is used
```

### 4.2 Upcasting (Derived\* → Base\*)

Converting a derived pointer to a base pointer is always safe and usually implicit. A `static_cast` can be used to make it explicit:

```cpp
Circle* c = new Circle(5.0, "red");
Shape*  s = static_cast<Shape*>(c);   // same as plain assignment
```

### 4.3 Downcasting — the danger

Converting a base pointer to a derived pointer compiles without error — but `static_cast` does **no runtime check**:

```cpp
// Safe — s really is a Circle
Circle* c2 = static_cast<Circle*>(s);
cout << c2->getRadius();   // fine

// WRONG — s is actually a Rectangle, not a Circle
Shape*  wrong = new Rectangle(4.0, 6.0, "blue");
Circle* bad   = static_cast<Circle*>(wrong);   // compiles, no warning
bad->getRadius();   // undefined behaviour — crash or garbage
```

`static_cast` trusts you. If you are wrong, the behaviour is undefined. When you are not certain what the object really is at runtime, use `dynamic_cast` instead.

**When to prefer `static_cast` over `dynamic_cast` for downcasting:** only when you have an external guarantee (e.g., you just stored the pointer and you know exactly what type it is), and you want to avoid the small runtime cost of RTTI.

---

## 5. `dynamic_cast`

`dynamic_cast` performs a **runtime type check** before doing the conversion. It requires the hierarchy to have at least one `virtual` function (which gives the object a vtable — the mechanism RTTI uses to determine the real type).

### 5.1 Pointer form

Cast a base pointer to a derived pointer. If the object is not of the target type, the result is `nullptr` — **always check before using it**:

```cpp
Shape* s = /* some Shape* from a collection */;

Circle* c = dynamic_cast<Circle*>(s);
if (c != nullptr) {
    cout << "It's a circle, radius = " << c->getRadius() << "\n";
} else {
    cout << "Not a circle.\n";
}
```

This is the standard pattern for extracting type-specific behaviour from a polymorphic collection when virtual dispatch is not enough.

### 5.2 Reference form

Cast a base reference to a derived reference. Because references cannot be `nullptr`, failure is reported by **throwing `std::bad_cast`**:

```cpp
try {
    Circle& c = dynamic_cast<Circle&>(*s);   // throws if s is not a Circle
    cout << "radius = " << c.getRadius() << "\n";
} catch (const bad_cast& e) {
    cout << "Cast failed: " << e.what() << "\n";
}
```

Use the reference form when you expect the cast to succeed and want failure to be an exception (not a silent `nullptr`).

### 5.3 How it works — RTTI

Every class with a virtual function gets a hidden **vtable** (a table of function pointers). Every object gets a hidden **vptr** (a pointer to its class's vtable). At runtime, `dynamic_cast` follows the vptr to determine the real type of the object and checks whether the requested conversion is valid.

This is called **Run-Time Type Information (RTTI)**. It has a small cost compared to `static_cast`, which is resolved entirely at compile time.

```
dynamic_cast<Circle*>(s):
  1. Follow s's vptr to its vtable.
  2. Check: is the real type Circle, or a class derived from Circle?
  3. If yes → return the adjusted pointer.
     If no  → return nullptr (pointer form) or throw bad_cast (reference form).
```

---

## 6. `const_cast`

`const_cast` adds or removes the `const` qualifier. Its only legitimate use in practice is calling a legacy API that forgot to mark its parameters `const`:

```cpp
// Old C-style function — written before const was used consistently
void legacyPrint(char* str) { cout << str; }

const char* msg = "hello";
// legacyPrint(msg);                     // error: cannot convert const char* → char*
legacyPrint(const_cast<char*>(msg));   // OK, as long as legacyPrint doesn't write through the pointer
```

**The rule:** `const_cast` is safe only if the underlying object is **not actually const** and you do not write through the resulting pointer. Modifying a truly `const` object through a `const_cast` pointer is **undefined behaviour**.

In well-designed modern code, `const_cast` is a red flag. If you find yourself reaching for it often, the design probably has a const-correctness issue.

---

## 7. `reinterpret_cast`

`reinterpret_cast` treats the raw bits at one address as if they were a completely different type. No conversion takes place — only a reinterpretation of the memory:

```cpp
unsigned int n = 0x41424344;
char* bytes = reinterpret_cast<char*>(&n);

for (size_t i = 0; i < sizeof(unsigned int); i++)
    cout << bytes[i];   // prints DCBA on a little-endian machine
```

There are no safety checks whatsoever. The result is platform-dependent and almost always implementation-defined. Legitimate uses exist in low-level systems programming (device drivers, network protocols, serialization buffers), but they are rare in OOP application code.

**If you find yourself writing `reinterpret_cast` in a class hierarchy, the design probably needs rethinking.**

---

## 8. Exception Handling

Up until now, errors were handled with early returns and `cout` messages inside methods. Exceptions offer a structured alternative: an error is *thrown* at the point it is detected and *caught* wherever the calling code is prepared to handle it, even if that is many call frames above.

### 8.1 `throw`, `try`, `catch`

```cpp
void riskyFunction() {
    throw runtime_error("something went wrong");
}

int main() {
    try {
        riskyFunction();
        cout << "This line is never reached.\n";
    } catch (const runtime_error& e) {
        cout << "Caught: " << e.what() << "\n";
    }
}
```

- `throw` terminates the current function and begins propagating the exception up the call stack.
- `try` marks a block of code to watch for exceptions.
- `catch` specifies what type of exception to handle and what to do with it.

If an exception propagates all the way out of `main()` without being caught, the program calls `std::terminate()` and aborts.

### 8.2 The standard exception hierarchy

```
std::exception
 ├── std::logic_error
 │    ├── std::invalid_argument
 │    ├── std::out_of_range
 │    └── ...
 └── std::runtime_error
      ├── std::range_error
      ├── std::overflow_error
      └── ...  (and std::bad_cast, std::bad_alloc, etc.)
```

Always catch by **`const` reference** (`const std::exception& e`) — catching by value would slice the object (the derived part is lost), and catching by pointer requires the thrower to allocate on the heap.

### 8.3 Custom exception classes

Derive from `std::runtime_error` (or another standard class) to get the `what()` message infrastructure for free:

```cpp
class ShapeException : public runtime_error {
public:
    explicit ShapeException(const string& msg) : runtime_error(msg) {}
};

class InvalidDimensionException : public ShapeException {
    double badValue;
public:
    InvalidDimensionException(const string& field, double val)
        : ShapeException("Invalid dimension for '" + field + "': " + to_string(val)),
          badValue(val) {}

    double getBadValue() const { return badValue; }
};
```

`InvalidDimensionException` inherits from `ShapeException`, which inherits from `runtime_error`. This means you can catch it at three different levels of specificity depending on how much detail the caller needs.

Notice that it carries extra data (`badValue`) beyond what the message string provides — the caller can inspect this programmatically.

### 8.4 Order of catch blocks

Catch blocks are tried **in order**. Because a derived exception matches a base catch, the derived catch must always come first:

```cpp
try {
    throw InvalidDimensionException("radius", -5.0);
} catch (const InvalidDimensionException& e) {
    // matched first — most specific
    cout << "Bad value: " << e.getBadValue() << "\n";
} catch (const ShapeException& e) {
    // never reached — already caught above
} catch (const exception& e) {
    // never reached — already caught above
}
```

Placing the base catch first would swallow all exceptions of that base type and the derived catch would become unreachable — many compilers warn about this.

### 8.5 Throwing from a constructor

A constructor that throws signals that the object could not be created. **The object does not exist** — no destructor will be called for it, and no caller will ever receive a reference to it:

```cpp
Circle::Circle(double radius, const string& color) : Shape(color) {
    if (radius <= 0)
        throw InvalidDimensionException("radius", radius);
    this->radius = radius;
}
```

```cpp
try {
    Circle bad(-5.0, "red");    // throws InvalidDimensionException
    bad.getRadius();             // never reached — bad was never constructed
} catch (const InvalidDimensionException& e) {
    cout << e.what() << "\n";
}
```

This is the correct pattern for enforcing invariants. A `Circle` with a non-positive radius is not a valid `Circle` — the constructor should refuse to produce one.

### 8.6 Stack unwinding

When an exception is thrown, the runtime **unwinds the call stack** — it walks back up through all active stack frames, destroying local variables in reverse order of construction (calling their destructors). This is the mechanism that makes exception handling compatible with RAII (Resource Acquisition Is Initialization):

```cpp
try {
    Guard g("Guard");   // constructed
    throw ShapeException("error mid-function");
    // Guard::~Guard() is called HERE, during unwinding, before catch runs
} catch (const ShapeException& e) {
    cout << "Caught: " << e.what() << "\n";
}
```

Output:
```
Guard constructed
Guard destroyed  ← destructor called by unwinding
Caught: error mid-function
```

This guarantee is why destructors that release resources (memory, file handles, locks) are so important — they run even during exceptions, preventing leaks.

---

## 9. Interfaces

### 9.1 Interface vs. abstract base class

In Lab 6, `Shape` was an **abstract base class** — it had data (`id`, `color`) and some of its methods were pure virtual. A derived class inherits both the data and the contract.

An **interface** is a pure abstract class with **no data members at all**. It expresses a *capability*, not an *identity*:

```cpp
class Drawable {
public:
    virtual void draw() const = 0;
    virtual ~Drawable() = default;   // always virtual in a class meant to be inherited
};

class Serializable {
public:
    virtual string serialize() const = 0;
    virtual ~Serializable() = default;
};
```

`Drawable` says: "anything that implements `draw()` can be used wherever a `Drawable` is expected."
`Serializable` says: "anything that implements `serialize()` can be used wherever a `Serializable` is expected."

Neither interface says anything about what the implementing class *is* — only what it *can do*.

### 9.2 Satisfying multiple interfaces

A class can inherit from multiple interfaces simultaneously. Because interfaces carry no data, there is no diamond problem or ambiguity:

```cpp
class Circle : public Shape,      // identity — a Circle IS a Shape
               public Drawable,   // capability — a Circle CAN BE drawn
               public Serializable {  // capability — a Circle CAN BE serialized
    // ...
    void draw()        const override { ... }
    string serialize() const override { ... }
};
```

The same `Circle` object can now be used wherever a `Shape*`, a `Drawable*`, or a `Serializable*` is expected.

### 9.3 Programming to an interface

The payoff is that you can write functions that accept *any* object satisfying a contract — without knowing anything about the concrete class:

```cpp
void renderAll(const vector<Drawable*>& items) {
    for (const Drawable* d : items)
        d->draw();   // virtual dispatch — calls the correct draw() for each type
}

void saveAll(const vector<Serializable*>& items) {
    for (const Serializable* s : items)
        cout << s->serialize() << "\n";
}
```

`renderAll` knows nothing about `Shape`, `Circle`, or `Rectangle`. It only knows the `Drawable` contract. If you add a completely new class — `Button`, `Sprite`, `Map` — that also implements `Drawable`, you can pass it to `renderAll` without modifying the function at all.

This is the **open/closed principle**: the function is *open* for extension (new types can be added) but *closed* for modification (the function itself does not change).

One object seen through three lenses:

```cpp
Circle c(7.0, "purple");

Shape*        asShape      = &c;
Drawable*     asDrawable   = &c;
Serializable* asSerializable = &c;

cout    << *asShape;           // uses Shape interface
asDrawable->draw();            // uses Drawable interface
asSerializable->serialize();   // uses Serializable interface
```

All three pointers point to the same object in memory. The interface pointer simply restricts what you can call through it.

---

## 10. Lab Code Walkthrough

### Class hierarchy

```
std::exception
 └── std::runtime_error
      └── ShapeException
           └── InvalidDimensionException

Drawable        (interface — no data)
Serializable    (interface — no data)

Shape           (abstract base — id, color; area/perimeter/typeName pure virtual)
 ├── Circle     : Shape, Drawable, Serializable  (radius)
 ├── Rectangle  : Shape, Drawable, Serializable  (width, height)
 └── Triangle   : Shape, Drawable, Serializable  (a, b, c)
```

---

### demoStaticCast

Shows numeric truncation, integer division pitfall, safe upcast, correct downcast, and the danger of a wrong downcast (compiles silently, undefined behaviour).

---

### demoDynamicCast

Iterates a mixed `vector<Shape*>`. For each element, tries `dynamic_cast<Circle*>` and `dynamic_cast<Rectangle*>` and prints type-specific information. Then demonstrates the reference form catching `std::bad_cast`.

---

### demoConstCast

Calls a lambda simulating a legacy C API that takes `char*` instead of `const char*`. Shows that the cast is only safe because the function does not actually write through the pointer.

---

### demoReinterpretCast

Reinterprets the bytes of an `unsigned int` as a `char*` to demonstrate endianness. Emphasises that this cast is not for OOP code.

---

### demoExceptions

Four mini-demos:
1. Throw from constructor — object is never created.
2. Catch via base class type (`ShapeException` catches `InvalidDimensionException`).
3. Catch blocks in order — derived before base.
4. Stack unwinding — `Guard`'s destructor runs before the catch block.

---

### demoInterfaces

Creates one object of each concrete type on the stack. Passes them to `renderAll` as `Drawable*` and to `saveAll` as `Serializable*`. Then shows the same `Circle` object accessed through three different pointer types simultaneously.

---

### demoPolymorphicCollection

A full end-to-end demo: builds a mixed collection, iterates it with virtual dispatch, uses `dynamic_cast` to extract circle radii, and uses `dynamic_cast` to the `Drawable` interface to render all elements.

---

## 11. Common Mistakes

| Mistake | Description | Fix |
|---|---|---|
| Using C-style casts on pointers | No type checking — silent undefined behaviour | Use `dynamic_cast` when the type is uncertain at runtime |
| Forgetting to check `dynamic_cast` result | `nullptr` dereference if the cast fails | Always `if (ptr != nullptr)` before using a dynamic_cast pointer result |
| `static_cast` downcast when wrong type | Compiles, but undefined behaviour at runtime | Use `dynamic_cast` if you are not certain of the type |
| `const_cast` then writing through the pointer | Undefined behaviour if the original was `const` | Only remove `const` when you will not modify the data |
| Catching by value instead of by reference | Object slicing — derived part is lost | Always `catch (const ExceptionType& e)` |
| Catch base before derived | The base catch swallows derived exceptions | Put the most specific `catch` first |
| Throwing from a destructor | Can call `std::terminate` during stack unwinding | Never throw from a destructor |
| Interface without virtual destructor | `delete drawable_ptr` skips derived destructor | Always declare `virtual ~Interface() = default` |
| Forgetting `override` on interface methods | Typo creates a new unrelated method silently | Always write `override` on every implemented method |

---

## 12. Quick Recap

| Concept | What it is | Where it appears in code |
|---|---|---|
| **`static_cast`** | Compile-time cast; trusts the programmer | Numeric conversions, upcasts, known-safe downcasts |
| **`dynamic_cast`** | Runtime-checked cast via RTTI | Safe downcast in polymorphic hierarchy |
| **`const_cast`** | Removes/adds `const` qualifier | Calling legacy APIs |
| **`reinterpret_cast`** | Raw bit reinterpretation | Low-level byte inspection; avoid in OOP code |
| **`throw`** | Signals an error by launching an exception | Inside constructors and methods when invariants are violated |
| **`try` / `catch`** | Marks code to watch and handles exceptions | Wherever the program can meaningfully recover |
| **Exception hierarchy** | Deriving exceptions from `std::runtime_error` | `ShapeException` → `InvalidDimensionException` |
| **Stack unwinding** | Destructors run as the stack is unwound | Guaranteed cleanup even when an exception is thrown |
| **Catch order** | Specific before general | `InvalidDimensionException` before `ShapeException` before `exception` |
| **Interface** | Pure abstract class with no data — a capability contract | `Drawable`, `Serializable` |
| **Multiple interfaces** | A class satisfies several contracts at once | `Circle : Shape, Drawable, Serializable` |
| **Program to an interface** | Functions accept interface pointers, not concrete types | `renderAll(vector<Drawable*>)`, `saveAll(vector<Serializable*>)` |
| **`dynamic_cast` to interface** | Extract an interface from a base pointer at runtime | `dynamic_cast<Drawable*>(shape_ptr)` |
