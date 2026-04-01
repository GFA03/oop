#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <iomanip>

using namespace std;

// ─────────────────────────────────────────────────────────────────────────────
//  Shape  —  abstract base class
// ─────────────────────────────────────────────────────────────────────────────

class Shape {
protected:
    static int noShapes;   // shared counter across ALL shape objects
    const int id;          // unique, immutable identifier
    string color;

public:
    Shape(const string& color);
    Shape(const Shape& other);
    virtual ~Shape();      // MUST be virtual — see section 9 in the README

    int    getId()    const;
    string getColor() const;

    // Pure virtual methods — Shape is abstract; derived classes MUST override these
    virtual double area()      const = 0;
    virtual double perimeter() const = 0;
    virtual string typeName()  const = 0;

    friend ostream& operator<<(ostream& os, const Shape& s);
};

int Shape::noShapes = 0;

Shape::Shape(const string& color)
    : id(++noShapes), color(color) {}

// Copy constructor: assigns a new id (independent object), copies color
Shape::Shape(const Shape& other)
    : id(++noShapes), color(other.color) {}

Shape::~Shape() {}   // std::string cleans itself up; nothing else to free here

int    Shape::getId()    const { return id; }
string Shape::getColor() const { return color; }

ostream& operator<<(ostream& os, const Shape& s) {
    os << s.typeName() << " #" << s.id
       << " | color: "     << s.color
       << " | area: "      << fixed << setprecision(2) << s.area()
       << " | perimeter: " << fixed << setprecision(2) << s.perimeter();
    return os;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Circle  —  inherits from Shape
// ─────────────────────────────────────────────────────────────────────────────

class Circle : public Shape {
    double radius;

public:
    Circle(double radius, const string& color);
    Circle(const Circle& other);
    ~Circle() override;

    double area()      const override;
    double perimeter() const override;
    string typeName()  const override;
};

Circle::Circle(double radius, const string& color)
    : Shape(color), radius(radius) {}   // calls Shape constructor via initializer list

Circle::Circle(const Circle& other)
    : Shape(other), radius(other.radius) {}   // forwards to Shape copy constructor

Circle::~Circle() {}

double Circle::area()      const { return M_PI * radius * radius; }
double Circle::perimeter() const { return 2.0 * M_PI * radius; }
string Circle::typeName()  const { return "Circle"; }

// ─────────────────────────────────────────────────────────────────────────────
//  Rectangle  —  inherits from Shape
// ─────────────────────────────────────────────────────────────────────────────

class Rectangle : public Shape {
    double width, height;

public:
    Rectangle(double width, double height, const string& color);
    Rectangle(const Rectangle& other);
    ~Rectangle() override;

    double area()      const override;
    double perimeter() const override;
    string typeName()  const override;
};

Rectangle::Rectangle(double width, double height, const string& color)
    : Shape(color), width(width), height(height) {}

Rectangle::Rectangle(const Rectangle& other)
    : Shape(other), width(other.width), height(other.height) {}

Rectangle::~Rectangle() {}

double Rectangle::area()      const { return width * height; }
double Rectangle::perimeter() const { return 2.0 * (width + height); }
string Rectangle::typeName()  const { return "Rectangle"; }

// ─────────────────────────────────────────────────────────────────────────────
//  Triangle  —  inherits from Shape
// ─────────────────────────────────────────────────────────────────────────────

class Triangle : public Shape {
    double a, b, c;   // side lengths

public:
    Triangle(double a, double b, double c, const string& color);
    Triangle(const Triangle& other);
    ~Triangle() override;

    double area()      const override;   // Heron's formula
    double perimeter() const override;
    string typeName()  const override;
};

Triangle::Triangle(double a, double b, double c, const string& color)
    : Shape(color), a(a), b(b), c(c) {}

Triangle::Triangle(const Triangle& other)
    : Shape(other), a(other.a), b(other.b), c(other.c) {}

Triangle::~Triangle() {}

double Triangle::area() const {
    double s = (a + b + c) / 2.0;   // semi-perimeter
    return sqrt(s * (s - a) * (s - b) * (s - c));   // Heron's formula
}
double Triangle::perimeter() const { return a + b + c; }
string Triangle::typeName()  const { return "Triangle"; }

// ─────────────────────────────────────────────────────────────────────────────
//  Diamond Inheritance Demo
//
//  Without virtual inheritance, a class that inherits from two classes that
//  both inherit from the same base ends up with TWO copies of that base —
//  ambiguous access and wasted memory. `virtual` inheritance solves this by
//  ensuring only one shared base subobject exists.
//
//              Shape  (abstract, virtual base)
//             /      \
//        Outlined   Filled
//             \      /
//          FilledCircle   ← one Shape subobject, not two
// ─────────────────────────────────────────────────────────────────────────────

class Outlined : virtual public Shape {
protected:
    double strokeWidth;
public:
    Outlined(const string& color, double strokeWidth);
    string strokeInfo() const;
};

Outlined::Outlined(const string& color, double strokeWidth)
    : Shape(color), strokeWidth(strokeWidth) {}

string Outlined::strokeInfo() const {
    return "stroke=" + to_string(strokeWidth) + "px";
}

class Filled : virtual public Shape {
protected:
    string fillPattern;
public:
    Filled(const string& color, const string& fillPattern);
    string fillInfo() const;
};

Filled::Filled(const string& color, const string& fillPattern)
    : Shape(color), fillPattern(fillPattern) {}

string Filled::fillInfo() const {
    return "fill=" + fillPattern;
}

// Most-derived class — responsible for initialising the virtual base (Shape).
// The Shape(color) calls inside Outlined and Filled are skipped.
class FilledCircle : public Outlined, public Filled {
    double radius;
public:
    FilledCircle(double radius, const string& color,
                 double strokeWidth, const string& fillPattern);

    double area()      const override;
    double perimeter() const override;
    string typeName()  const override;
    void   describe()  const;
};

FilledCircle::FilledCircle(double radius, const string& color,
                            double strokeWidth, const string& fillPattern)
    : Shape(color),                   // virtual base: only this call takes effect
      Outlined(color, strokeWidth),   // Shape(color) inside Outlined is skipped
      Filled(color, fillPattern),     // Shape(color) inside Filled is skipped
      radius(radius) {}

double FilledCircle::area()      const { return M_PI * radius * radius; }
double FilledCircle::perimeter() const { return 2.0 * M_PI * radius; }
string FilledCircle::typeName()  const { return "FilledCircle"; }

void FilledCircle::describe() const {
    cout << typeName() << " #" << getId()
         << " | " << strokeInfo()
         << " | " << fillInfo()
         << " | color: " << getColor() << '\n';
}

// ─────────────────────────────────────────────────────────────────────────────
//  Static vs. dynamic dispatch demo classes
//  These two pairs exist solely to illustrate the difference virtual makes.
// ─────────────────────────────────────────────────────────────────────────────

class BaseNoVirtual {
public:
    void whoAmI() const { cout << "Base (no virtual)\n"; }
};

class DerivedNoVirtual : public BaseNoVirtual {
public:
    void whoAmI() const { cout << "Derived (no virtual)\n"; }
};

class BaseVirtual {
public:
    virtual void whoAmI() const { cout << "Base (virtual)\n"; }
    virtual ~BaseVirtual() {}
};

class DerivedVirtual : public BaseVirtual {
public:
    void whoAmI() const override { cout << "Derived (virtual)\n"; }
};

// ─────────────────────────────────────────────────────────────────────────────
//  main
// ─────────────────────────────────────────────────────────────────────────────

int main() {

    // ── 1. Abstract class ────────────────────────────────────────────────────
    cout << "─── 1. Abstract class ───────────────────────────────────────\n";
    // Shape s("red");   // uncomment → compile error: cannot instantiate abstract class
    cout << "Shape is abstract: `Shape s;` does not compile.\n\n";

    // ── 2. Static vs. dynamic dispatch ───────────────────────────────────────
    cout << "─── 2. Static vs. dynamic dispatch ─────────────────────────\n";

    BaseNoVirtual* p1 = new DerivedNoVirtual();
    BaseVirtual*   p2 = new DerivedVirtual();

    cout << "Base pointer, no virtual  → "; p1->whoAmI();   // calls BASE version
    cout << "Base pointer, virtual     → "; p2->whoAmI();   // calls DERIVED version

    delete p1;
    delete p2;
    cout << '\n';

    // ── 3. Polymorphic collection ─────────────────────────────────────────────
    cout << "─── 3. Polymorphic collection ───────────────────────────────\n";

    vector<Shape*> shapes;
    shapes.push_back(new Circle    (5.0,              "red"   ));
    shapes.push_back(new Rectangle (4.0,  6.0,        "blue"  ));
    shapes.push_back(new Triangle  (3.0,  4.0,  5.0,  "green" ));
    shapes.push_back(new Circle    (2.5,              "yellow"));
    shapes.push_back(new Rectangle (10.0, 3.0,        "purple"));
    shapes.push_back(new Triangle  (6.0,  8.0,  10.0, "orange"));

    for (const Shape* s : shapes)
        cout << *s << '\n';

    // ── 4. Each pointer calls the right method ────────────────────────────────
    cout << "\n─── 4. Dynamic dispatch on the collection ───────────────────\n";
    cout << "Calling area() through Shape* pointers:\n";
    for (const Shape* s : shapes)
        cout << "  " << s->typeName() << " #" << s->getId()
             << "  →  area = " << fixed << setprecision(2) << s->area() << '\n';

    // ── 5. Find shape with largest area ───────────────────────────────────────
    cout << "\n─── 5. Largest area ─────────────────────────────────────────\n";
    Shape* largest = shapes[0];
    for (Shape* s : shapes)
        if (s->area() > largest->area())
            largest = s;
    cout << "Largest: " << *largest << '\n';

    // ── 6. Total area ─────────────────────────────────────────────────────────
    cout << "\n─── 6. Total area ───────────────────────────────────────────\n";
    double total = 0.0;
    for (const Shape* s : shapes)
        total += s->area();
    cout << "Sum of all areas: " << fixed << setprecision(2) << total << '\n';

    // ── 7. Copy constructor ────────────────────────────────────────────────────
    cout << "\n─── 7. Copy constructor ─────────────────────────────────────\n";
    Circle original(7.0, "pink");
    Circle copy = original;
    cout << "Original: " << original << '\n';
    cout << "Copy:     " << copy     << '\n';
    cout << "(Copy gets a new id — they are independent objects)\n";

    // ── 8. Virtual destructor ──────────────────────────────────────────────────
    cout << "\n─── 8. Cleanup via virtual destructor ───────────────────────\n";
    for (Shape* s : shapes)
        delete s; // virtual destructor ensures each derived destructor is called
    shapes.clear();
    cout << "All shapes deleted correctly.\n";

    // ── 9. Diamond inheritance ─────────────────────────────────────────────────
    cout << "\n─── 9. Diamond inheritance ──────────────────────────────────\n";
    cout << "FilledCircle inherits from both Outlined and Filled,\n"
         << "which both inherit *virtually* from Shape.\n"
         << "Result: exactly ONE Shape subobject (one id, one color).\n\n";

    FilledCircle fc(3.0, "cyan", 1.5, "hatched");
    fc.describe();
    cout << *static_cast<Shape*>(&fc) << '\n';

    // Both intermediate-class pointers refer to the same Shape subobject
    Outlined* op = &fc;
    Filled*   fp = &fc;
    cout << "\nVia Outlined* -> id = " << op->getId() << '\n';
    cout << "Via Filled*   -> id = " << fp->getId() << '\n';
    cout << "(Same id — single shared Shape base, not two copies)\n";

    return 0;
}
