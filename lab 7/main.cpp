/*
 * Lab 7 — Type Casting, Exception Handling, and Interfaces
 *
 * Concepts:
 *   - static_cast  : compile-time casts (numeric, up/downcast)
 *   - dynamic_cast : runtime-checked downcast in polymorphic hierarchies
 *   - const_cast   : removing const for legacy APIs
 *   - reinterpret_cast : raw bit reinterpretation
 *   - Exception hierarchy: throw / try / catch / stack unwinding
 *   - Interfaces: pure abstract classes used as contracts (no data)
 */

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <stdexcept>
#include <sstream>

using namespace std;

// ============================================================
//  1. CUSTOM EXCEPTION HIERARCHY
//
//  Derive from std::runtime_error so our exceptions fit naturally
//  into the standard exception hierarchy and carry a message.
// ============================================================

class ShapeException : public runtime_error {
public:
    explicit ShapeException(const string& msg) : runtime_error(msg) {}
};

// A more specific subclass — carries the bad value for inspection.
class InvalidDimensionException : public ShapeException {
    double badValue;
public:
    InvalidDimensionException(const string& field, double val)
        : ShapeException("Invalid dimension for '" + field + "': " + to_string(val)),
          badValue(val) {}

    double getBadValue() const { return badValue; }
};

// ============================================================
//  2. INTERFACES
//
//  A pure abstract class with no data members acts as a contract.
//  Any class that inherits from it and implements all = 0 methods
//  is said to "satisfy" that interface.
//
//  Key difference from an abstract base class like Shape:
//    - An interface carries NO state (no data members).
//    - It expresses a capability ("can be drawn", "can be serialized"),
//      not an identity ("is a shape").
//    - A class can satisfy multiple interfaces simultaneously.
// ============================================================

class Drawable {
public:
    virtual void draw() const = 0;
    virtual ~Drawable() = default;
};

class Serializable {
public:
    virtual string serialize() const = 0;
    virtual ~Serializable() = default;
};

// ============================================================
//  3. ABSTRACT BASE CLASS
// ============================================================

class Shape {
protected:
    static int noShapes;
    const int  id;
    string     color;

public:
    Shape(const string& color) : id(++noShapes), color(color) {}
    Shape(const Shape& other)  : id(++noShapes), color(other.color) {}
    virtual ~Shape() = default;

    int    getId()    const { return id;    }
    string getColor() const { return color; }

    virtual double area()      const = 0;
    virtual double perimeter() const = 0;
    virtual string typeName()  const = 0;

    friend ostream& operator<<(ostream& os, const Shape& s) {
        os << s.typeName() << " #" << s.id
           << " [color=" << s.color
           << ", area="  << s.area()
           << ", perimeter=" << s.perimeter() << "]";
        return os;
    }
};

int Shape::noShapes = 0;

// ============================================================
//  4. CONCRETE SHAPES
//
//  Each shape inherits from Shape (identity) and from both
//  Drawable and Serializable (capabilities / contracts).
//
//  Constructors validate their arguments and throw
//  InvalidDimensionException or ShapeException on bad input.
// ============================================================

class Circle : public Shape, public Drawable, public Serializable {
    double radius;
public:
    Circle(double radius, const string& color) : Shape(color) {
        if (radius <= 0)
            throw InvalidDimensionException("radius", radius);
        this->radius = radius;
    }

    double getRadius() const { return radius; }

    double area()      const override { return M_PI * radius * radius; }
    double perimeter() const override { return 2.0 * M_PI * radius; }
    string typeName()  const override { return "Circle"; }

    void draw() const override {
        cout << "    Drawing a " << color << " circle with radius " << radius << "\n";
    }

    string serialize() const override {
        ostringstream oss;
        oss << "Circle{id=" << id << ",radius=" << radius << ",color=" << color << "}";
        return oss.str();
    }
};

class Rectangle : public Shape, public Drawable, public Serializable {
    double width, height;
public:
    Rectangle(double width, double height, const string& color) : Shape(color) {
        if (width  <= 0) throw InvalidDimensionException("width",  width);
        if (height <= 0) throw InvalidDimensionException("height", height);
        this->width  = width;
        this->height = height;
    }

    double getWidth()  const { return width;  }
    double getHeight() const { return height; }

    double area()      const override { return width * height; }
    double perimeter() const override { return 2.0 * (width + height); }
    string typeName()  const override { return "Rectangle"; }

    void draw() const override {
        cout << "    Drawing a " << color << " rectangle "
             << width << "x" << height << "\n";
    }

    string serialize() const override {
        ostringstream oss;
        oss << "Rectangle{id=" << id
            << ",width="  << width
            << ",height=" << height
            << ",color="  << color << "}";
        return oss.str();
    }
};

class Triangle : public Shape, public Drawable, public Serializable {
    double a, b, c;

    static void validate(double a, double b, double c) {
        if (a <= 0) throw InvalidDimensionException("a", a);
        if (b <= 0) throw InvalidDimensionException("b", b);
        if (c <= 0) throw InvalidDimensionException("c", c);
        if (a + b <= c || a + c <= b || b + c <= a)
            throw ShapeException(
                "Triangle inequality violated: sides " +
                to_string(a) + ", " + to_string(b) + ", " + to_string(c));
    }

public:
    Triangle(double a, double b, double c, const string& color)
        : Shape(color), a(a), b(b), c(c) { validate(a, b, c); }

    double area() const override {
        double s = (a + b + c) / 2.0;
        return sqrt(s * (s-a) * (s-b) * (s-c));  // Heron's formula
    }
    double perimeter() const override { return a + b + c; }
    string typeName()  const override { return "Triangle"; }

    void draw() const override {
        cout << "    Drawing a " << color << " triangle with sides "
             << a << ", " << b << ", " << c << "\n";
    }

    string serialize() const override {
        ostringstream oss;
        oss << "Triangle{id=" << id
            << ",a=" << a << ",b=" << b << ",c=" << c
            << ",color=" << color << "}";
        return oss.str();
    }
};

// ============================================================
//  5. INTERFACE-TYPED FUNCTIONS
//
//  These functions accept objects purely through their interface.
//  They know nothing about Shape, Circle, etc. — only the contract.
// ============================================================

void renderAll(const vector<Drawable*>& items) {
    for (const Drawable* d : items)
        d->draw();
}

void saveAll(const vector<Serializable*>& items) {
    for (const Serializable* s : items)
        cout << "    " << s->serialize() << "\n";
}

// ============================================================
//  6. DEMO FUNCTIONS
// ============================================================

void separator(const string& title) {
    cout << "\n══════════════════════════════════════════\n";
    cout << "  " << title << "\n";
    cout << "══════════════════════════════════════════\n";
}

// ------------------------------------------------------------------
//  6a. static_cast
// ------------------------------------------------------------------
void demoStaticCast() {
    separator("static_cast");

    // --- Numeric conversions ---
    double d = 3.99;
    int    i = static_cast<int>(d);           // truncates, never rounds
    cout << "  static_cast<int>(3.99) = " << i << "  (truncates, no rounding)\n";

    int x = 7, y = 2;
    cout << "  7 / 2 (int arithmetic)    = " << (x / y) << "\n";
    cout << "  7 / 2 (cast first)        = " << (static_cast<double>(x) / y) << "\n";

    // --- Upcast (Derived* → Base*): always safe, rarely needs explicit cast ---
    Circle* c = new Circle(5.0, "red");
    Shape*  s = static_cast<Shape*>(c);
    cout << "\n  Upcast Circle* → Shape*:  " << *s << "\n";

    // --- Downcast (Base* → Derived*): no runtime check ---
    //  When you KNOW the pointer really is a Circle, this is safe:
    Circle* c2 = static_cast<Circle*>(s);
    cout << "  Downcast back → Circle*:  radius = " << c2->getRadius() << "\n";

    //  When you're WRONG, it compiles silently and produces undefined behaviour:
    Shape* wrong = new Rectangle(4.0, 6.0, "blue");
    Circle* bad  = static_cast<Circle*>(wrong);   // compiles — no warning
    //  Calling bad->getRadius() here is undefined behaviour (may crash or
    //  return garbage); we cast it back to void* just to show the pointers
    //  are equal without actually dereferencing the bad cast.
    cout << "\n  static_cast<Circle*>(Rectangle*): compiles with no error\n";
    cout << "  But calling getRadius() on it is UNDEFINED BEHAVIOUR — don't do it!\n";
    (void)bad;

    delete c;
    delete wrong;
}

// ------------------------------------------------------------------
//  6b. dynamic_cast
//      Requires at least one virtual function in the hierarchy (RTTI).
//      Returns nullptr (pointer form) or throws std::bad_cast (reference form).
// ------------------------------------------------------------------
void demoDynamicCast() {
    separator("dynamic_cast");

    vector<Shape*> shapes = {
        new Circle   (5.0,            "red"   ),
        new Rectangle(4.0,  6.0,      "blue"  ),
        new Circle   (3.0,            "green" ),
        new Triangle (3.0,  4.0, 5.0, "yellow"),
    };

    // --- Pointer form: check the result before using it ---
    cout << "  Pointer form (nullptr on failure):\n";
    for (Shape* s : shapes) {
        Circle*    c = dynamic_cast<Circle*>(s);
        Rectangle* r = dynamic_cast<Rectangle*>(s);

        if (c)
            cout << "    Circle    → radius = " << c->getRadius() << "\n";
        else if (r)
            cout << "    Rectangle → " << r->getWidth() << " x " << r->getHeight() << "\n";
        else
            cout << "    " << s->typeName() << " → (no type-specific info)\n";
    }

    // --- Reference form: throws std::bad_cast on failure ---
    cout << "\n  Reference form (throws bad_cast on failure):\n";
    try {
        Circle& c = dynamic_cast<Circle&>(*shapes[0]);  // shapes[0] IS a Circle
        cout << "    Cast to Circle& succeeded: radius = " << c.getRadius() << "\n";

        // shapes[1] is a Rectangle → throws
        Circle& fail = dynamic_cast<Circle&>(*shapes[1]);
        (void)fail;  // never reached
    } catch (const bad_cast& e) {
        cout << "    bad_cast caught: " << e.what() << "\n";
    }

    for (Shape* s : shapes) delete s;
}

// ------------------------------------------------------------------
//  6c. const_cast
//      Only use to call legacy APIs that forgot to mark parameters const.
//      NEVER write through the resulting pointer — undefined behaviour.
// ------------------------------------------------------------------
void demoConstCast() {
    separator("const_cast");

    // Simulate an old C-style function that takes char* instead of const char*
    auto legacyPrint = [](char* str) {
        cout << "    legacy output: " << str << "\n";
    };

    const char* msg = "hello from const string";
    // legacyPrint(msg);                      // error: cannot convert const char* → char*
    legacyPrint(const_cast<char*>(msg));   // OK — we know legacyPrint won't write through it

    cout << "    Safe only because the underlying string is not actually modified.\n";
}

// ------------------------------------------------------------------
//  6d. reinterpret_cast
//      Treats the raw bytes of one type as if they were another type.
//      No safety checks whatsoever — almost never needed in OOP code.
// ------------------------------------------------------------------
void demoReinterpretCast() {
    separator("reinterpret_cast");

    // Inspect the raw bytes of an integer (endianness demo)
    unsigned int n = 0x41424344;   // bytes: 'D','C','B','A' on little-endian
    char* bytes = reinterpret_cast<char*>(&n);

    cout << "  0x41424344 as individual bytes (little-endian order): ";
    for (size_t i = 0; i < sizeof(unsigned int); i++)
        cout << bytes[i];
    cout << "\n";
    cout << "  Correct only if you know exactly what you're doing.\n";
    cout << "  In OOP code, if you reach for reinterpret_cast, reconsider the design.\n";
}

// ------------------------------------------------------------------
//  6e. Exception handling
// ------------------------------------------------------------------
void demoExceptions() {
    separator("Exception Handling");

    // 1. Throw from a constructor — object is never created
    cout << "  1. Throwing from a constructor:\n";
    try {
        Circle bad(-5.0, "red");             // throws InvalidDimensionException
        cout << "    (never printed)\n";
    } catch (const InvalidDimensionException& e) {
        cout << "    InvalidDimensionException: " << e.what()
             << "  (bad value = " << e.getBadValue() << ")\n";
    }

    // 2. Catch by base class type — hierarchy works as expected
    cout << "\n  2. Catching via base class (ShapeException):\n";
    try {
        Triangle bad(1.0, 2.0, 100.0, "blue");  // violates triangle inequality
    } catch (const ShapeException& e) {
        cout << "    ShapeException: " << e.what() << "\n";
    }

    // 3. Order of catch blocks matters — always put derived before base
    cout << "\n  3. Specific catch before general catch:\n";
    try {
        Rectangle bad(-3.0, 5.0, "green");   // throws InvalidDimensionException
    } catch (const InvalidDimensionException& e) {
        cout << "    Matched InvalidDimensionException (most specific)\n";
    } catch (const ShapeException&) {
        cout << "    Matched ShapeException (never reached here)\n";
    } catch (const exception&) {
        cout << "    Matched std::exception (never reached here)\n";
    }

    // 4. Stack unwinding — destructors run even when an exception is thrown
    cout << "\n  4. Stack unwinding (destructor runs during exception propagation):\n";
    {
        struct Guard {
            string name;
            Guard(const string& n) : name(n) {
                cout << "    " << name << " constructed\n";
            }
            ~Guard() {
                cout << "    " << name << " destroyed  ← destructor called by unwinding\n";
            }
        };

        try {
            Guard g("Guard");
            throw ShapeException("something went wrong mid-function");
            cout << "    (never printed)\n";
        } catch (const ShapeException& e) {
            cout << "    Caught: " << e.what() << "\n";
        }
    }
}

// ------------------------------------------------------------------
//  6f. Interfaces
// ------------------------------------------------------------------
void demoInterfaces() {
    separator("Interfaces");

    Circle    c(7.0, "purple");
    Rectangle r(4.0, 3.0, "orange");
    Triangle  t(5.0, 5.0, 6.0, "teal");

    // renderAll only knows about Drawable — it doesn't know these are Shapes
    cout << "  renderAll(Drawable*):\n";
    vector<Drawable*> drawables = { &c, &r, &t };
    renderAll(drawables);

    // saveAll only knows about Serializable
    cout << "\n  saveAll(Serializable*):\n";
    vector<Serializable*> serializables = { &c, &r, &t };
    saveAll(serializables);

    // The same object satisfies three independent contracts at once
    cout << "\n  One Circle object seen through three different lenses:\n";
    Shape*        asShape  = &c;
    Drawable*     asDrawable = &c;
    Serializable* asSerial   = &c;

    cout << "    Shape:        " << *asShape << "\n";
    cout << "    Drawable:     "; asDrawable->draw();
    cout << "    Serializable: " << asSerial->serialize() << "\n";
}

// ------------------------------------------------------------------
//  6g. Polymorphic collection — everything together
// ------------------------------------------------------------------
void demoPolymorphicCollection() {
    separator("Polymorphic Collection (all concepts together)");

    vector<Shape*> shapes = {
        new Circle   (4.0,            "red"   ),
        new Rectangle(3.0,  5.0,      "blue"  ),
        new Circle   (2.5,            "green" ),
        new Triangle (6.0,  8.0, 10.0,"yellow"),
    };

    // Virtual dispatch — correct method called for each concrete type
    double totalArea = 0;
    cout << "  All shapes:\n";
    for (const Shape* s : shapes) {
        cout << "    " << *s << "\n";
        totalArea += s->area();
    }
    cout << "  Total area: " << totalArea << "\n";

    // dynamic_cast to extract type-specific data
    cout << "\n  Circles in the collection:\n";
    for (Shape* s : shapes) {
        Circle* c = dynamic_cast<Circle*>(s);
        if (c)
            cout << "    radius = " << c->getRadius() << "\n";
    }

    // Use Drawable interface to render — dynamic_cast to interface type
    cout << "\n  Rendering all (via Drawable interface):\n";
    for (Shape* s : shapes) {
        Drawable* d = dynamic_cast<Drawable*>(s);
        if (d) d->draw();
    }

    for (Shape* s : shapes) delete s;
}

// ============================================================
//  main
// ============================================================

int main() {
    demoStaticCast();
    demoDynamicCast();
    demoConstCast();
    demoReinterpretCast();
    demoExceptions();
    demoInterfaces();
    demoPolymorphicCollection();
    return 0;
}
