/*
 * Lab 12 — The FACTORY METHOD Pattern
 *
 * Problem: A drawing program lets the user type the name of a shape
 *   ("circle", "square", "triangle") and creates the right object.
 *
 *   - Concrete shape classes will be added over time.
 *   - Many places in the codebase create shapes from user input.
 *
 * We will solve the problem twice:
 *   1) WITHOUT the pattern — every call site contains a big if/else
 *      that knows about every concrete class
 *   2) WITH the Factory pattern — one factory hides the construction
 *      logic. Call sites just say "give me the shape called X"
 */

#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

using namespace std;

void section(const string& title) {
    cout << "\n══════════════════════════════════════════\n";
    cout << "  " << title << "\n";
    cout << "══════════════════════════════════════════\n";
}

// ============================================================
//  Common Shape hierarchy used by both versions
// ============================================================
class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw() const = 0;
};

class Circle   : public Shape { public: void draw() const override { cout << "    drawing a circle\n";   } };
class Square   : public Shape { public: void draw() const override { cout << "    drawing a square\n";   } };
class Triangle : public Shape { public: void draw() const override { cout << "    drawing a triangle\n"; } };

// ============================================================
//  1. WITHOUT the pattern
//
//  The decision "which concrete class do I instantiate?" is copy-
//  pasted at every call site. If a new shape is added (Hexagon,
//  Ellipse, …) you have to find and update every one of these
//  if/else trees. Miss one and the new shape silently isn't supported.
// ============================================================

namespace without_pattern {

void renderUserShape(const string& type) {
    Shape* s = nullptr;
    if      (type == "circle")   s = new Circle();
    else if (type == "square")   s = new Square();
    else if (type == "triangle") s = new Triangle();
    else    throw runtime_error("unknown shape: " + type);

    s->draw();
    delete s;
}

void exportToSVG(const string& type) {
    // The SAME if/else, copy-pasted. Add a new shape → update here too.
    Shape* s = nullptr;
    if      (type == "circle")   s = new Circle();
    else if (type == "square")   s = new Square();
    else if (type == "triangle") s = new Triangle();
    else    throw runtime_error("unknown shape: " + type);

    cout << "    (pretending to export ";
    s->draw();
    delete s;
}

void demo() {
    section("1. WITHOUT Factory — every call site repeats the if/else");

    renderUserShape("circle");
    renderUserShape("square");
    exportToSVG("triangle");

    cout << "  If we add a new shape, we must update every place that\n";
    cout << "  contains this if/else. Easy to miss one.\n";
}

} // namespace without_pattern

// ============================================================
//  2. WITH the Factory pattern
//
//  A single function (or static method) owns the construction logic.
//  Client code never mentions the concrete classes — it asks the
//  factory for a Shape by name.
//
//  Adding a new shape now requires changing exactly ONE place: the
//  factory itself. Everything else just works.
// ============================================================

namespace with_pattern {

class ShapeFactory {
public:
    static unique_ptr<Shape> create(const string& type) {
        if (type == "circle")   return make_unique<Circle>();
        if (type == "square")   return make_unique<Square>();
        if (type == "triangle") return make_unique<Triangle>();
        throw runtime_error("unknown shape: " + type);
    }
};

void renderUserShape(const string& type) {
    auto s = ShapeFactory::create(type);
    s->draw();
}

void exportToSVG(const string& type) {
    auto s = ShapeFactory::create(type);
    cout << "    (pretending to export ";
    s->draw();
}

void demo() {
    section("2. WITH Factory — construction lives in ONE place");

    renderUserShape("circle");
    renderUserShape("square");
    exportToSVG("triangle");

    cout << "  Adding a new shape now means editing ShapeFactory::create\n";
    cout << "  and nothing else. Call sites don't even know it changed.\n";
}

} // namespace with_pattern

// ============================================================
//  Why the Factory version is better
//   - DRY: the "string → class" mapping lives in ONE place.
//   - Open/Closed: adding a new shape doesn't touch existing call sites.
//   - Call sites stop depending on every concrete class — they only
//     depend on the Shape interface and the factory.
//   - We also dropped manual `new`/`delete`: the factory returns a
//     unique_ptr (see lab 11), so no leaks even on error paths.
// ============================================================

int main() {
    without_pattern::demo();
    with_pattern::demo();
    return 0;
}
