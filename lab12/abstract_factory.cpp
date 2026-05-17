/*
 * Lab 12 — The ABSTRACT FACTORY Pattern
 *
 * Problem: We are building a cross-platform GUI. Each platform
 *   (Windows, macOS) has its own Button and its own Checkbox, and
 *   they MUST be used together — a WindowsButton next to a MacCheckbox
 *   would look broken and would not interact correctly with the OS.
 *
 *   - The client code wants to draw a "form" without caring which OS.
 *   - It must be impossible to accidentally mix widgets from different
 *     platform families.
 *
 * We will solve the problem twice:
 *   1) WITHOUT the pattern — if-statements over the platform string,
 *      sprinkled in every place that creates a widget
 *   2) WITH the Abstract Factory pattern — a family of related products
 *      is created by a single factory object, picked once at startup
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
//  Product hierarchies — Button and Checkbox, two per platform
// ============================================================
class Button {
public:
    virtual ~Button() = default;
    virtual void render() const = 0;
};
class Checkbox {
public:
    virtual ~Checkbox() = default;
    virtual void render() const = 0;
};

class WinButton    : public Button   { public: void render() const override { cout << "    [Windows] Button\n";   } };
class WinCheckbox  : public Checkbox { public: void render() const override { cout << "    [Windows] Checkbox\n"; } };
class MacButton    : public Button   { public: void render() const override { cout << "    [macOS]   Button\n";   } };
class MacCheckbox  : public Checkbox { public: void render() const override { cout << "    [macOS]   Checkbox\n"; } };

// ============================================================
//  1. WITHOUT the pattern
//
//  Every place that builds a widget needs to ask "which platform?"
//  and pick the matching class. Two problems:
//    - The platform check is duplicated everywhere.
//    - Nothing prevents a WindowsButton from being placed next to a
//      MacCheckbox if a programmer types the wrong string.
// ============================================================

namespace without_pattern {

void renderLoginForm(const string& platform) {
    Button*   btn = nullptr;
    Checkbox* cb  = nullptr;

    if (platform == "windows") {
        btn = new WinButton();
        cb  = new WinCheckbox();
    } else if (platform == "macos") {
        btn = new MacButton();
        cb  = new MacCheckbox();
    } else {
        throw runtime_error("unknown platform");
    }

    btn->render();
    cb->render();

    delete btn;
    delete cb;
}

void renderSettingsPage(const string& platform) {
    // Same family-selection logic, copy-pasted again.
    // And nothing stops a developer from writing the wrong combination:
    Button*   btn = (platform == "windows") ? (Button*)new WinButton()   : (Button*)new MacButton();
    Checkbox* cb  = (platform == "windows") ? (Checkbox*)new MacCheckbox() // BUG — wrong family!
                                            : (Checkbox*)new MacCheckbox();
    btn->render();
    cb->render();
    delete btn;
    delete cb;
}

void demo() {
    section("1. WITHOUT Abstract Factory — easy to mix families");

    cout << "  Login form on Windows:\n";
    renderLoginForm("windows");

    cout << "\n  Settings page (with the bug):\n";
    renderSettingsPage("windows");
    cout << "  Oops — a Windows button next to a Mac checkbox.\n";
    cout << "  The type system did nothing to stop us.\n";
}

} // namespace without_pattern

// ============================================================
//  2. WITH the Abstract Factory pattern
//
//  Define one interface, GUIFactory, that knows how to create a
//  whole FAMILY of related products. Each platform has its own
//  concrete factory that returns its own widgets. Client code is
//  written against GUIFactory only — it never names a concrete
//  platform class.
//
//  Mixing families becomes impossible: a single factory only ever
//  returns one platform's widgets.
// ============================================================

namespace with_pattern {

class GUIFactory {
public:
    virtual ~GUIFactory() = default;
    virtual unique_ptr<Button>   createButton()   const = 0;
    virtual unique_ptr<Checkbox> createCheckbox() const = 0;
};

class WinFactory : public GUIFactory {
public:
    unique_ptr<Button>   createButton()   const override { return make_unique<WinButton>(); }
    unique_ptr<Checkbox> createCheckbox() const override { return make_unique<WinCheckbox>(); }
};

class MacFactory : public GUIFactory {
public:
    unique_ptr<Button>   createButton()   const override { return make_unique<MacButton>(); }
    unique_ptr<Checkbox> createCheckbox() const override { return make_unique<MacCheckbox>(); }
};

// Client code — works for ANY platform. No if/else.
void renderLoginForm(const GUIFactory& gui) {
    auto btn = gui.createButton();
    auto cb  = gui.createCheckbox();
    btn->render();
    cb->render();
}

void renderSettingsPage(const GUIFactory& gui) {
    auto btn = gui.createButton();
    auto cb  = gui.createCheckbox();
    btn->render();
    cb->render();
}

unique_ptr<GUIFactory> pickFactory(const string& platform) {
    if (platform == "windows") return make_unique<WinFactory>();
    if (platform == "macos")   return make_unique<MacFactory>();
    throw runtime_error("unknown platform");
}

void demo() {
    section("2. WITH Abstract Factory — families stay together");

    auto gui = pickFactory("windows");

    cout << "  Login form on Windows:\n";
    renderLoginForm(*gui);

    cout << "\n  Settings page on Windows:\n";
    renderSettingsPage(*gui);
    cout << "  Both widgets are Windows widgets. Impossible to mix\n";
    cout << "  them up — the WinFactory only knows how to build WinXxx.\n";

    cout << "\n  Same client code, macOS family:\n";
    auto macGui = pickFactory("macos");
    renderLoginForm(*macGui);
}

} // namespace with_pattern

// ============================================================
//  Why the Abstract Factory version is better
//   - The "which platform?" decision happens ONCE, at startup.
//   - Client code accepts a GUIFactory& — fully platform-agnostic.
//   - Families can't be mixed: a WinFactory can't return a MacButton.
//   - Adding a Linux family means: write LinFactory + LinButton +
//     LinCheckbox. NO existing client code changes.
// ============================================================

int main() {
    without_pattern::demo();
    with_pattern::demo();
    return 0;
}
