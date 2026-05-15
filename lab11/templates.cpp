/*
 * Lab 11 — Templates
 *
 * Concepts:
 *   - Why templates: the motivating problem
 *   - Function templates
 *   - Template type deduction
 *   - Multiple type parameters
 *   - Class templates
 *   - Non-type template parameters
 *   - Template specialization
 *   - Templates and polymorphism (a tiny generic container)
 *   - Where templates live (.h files)
 */

#include <iostream>
#include <vector>
#include <string>
#include <memory>

using namespace std;

void separator(const string& title) {
    cout << "\n══════════════════════════════════════════\n";
    cout << "  " << title << "\n";
    cout << "══════════════════════════════════════════\n";
}

// ============================================================
//  1. THE PROBLEM — writing the same code three times
//
//  Suppose we want a function that returns the larger of two
//  values. We can write it for int:
// ============================================================

int maxInt(int a, int b) {
    return (a > b) ? a : b;
}

// Now we also need it for double…
double maxDouble(double a, double b) {
    return (a > b) ? a : b;
}

// And for strings…
string maxString(const string& a, const string& b) {
    return (a > b) ? a : b;
}

// Notice: three functions, identical bodies. Only the types differ.
// That repetition is what templates eliminate.

void demoProblem() {
    separator("1. The problem — same code, different types");

    cout << "  maxInt(3, 7)             = " << maxInt(3, 7) << "\n";
    cout << "  maxDouble(2.5, 1.7)      = " << maxDouble(2.5, 1.7) << "\n";
    cout << "  maxString(\"cat\", \"dog\") = " << maxString("cat", "dog") << "\n";
    cout << "  Three functions, identical logic. There must be a better way.\n";
}

// ============================================================
//  2. FUNCTION TEMPLATES — write once, call with any type
//
//  `template <typename T>` says: "this is a recipe for a function;
//  the compiler will generate a real function for whichever type
//  T you actually use it with."
// ============================================================

template <typename T>
T maximum(T a, T b) {
    return (a > b) ? a : b;
}

void demoFunctionTemplate() {
    separator("2. Function template — one recipe, many instances");

    cout << "  maximum(3, 7)            = " << maximum(3, 7) << "       (T = int)\n";
    cout << "  maximum(2.5, 1.7)        = " << maximum(2.5, 1.7) << "     (T = double)\n";
    cout << "  maximum(string(\"cat\"), string(\"dog\")) = "
         << maximum<string>("cat", "dog") << "  (T = string)\n";

    // You can also write the type explicitly:
    cout << "  maximum<double>(3, 2.5)  = " << maximum<double>(3, 2.5) << "\n";
    cout << "  The compiler stamps out a separate function body for each T it sees.\n";
}

// ============================================================
//  3. TYPE DEDUCTION — the compiler figures T out for you
//
//  In maximum(3, 7) you did not write maximum<int>. The compiler
//  saw two ints and deduced T = int automatically.
//
//  Type deduction fails if the arguments don't agree:
//
//      maximum(3, 2.5);    // ERROR — int vs double, T can't be both
//
//  In that case you write the type explicitly: maximum<double>(3, 2.5).
// ============================================================

void demoTypeDeduction() {
    separator("3. Type deduction — usually invisible");

    cout << "  Implicit: maximum(1, 2)    = " << maximum(1, 2) << "\n";
    cout << "  Explicit: maximum<int>(1,2)= " << maximum<int>(1, 2) << "\n";

    // maximum(1, 2.0);   // would not compile — ambiguous T
    cout << "  Mixed-type calls need an explicit <T>:\n";
    cout << "  maximum<double>(1, 2.0)   = " << maximum<double>(1, 2.0) << "\n";
}

// ============================================================
//  4. MULTIPLE TYPE PARAMETERS
//
//  Templates aren't limited to one type. You can take as many
//  type parameters as you need.
// ============================================================

template <typename A, typename B>
void printPair(const A& first, const B& second) {
    cout << "  (" << first << ", " << second << ")\n";
}

void demoMultipleTypeParams() {
    separator("4. Multiple type parameters");

    printPair(1, "one");        // A = int, B = const char*
    printPair(3.14, true);      // A = double, B = bool
    printPair(string("pi"), 3.14);
}

// ============================================================
//  5. CLASS TEMPLATES — a class blueprint for any type
//
//  A class template defines a class shape; the actual class is
//  generated when you write `Box<int>` or `Box<string>`.
//
//  std::vector<T>, std::map<K,V>, std::shared_ptr<T> — all class
//  templates from the standard library.
// ============================================================

template <typename T>
class Box {
    T contents;
public:
    Box(const T& value) : contents(value) {}

    T get() const { return contents; }
    void set(const T& value) { contents = value; }

    void print() const {
        cout << "  Box contains: " << contents << "\n";
    }
};

void demoClassTemplate() {
    separator("5. Class template — Box<T>");

    Box<int>    boxOfInt(42);
    Box<string> boxOfStr("hello");
    Box<double> boxOfDouble(3.14);

    boxOfInt.print();
    boxOfStr.print();
    boxOfDouble.print();

    boxOfInt.set(100);
    boxOfInt.print();

    // Box<int> and Box<string> are TWO DIFFERENT TYPES at compile time.
    // You cannot assign one to the other.
}

// ============================================================
//  6. NON-TYPE TEMPLATE PARAMETERS
//
//  Template parameters can also be integer values, not just types.
//  std::array<T, N> is the classic example: N is part of the type.
// ============================================================

template <typename T, int N>
class FixedArray {
    T data[N];
public:
    int size() const { return N; }
    T& operator[](int i) { return data[i]; }
    const T& operator[](int i) const { return data[i]; }
};

void demoNonTypeParam() {
    separator("6. Non-type template parameters");

    FixedArray<int, 5> a;
    for (int i = 0; i < a.size(); i++)
        a[i] = i * i;

    cout << "  FixedArray<int, 5>: ";
    for (int i = 0; i < a.size(); i++)
        cout << a[i] << " ";
    cout << "\n";

    FixedArray<string, 3> b;
    b[0] = "one"; b[1] = "two"; b[2] = "three";
    cout << "  FixedArray<string, 3>: ";
    for (int i = 0; i < b.size(); i++)
        cout << b[i] << " ";
    cout << "\n";

    cout << "  FixedArray<int,5> and FixedArray<int,6> are DIFFERENT TYPES.\n";
}

// ============================================================
//  7. TEMPLATE SPECIALIZATION — different behaviour for a type
//
//  Sometimes the generic version isn't quite right for one
//  specific type. You can provide a "specialization" for that
//  type, and the compiler will use it instead of the generic.
// ============================================================

template <typename T>
void describe(const T& value) {
    cout << "  generic value: " << value << "\n";
}

// Full specialization for bool — print "true"/"false" instead of 1/0
template <>
void describe<bool>(const bool& value) {
    cout << "  boolean: " << (value ? "true" : "false") << "\n";
}

// Full specialization for const char* — quote it
template <>
void describe<const char*>(const char* const& value) {
    cout << "  c-string: \"" << value << "\"\n";
}

void demoSpecialization() {
    separator("7. Template specialization — special-case a type");

    describe(42);          // uses generic
    describe(3.14);        // uses generic
    describe(true);        // uses bool specialization
    describe(false);       // uses bool specialization
    const char* msg = "hi";
    describe(msg);         // uses c-string specialization
}

// ============================================================
//  8. A TINY GENERIC CONTAINER — Stack<T>
//
//  Putting it together: a stack that holds any type. This is the
//  same shape as std::stack<T> in the standard library.
// ============================================================

template <typename T>
class Stack {
    vector<T> data;
public:
    void push(const T& value) { data.push_back(value); }

    void pop() {
        if (data.empty()) return;
        data.pop_back();
    }

    T& top() { return data.back(); }
    const T& top() const { return data.back(); }

    bool empty() const { return data.empty(); }
    size_t size() const { return data.size(); }
};

void demoGenericStack() {
    separator("8. Generic Stack<T>");

    Stack<int> intStack;
    intStack.push(1);
    intStack.push(2);
    intStack.push(3);
    cout << "  intStack.top() = " << intStack.top() << "\n";
    intStack.pop();
    cout << "  after pop, top = " << intStack.top() << "\n";

    Stack<string> strStack;
    strStack.push("first");
    strStack.push("second");
    cout << "  strStack.top() = " << strStack.top() << "\n";

    cout << "  One implementation, two completely different containers.\n";
}

// ============================================================
//  9. TEMPLATES AND POLYMORPHISM — generic algorithms over Shape
//
//  Templates and inheritance solve different problems and combine
//  beautifully. Inheritance gives you runtime polymorphism (which
//  Shape virtual function gets called depends on the real type).
//  Templates give you compile-time genericity (the same code is
//  reused for many types).
// ============================================================

class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw() const = 0;
};

class Circle    : public Shape { public: void draw() const override { cout << "    a circle\n";    } };
class Rectangle : public Shape { public: void draw() const override { cout << "    a rectangle\n"; } };

// A function template that draws any container of Shape*-like
// items (raw pointers, unique_ptrs, shared_ptrs — anything you
// can call ->draw() on).
template <typename Container>
void drawAll(const Container& items) {
    for (const auto& item : items)
        item->draw();
}

void demoTemplatesWithPolymorphism() {
    separator("9. Templates + polymorphism — drawAll for any container");

    vector<unique_ptr<Shape>> a;
    a.push_back(make_unique<Circle>());
    a.push_back(make_unique<Rectangle>());

    vector<shared_ptr<Shape>> b;
    b.push_back(make_shared<Circle>());
    b.push_back(make_shared<Rectangle>());

    cout << "  drawAll over vector<unique_ptr<Shape>>:\n";
    drawAll(a);

    cout << "  drawAll over vector<shared_ptr<Shape>>:\n";
    drawAll(b);

    cout << "  Same template, two different container types — written once.\n";
}

// ============================================================
//  main
// ============================================================

int main() {
    demoProblem();
    demoFunctionTemplate();
    demoTypeDeduction();
    demoMultipleTypeParams();
    demoClassTemplate();
    demoNonTypeParam();
    demoSpecialization();
    demoGenericStack();
    demoTemplatesWithPolymorphism();
    return 0;
}
