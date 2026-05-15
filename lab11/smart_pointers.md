# Lab 11 — Smart Pointers

> **Course:** Object-Oriented Programming
> **Topics:** RAII · The raw `new`/`delete` problem · `std::unique_ptr` · `std::shared_ptr` · `std::weak_ptr` · `make_unique` / `make_shared` · Ownership conventions · Custom deleters

---

## Table of Contents

1. [What's new in Lab 11?](#1-whats-new-in-lab-11)
2. [The problem with raw pointers](#2-the-problem-with-raw-pointers)
3. [RAII — Resource Acquisition Is Initialization](#3-raii--resource-acquisition-is-initialization)
4. [`unique_ptr` — single owner](#4-unique_ptr--single-owner)
5. [`make_unique` — the preferred constructor](#5-make_unique--the-preferred-constructor)
6. [Smart pointers and polymorphism](#6-smart-pointers-and-polymorphism)
7. [`shared_ptr` — shared ownership](#7-shared_ptr--shared-ownership)
8. [`weak_ptr` — looking without owning](#8-weak_ptr--looking-without-owning)
9. [Cycles and how `weak_ptr` breaks them](#9-cycles-and-how-weak_ptr-breaks-them)
10. [Passing smart pointers to functions](#10-passing-smart-pointers-to-functions)
11. [Exception safety](#11-exception-safety)
12. [Custom deleters](#12-custom-deleters)
13. [Common Mistakes](#13-common-mistakes)
14. [Exercises](#14-exercises)
15. [Quick Recap](#15-quick-recap)

---

## 1. What's new in Lab 11?

Back in Lab 6 we wrote `vector<Shape*>` and had to remember to `delete` every element by hand. In Lab 7 we threw exceptions from constructors — and very quietly, every `new` we ever wrote became fragile, because an exception thrown halfway through a function would simply leak whatever you allocated before it.

The whole story of Lab 11 starts with a question:

> *"Who is responsible for deleting this object, and what happens if they forget?"*

A **smart pointer** is an object that owns a pointer and deletes it for you when the smart pointer itself is destroyed. You never write `delete`. You never leak. You never double-delete. The compiler enforces all of this through the smart pointer's destructor.

New concepts introduced in this lab:

- **RAII** — the C++ principle that ties resource lifetime to object lifetime.
- **`std::unique_ptr<T>`** — a pointer that has exactly one owner.
- **`std::shared_ptr<T>`** — a pointer with a built-in reference count: shared ownership.
- **`std::weak_ptr<T>`** — a non-owning observer of a `shared_ptr`.
- **`make_unique` / `make_shared`** — the preferred factory functions.
- **Ownership conventions** — how to pass these things to functions.
- **Custom deleters** — for resources that aren't `delete`-d.

All examples live in `smart_pointers.cpp` — read it side-by-side with this document.

---

## 2. The problem with raw pointers

In Lab 6 every shape was created with `new` and destroyed with `delete`. That pattern looks fine — until the function doing the work has more than one possible exit:

```cpp
void leakyFunction(bool fail) {
    Shape* s = new Circle(3.0, "leaky");

    if (fail) {
        return;   // <- forgot to delete s. Memory leaked.
    }

    s->draw();
    delete s;
}
```

The compiler never warns you. The program never crashes. The leak just accumulates silently every time you call `leakyFunction(true)`. Now multiply that by Lab 7: **any** function that allocates with `new` and then throws an exception before reaching its `delete` will leak. So this pattern is unsafe almost by default.

You could try to fix it with `try`/`catch` everywhere, like this:

```cpp
Shape* s = new Circle(3.0, "leaky");
try {
    // ... lots of code that might throw ...
    delete s;
} catch (...) {
    delete s;
    throw;
}
```

That works, but it's tedious, error-prone, and exactly the kind of repetition C++ is supposed to spare you. The real answer is: **let an object own the pointer, and let that object's destructor do the cleanup.**

---

## 3. RAII — Resource Acquisition Is Initialization

RAII is the C++ idiom that makes everything in this lab work:

> **A resource is acquired when an object is constructed, and released when that object is destroyed.**

You already know one example: `std::string`. When a `string` is constructed it allocates whatever memory it needs; when it goes out of scope, its destructor frees that memory. You never call `free` on a `string`. It just works.

Smart pointers apply that same pattern to *any* heap-allocated object you might create with `new`. The smart pointer is the **owner**. When the owner dies, the owned object dies with it. Stack unwinding (Lab 7, §8.6) handles this even when an exception is thrown.

This single idea replaces almost every manual `delete` you would have written in Labs 6, 7, and 8.

---

## 4. `unique_ptr` — single owner

A `std::unique_ptr<T>` is a pointer that owns exactly one object. When the `unique_ptr` is destroyed, it deletes the object.

```cpp
#include <memory>

{
    std::unique_ptr<Shape> s(new Circle(5.0, "uniq"));
    s->draw();          // works just like a raw pointer
}   // <- s goes out of scope here. The Circle is deleted automatically.
```

You use it almost exactly like a raw pointer: `s->method()` and `*s` work as expected. The only big difference is the ownership rule.

### `unique_ptr` is non-copyable

There can be only one owner. The following **does not compile**:

```cpp
std::unique_ptr<Shape> a(new Circle(1.0, "a"));
std::unique_ptr<Shape> b = a;   // ERROR: unique_ptr cannot be copied
```

If copying were allowed, both `a` and `b` would think they own the same Circle. When they both went out of scope, the Circle would be deleted twice — undefined behaviour.

### Transferring ownership with `std::move`

You can give ownership away, just not copy it. That's what `std::move` is for:

```cpp
std::unique_ptr<Shape> a(new Circle(1.0, "a"));
std::unique_ptr<Shape> b = std::move(a);   // ownership moves a → b
// Now a is empty (nullptr), b owns the Circle.
```

After the move, `a` is a perfectly valid `unique_ptr` — it's just empty. You can put a new object into it, or let it die quietly.

---

## 5. `make_unique` — the preferred constructor

Writing `unique_ptr<T>(new T(args...))` is verbose and contains the word `new`, which we are trying to eliminate. The standard library provides `make_unique`:

```cpp
auto c = std::make_unique<Circle>(2.5, "c");
//   |                   |        |
//   |                   |        constructor arguments for Circle
//   |                   the type of object to create
//   automatic type — c is std::unique_ptr<Circle>
```

`make_unique<T>(args...)` constructs a `T` with those arguments and wraps it in a `unique_ptr`. Prefer it for three reasons:

1. **No `new` keyword** anywhere in your code — that alone removes a class of bugs.
2. **Exception-safe** when used in function calls (a subtle but real win).
3. **Shorter** — `auto c = make_unique<Circle>(2.5, "c");` reads cleanly.

There is also `std::make_shared` for `shared_ptr`. Use these whenever you can.

> **Note:** `make_unique` requires C++14. `make_shared` works since C++11.

---

## 6. Smart pointers and polymorphism

Remember Lab 6's polymorphic collection?

```cpp
std::vector<Shape*> shapes;
shapes.push_back(new Circle(1.0, "c1"));
shapes.push_back(new Rectangle(2.0, 3.0, "r1"));
// ... use them ...
for (auto* s : shapes) delete s;   // <- manual cleanup
```

The cleanup loop at the end is exactly what smart pointers exist to remove:

```cpp
std::vector<std::unique_ptr<Shape>> shapes;
shapes.push_back(std::make_unique<Circle>(1.0, "c1"));
shapes.push_back(std::make_unique<Rectangle>(2.0, 3.0, "r1"));

for (const auto& s : shapes)
    s->draw();
// No cleanup loop. When `shapes` goes out of scope it destroys each
// unique_ptr, which deletes the Shape it owns.
```

For this to work safely, `Shape` **must have a virtual destructor** — exactly what Lab 6 §9 already required. The smart pointer calls `delete` on the stored pointer, and `delete` of a base pointer needs the virtual destructor to find the right derived destructor. Nothing new there; same rule, different syntax for the cleanup.

---

## 7. `shared_ptr` — shared ownership

Sometimes one object is owned in many places at once. Example: a texture loaded into memory and referenced by every sprite that uses it. None of the sprites *uniquely* owns the texture — but it should stay alive as long as at least one sprite needs it.

`std::shared_ptr<T>` keeps a **reference count**. Every copy of the shared_ptr increments the count; every destruction decrements it. The object dies exactly when the count reaches zero.

```cpp
std::shared_ptr<Shape> a = std::make_shared<Circle>(7.0, "shared");
//                                                       count = 1

{
    std::shared_ptr<Shape> b = a;        //                count = 2
    b->draw();
}   // b leaves scope                                  //  count = 1

// a leaves scope when this function returns          //  count = 0 → Circle destroyed
```

You can ask the current count with `.use_count()`:

```cpp
std::cout << a.use_count();   // prints the number of shared_ptrs that own this object
```

### When to choose `shared_ptr` over `unique_ptr`

| Question | If yes → use… |
|---|---|
| Is there exactly one owner? | `unique_ptr` |
| Could the object live in several independent places at once? | `shared_ptr` |
| Do you need to copy the pointer around freely? | `shared_ptr` |

`shared_ptr` is more flexible but also more expensive (reference counting isn't free). **Default to `unique_ptr`. Reach for `shared_ptr` only when you actually need shared ownership.**

---

## 8. `weak_ptr` — looking without owning

Suppose you want to remember *that* an object exists without keeping it alive. A `std::weak_ptr<T>` is a non-owning observer of a `shared_ptr<T>`: it doesn't bump the reference count, and it can tell you whether the object is still alive.

To use the object through a `weak_ptr`, call `.lock()`. That returns a fresh `shared_ptr` which is either:

- **valid**, if the object is still alive (and bumps the count for as long as you hold it), or
- **empty (`nullptr`)**, if the last owner has already let go.

```cpp
std::weak_ptr<Shape> observer;

{
    std::shared_ptr<Shape> owner = std::make_shared<Circle>(9.9, "demo");
    observer = owner;   // observer now watches the same object

    if (auto locked = observer.lock()) {
        locked->draw();   // safe — object is alive
    }
}   // owner dies here, Circle is destroyed

if (auto locked = observer.lock()) {
    // never reached
} else {
    std::cout << "Object is gone.\n";
}
```

Typical uses:
- **Caches** that should not keep their entries alive.
- **Observer / parent-back-pointer** patterns where you want to refer to something without owning it.
- **Breaking cycles** (next section).

---

## 9. Cycles and how `weak_ptr` breaks them

If two `shared_ptr`s point at each other, **the reference count never reaches zero**:

```
   ┌──── shared_ptr ────┐
   │                    ▼
  Node A              Node B
   ▲                    │
   └──── shared_ptr ────┘
```

Even when no external code holds either node any more, A keeps B alive and B keeps A alive. Classic memory leak.

Solution: pick one of the two links and make it a `weak_ptr` instead:

```cpp
struct Node {
    std::shared_ptr<Node> next;
    std::weak_ptr<Node>   prev;   // <- weak, breaks the cycle
};
```

Now the strong (shared) link points only one way. When the last external `shared_ptr` is destroyed, the strong chain unravels and both nodes are freed.

**Rule of thumb:** in any "doubly linked" structure (parent/child, prev/next, owner/observer), make one direction strong (`shared_ptr`) and the other weak (`weak_ptr`).

---

## 10. Passing smart pointers to functions

There are three different intentions when passing a pointer to a function, and three different ways to express them:

| Intention | Signature | What it says |
|---|---|---|
| I will **take ownership** | `void f(std::unique_ptr<T> p)` | Caller must transfer with `std::move`. After the call, the caller no longer owns the object. |
| I just want to **look at it** | `void f(const T& obj)` or `void f(const T* obj)` | No ownership changes hands. Pass `*ptr` or `ptr.get()`. |
| I want to **share ownership** | `void f(std::shared_ptr<T> p)` | The function bumps the reference count for the duration of the call. |

```cpp
auto s = std::make_unique<Circle>(3.3, "x");

inspectShape(*s);                 // just look — caller still owns
consumeShape(std::move(s));       // give it away — caller's pointer is now empty
```

The most common mistake is taking a `unique_ptr` parameter when you only want to read the object. That forces every caller to give up ownership — usually not what you want. **If you don't intend to keep the object, take a reference instead.**

---

## 11. Exception safety

This is where smart pointers truly shine. Lab 7 explained that stack unwinding runs destructors for local variables as exceptions propagate. Since a smart pointer is a local variable, **its destructor — and therefore the `delete` of its object — runs even when an exception is thrown**:

```cpp
void mightThrow(bool doThrow) {
    auto safe = std::make_unique<Circle>(1.1, "safe");

    if (doThrow)
        throw std::runtime_error("boom!");

    safe->draw();
}
```

If `doThrow` is true, the exception propagates out of `mightThrow`. On its way out, `safe`'s destructor runs and deletes the Circle. **No leak, no try/catch needed.**

Compare to the raw-pointer version, which would require explicit cleanup in every catch handler. RAII + smart pointers eliminates that boilerplate entirely.

---

## 12. Custom deleters

Not every resource is released with `delete`. C-style file handles use `fclose`, sockets use `close`, mutexes use `pthread_mutex_unlock`, and so on. Both `unique_ptr` and `shared_ptr` let you supply a custom deleter — a function or lambda that does the right thing for *your* resource:

```cpp
auto release = [](int* h) {
    std::cout << "releasing handle\n";
    delete h;
};

{
    std::unique_ptr<int, decltype(release)> handle(new int(42), release);
    std::cout << *handle << "\n";
}   // <- release() runs here, instead of plain delete
```

This is how libraries like cURL, OpenSSL, and SQLite are typically wrapped in modern C++: a `unique_ptr` whose deleter is the library's release function. You get all the RAII benefits with no extra wrapper class needed.

---

## 13. Common Mistakes

| Mistake | Why it's bad | Fix |
|---|---|---|
| Mixing `new` with `unique_ptr` and then `delete` manually | Double-delete — undefined behaviour | Let the `unique_ptr` do the deleting |
| Copying a `unique_ptr` (`auto b = a;`) | Doesn't compile — but trying to is a sign you wanted `shared_ptr` | Use `std::move` to transfer, or use `shared_ptr` if you need copies |
| Forgetting `virtual` on the base destructor | Deleting through `unique_ptr<Base>` skips the derived destructor | Always `virtual ~Base() = default;` in a polymorphic base |
| Two `shared_ptr`s pointing at each other | Reference cycle → memory leak | Break the cycle with a `weak_ptr` |
| Using `shared_ptr` for everything by default | Reference counting has a runtime cost; unclear ownership | Default to `unique_ptr`. Pick `shared_ptr` only if you really share |
| Taking `unique_ptr<T>` as a parameter when you only read the object | Caller is forced to give up ownership | Take `const T&` instead |
| Constructing two `shared_ptr`s from the same raw pointer | Each gets its own ref count → double-delete | Construct once with `make_shared`, then copy that |
| Dereferencing a `weak_ptr` without `.lock()` | A `weak_ptr` cannot dereference directly | Always `auto p = w.lock(); if (p) { ... }` |

---

## 14. Exercises

Work through these in order — each one builds on the previous. Open `smart_pointers.cpp` and add your solutions there, or start a fresh file.

### Exercise 1 — Plug a leak ⭐

The following function leaks on every call where `negative` is true:

```cpp
double area(double radius, bool negative) {
    Circle* c = new Circle(radius, "x");
    if (negative && radius < 0) return -1;   // leak: `c` is never deleted
    double a = c->area();
    delete c;
    return a;
}
```

Rewrite it using `unique_ptr` and `make_unique` so that no `delete` appears anywhere in the function body. The early return must still leak nothing.

### Exercise 2 — Polymorphic collection, smart-pointer style ⭐

Take the polymorphic collection from Lab 6:

```cpp
std::vector<Shape*> shapes;
shapes.push_back(new Circle(1.0, "c1"));
shapes.push_back(new Rectangle(2.0, 3.0, "r1"));
for (Shape* s : shapes) s->draw();
for (Shape* s : shapes) delete s;
```

Rewrite it using `std::vector<std::unique_ptr<Shape>>`. The cleanup loop should disappear entirely. Verify with a debug print in `~Shape()` that every shape is destroyed exactly once.

### Exercise 3 — Parent / child tree ⭐⭐

Implement a tiny tree node:

```cpp
struct TreeNode {
    std::string name;
    std::vector<std::shared_ptr<TreeNode>> children;
    std::weak_ptr<TreeNode> parent;
};
```

Build a small tree (root → A, B; A → A1, A2). Then write a function `pathToRoot(node)` that returns a vector of names from the given node up to the root, by walking `parent.lock()` at each step. Why must `parent` be a `weak_ptr` and not a `shared_ptr`? (Hint: re-read §9.)

### Exercise 4 — RAII wrapper for a C handle ⭐⭐

Wrap `std::FILE*` in a `unique_ptr` with `std::fclose` as a custom deleter, so you never have to call `fclose` by hand:

```cpp
auto file = /* your code */;
std::fprintf(file.get(), "hello\n");
// when `file` goes out of scope, fclose runs automatically
```

The trick is getting the type right: `unique_ptr<FILE, decltype(&std::fclose)>`. Write a helper function `openFile(path)` that returns this type.

### Exercise 5 — Find the bug ⭐⭐⭐

The program below compiles and runs, but the destructors never fire. Fix it with a single keyword change:

```cpp
struct Person {
    std::string name;
    std::shared_ptr<Person> bestFriend;
    ~Person() { std::cout << name << " destroyed\n"; }
};

int main() {
    auto a = std::make_shared<Person>(Person{"Alice", {}});
    auto b = std::make_shared<Person>(Person{"Bob",   {}});
    a->bestFriend = b;
    b->bestFriend = a;
}   // nothing prints — why?
```

Explain *why* the destructors don't run, and what the minimal fix is.

---

## 15. Quick Recap

| Concept | What it is | When to use |
|---|---|---|
| **RAII** | Resource lifetime = object lifetime | Always — the foundation of modern C++ |
| **`unique_ptr<T>`** | Single-owner smart pointer | Default choice; anywhere you used to write `new`/`delete` |
| **`make_unique<T>`** | Factory function for `unique_ptr` | Always — never write `new` in modern code |
| **`shared_ptr<T>`** | Reference-counted smart pointer | When the same object must be owned in multiple places |
| **`make_shared<T>`** | Factory function for `shared_ptr` | When you actually need `shared_ptr` |
| **`weak_ptr<T>`** | Non-owning observer of a `shared_ptr` | Caches, observers, breaking cycles |
| **`std::move`** | Transfers ownership | When passing a `unique_ptr` to something that will keep it |
| **Custom deleter** | A function called instead of `delete` | Wrapping C APIs in RAII |
| **Virtual destructor** | Required on a polymorphic base | Same rule as Lab 6 — still applies with smart pointers |
