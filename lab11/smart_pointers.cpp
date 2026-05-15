/*
 * Lab 11 — Smart Pointers
 *
 * Concepts:
 *   - The problem with raw new/delete
 *   - RAII (Resource Acquisition Is Initialization)
 *   - std::unique_ptr  : single owner, automatic cleanup
 *   - std::shared_ptr  : shared ownership via reference counting
 *   - std::weak_ptr    : non-owning observer of a shared_ptr
 *   - make_unique / make_shared
 *   - Smart pointers with polymorphism
 *   - Custom deleters
 */

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>

using namespace std;

// ============================================================
//  Reusing the Shape hierarchy from earlier labs.
//  Each constructor announces itself, and each destructor announces
//  itself too — that way we can SEE when objects are destroyed.
// ============================================================

class Shape {
protected:
    string name;
public:
    Shape(const string& name) : name(name) {
        cout << "    [+] Shape " << name << " constructed\n";
    }
    virtual ~Shape() {
        cout << "    [-] Shape " << name << " destroyed\n";
    }
    virtual void draw() const = 0;
    string getName() const { return name; }
};

class Circle : public Shape {
    double radius;
public:
    Circle(double r, const string& name) : Shape(name), radius(r) {}
    void draw() const override {
        cout << "    drawing circle '" << name << "' (radius=" << radius << ")\n";
    }
};

class Rectangle : public Shape {
    double w, h;
public:
    Rectangle(double w, double h, const string& name) : Shape(name), w(w), h(h) {}
    void draw() const override {
        cout << "    drawing rectangle '" << name << "' (" << w << "x" << h << ")\n";
    }
};

// Helper to print section headers
void separator(const string& title) {
    cout << "\n══════════════════════════════════════════\n";
    cout << "  " << title << "\n";
    cout << "══════════════════════════════════════════\n";
}

// ============================================================
//  1. THE PROBLEM — raw new/delete is fragile
//
//  Every `new` requires a matching `delete` on every possible exit
//  path. As soon as a function has more than one return, or it can
//  throw, keeping track of who must `delete` what becomes painful.
// ============================================================

void leakyFunction(bool fail) {
    Shape* s = new Circle(3.0, "leaky");

    if (fail) {
        // Oops — early return forgot to delete s.
        // Memory is leaked. The compiler doesn't warn.
        return;
    }

    s->draw();
    delete s;   // only reached on the happy path
}

void demoRawPointerProblem() {
    separator("1. The problem: raw new/delete leaks easily");

    cout << "  Calling leakyFunction(false) — happy path:\n";
    leakyFunction(false);

    cout << "\n  Calling leakyFunction(true) — early return:\n";
    leakyFunction(true);
    cout << "  Notice: Shape 'leaky' was never destroyed. That is a memory leak.\n";
}

// ============================================================
//  2. unique_ptr — single owner, automatic cleanup
//
//  A unique_ptr owns exactly one object. When the unique_ptr goes
//  out of scope, it deletes the object for you. No more `delete`,
//  no more leaks.
//
//  You use it almost like a raw pointer: operator* and operator->
//  give you access to the object.
// ============================================================

void demoUniquePtr() {
    separator("2. unique_ptr — single owner");

    {
        cout << "  Entering inner scope...\n";
        unique_ptr<Shape> s(new Circle(5.0, "uniq"));
        s->draw();
        cout << "  Leaving inner scope — unique_ptr will delete the Circle automatically.\n";
    }
    cout << "  Circle is already gone. No leak, no manual delete.\n";

    // unique_ptr is NON-COPYABLE — only one owner is allowed.
    // The following line would not compile:
    //
    //     unique_ptr<Shape> a(new Circle(1.0, "a"));
    //     unique_ptr<Shape> b = a;   // ERROR — copy is disabled
    //
    // But you CAN transfer ownership with std::move:
    cout << "\n  Transferring ownership with std::move:\n";
    unique_ptr<Shape> a(new Circle(1.0, "a"));
    unique_ptr<Shape> b = std::move(a);   // ownership moves from a to b
    cout << "  a is now " << (a ? "non-null" : "null") << "\n";
    cout << "  b is now " << (b ? "non-null" : "null") << "\n";
    b->draw();
    // When b leaves the function, the Circle gets destroyed.
}

// ============================================================
//  3. make_unique — the preferred way to construct a unique_ptr
//
//  make_unique<T>(args...) creates an object of type T with those
//  arguments and wraps it in a unique_ptr. Prefer it over `new`:
//    - shorter to write
//    - exception-safe (no chance of leaking a half-built object)
//    - no `new` keyword in your code at all
// ============================================================

void demoMakeUnique() {
    separator("3. make_unique — the preferred way");

    auto c = make_unique<Circle>(2.5, "made_with_make_unique");
    c->draw();
    cout << "  No `new`, no `delete`, no leaks.\n";
}

// ============================================================
//  4. unique_ptr with polymorphism — replacing vector<Shape*>
//
//  In lab 6 we wrote `vector<Shape*>` and had to delete every
//  element by hand. With unique_ptr, the vector cleans up after
//  itself when it goes out of scope.
// ============================================================

void demoPolymorphicUniquePtr() {
    separator("4. unique_ptr with polymorphism — clean polymorphic collections");

    vector<unique_ptr<Shape>> shapes;
    shapes.push_back(make_unique<Circle>(1.0, "c1"));
    shapes.push_back(make_unique<Rectangle>(2.0, 3.0, "r1"));
    shapes.push_back(make_unique<Circle>(4.0, "c2"));

    cout << "  Drawing all shapes:\n";
    for (const auto& s : shapes)
        s->draw();

    cout << "  Leaving function — destructors will run automatically, in reverse order:\n";
    // No `for (auto* p : shapes) delete p;` needed.
}

// ============================================================
//  5. shared_ptr — shared ownership via reference counting
//
//  Sometimes one object needs to be owned by several places at
//  once: a Circle could be drawn on two scenes that share its
//  data. A unique_ptr cannot do this (only one owner allowed).
//
//  shared_ptr keeps a reference count. The object is destroyed
//  exactly when the LAST shared_ptr to it goes away.
// ============================================================

void demoSharedPtr() {
    separator("5. shared_ptr — shared ownership");

    shared_ptr<Shape> a = make_shared<Circle>(7.0, "shared_circle");
    cout << "  After creating a: use_count = " << a.use_count() << "\n";

    {
        shared_ptr<Shape> b = a;   // copy — both point at the same object
        cout << "  Inside inner scope, after b = a: use_count = " << a.use_count() << "\n";
        b->draw();
    }   // b goes out of scope — count drops back to 1, object NOT destroyed yet

    cout << "  Back in outer scope, b is gone: use_count = " << a.use_count() << "\n";
    cout << "  Leaving function — last shared_ptr dies, object gets destroyed:\n";
}

// ============================================================
//  6. weak_ptr — non-owning observer
//
//  Sometimes you want to remember an object without keeping it
//  alive. A weak_ptr looks at a shared_ptr but does NOT bump the
//  reference count. To use the object, you call .lock() — that
//  gives you a shared_ptr that is either valid (if the object is
//  still alive) or empty (if it has already been destroyed).
//
//  Two common uses:
//   - breaking circular references between shared_ptrs (otherwise
//     the count never reaches zero and the objects leak)
//   - caches and observers that should not keep objects alive
// ============================================================

void demoWeakPtr() {
    separator("6. weak_ptr — looks but doesn't own");

    weak_ptr<Shape> observer;

    {
        shared_ptr<Shape> owner = make_shared<Circle>(9.9, "weak_demo");
        observer = owner;   // weak_ptr now watches the same object

        if (auto locked = observer.lock()) {
            cout << "  Object is still alive. Drawing it:\n";
            locked->draw();
        }
    }   // shared_ptr `owner` dies here — object is destroyed

    if (auto locked = observer.lock()) {
        cout << "  (never printed)\n";
    } else {
        cout << "  After the owner is gone, observer.lock() returns nullptr.\n";
        cout << "  weak_ptr does NOT keep the object alive.\n";
    }
}

// ============================================================
//  7. The cyclic-reference problem (and the weak_ptr fix)
//
//  If two shared_ptrs point at each other, the reference count
//  for each one is always at least 1 — they keep each other alive
//  forever. Solution: make one of the two links a weak_ptr.
// ============================================================

struct Node {
    string name;
    shared_ptr<Node> next;
    // shared_ptr<Node> prev;   // <-- THIS would cause a cycle
    weak_ptr<Node>   prev;      // weak_ptr breaks the cycle

    Node(const string& n) : name(n) {
        cout << "    [+] Node " << name << " constructed\n";
    }
    ~Node() {
        cout << "    [-] Node " << name << " destroyed\n";
    }
};

void demoCyclicReference() {
    separator("7. Breaking cycles with weak_ptr");

    {
        auto a = make_shared<Node>("A");
        auto b = make_shared<Node>("B");
        a->next = b;
        b->prev = a;       // weak_ptr — does not bump A's count
        cout << "  Linked A <-> B. Leaving scope...\n";
    }
    cout << "  Both nodes were destroyed cleanly. If `prev` had been a\n";
    cout << "  shared_ptr, neither would have been destroyed: classic leak.\n";
}

// ============================================================
//  8. Passing smart pointers to functions — the conventions
// ============================================================

// Takes ownership: caller must transfer with std::move.
void consumeShape(unique_ptr<Shape> s) {
    cout << "  consumeShape received: ";
    s->draw();
    // `s` goes out of scope here → object destroyed.
}

// Doesn't own anything — just uses the Shape. Use raw pointer or
// reference for this. NOT a smart pointer.
void inspectShape(const Shape& s) {
    cout << "  inspectShape received: ";
    s.draw();
}

void demoOwnershipConventions() {
    separator("8. Passing smart pointers — ownership conventions");

    auto s = make_unique<Circle>(3.3, "passed_around");

    // Inspect: pass by reference. We still own it afterwards.
    inspectShape(*s);
    cout << "  After inspectShape, we still own s.\n";

    // Consume: transfer ownership with std::move. After this, s is empty.
    consumeShape(std::move(s));
    cout << "  After consumeShape with std::move, s is "
         << (s ? "non-null" : "null") << ".\n";
}

// ============================================================
//  9. Exception safety — smart pointers + stack unwinding = no leaks
//
//  Lab 7 showed that destructors run during stack unwinding. That
//  means a unique_ptr that goes out of scope because an exception
//  was thrown still deletes its object. Raw `new`/`delete` cannot
//  give you this guarantee without lots of try/catch boilerplate.
// ============================================================

void mightThrow(bool doThrow) {
    auto safe = make_unique<Circle>(1.1, "exception_safe");

    if (doThrow)
        throw runtime_error("boom!");

    safe->draw();
    // safe is destroyed automatically here as well.
}

void demoExceptionSafety() {
    separator("9. Smart pointers + exceptions = automatic cleanup");

    cout << "  Calling mightThrow(true) — throws halfway through:\n";
    try {
        mightThrow(true);
    } catch (const exception& e) {
        cout << "  Caught: " << e.what() << "\n";
    }
    cout << "  Notice: the Circle inside mightThrow was still destroyed\n";
    cout << "  during stack unwinding. No leak, even on the error path.\n";
}

// ============================================================
//  10. Custom deleters — for resources that aren't `new`-allocated
//
//  Sometimes the cleanup is not `delete`. Files, sockets, mutexes,
//  C library handles — they each have their own release function.
//  unique_ptr and shared_ptr both let you supply a custom deleter.
// ============================================================

void demoCustomDeleter() {
    separator("10. Custom deleter — releasing non-`new` resources");

    // Imagine acquireHandle() / releaseHandle() are an old C API.
    auto acquire = [](const string& name) -> int* {
        cout << "    [+] acquired handle " << name << "\n";
        return new int(42);
    };
    auto release = [](int* h) {
        cout << "    [-] released handle\n";
        delete h;
    };

    {
        unique_ptr<int, decltype(release)> handle(acquire("file"), release);
        cout << "  using handle, value = " << *handle << "\n";
    }
    cout << "  Handle was released automatically — same RAII pattern.\n";
}

// ============================================================
//  main — runs every demo in order
// ============================================================

int main() {
    demoRawPointerProblem();
    demoUniquePtr();
    demoMakeUnique();
    demoPolymorphicUniquePtr();
    demoSharedPtr();
    demoWeakPtr();
    demoCyclicReference();
    demoOwnershipConventions();
    demoExceptionSafety();
    demoCustomDeleter();
    return 0;
}
