/*
 * Lab 12 — The SINGLETON Pattern
 *
 * Problem: We need exactly ONE Logger shared by the whole program.
 *   - Every part of the code should write to the same log.
 *   - Creating a second logger by accident would split the output
 *     across two places and confuse debugging.
 *
 * We will solve the problem twice:
 *   1) WITHOUT the pattern — using globals / passing the logger around
 *   2) WITH the Singleton pattern — a class that enforces "only one"
 */

#include <iostream>
#include <string>

using namespace std;

// ============================================================
//  Helper to print section headers
// ============================================================
void section(const string& title) {
    cout << "\n══════════════════════════════════════════\n";
    cout << "  " << title << "\n";
    cout << "══════════════════════════════════════════\n";
}

// ============================================================
//  1. WITHOUT the pattern
//
//  Approach A: a plain global variable.
//    - Anyone can construct a SECOND Logger. Nothing stops them.
//    - Initialization order across translation units is undefined,
//      so the global may not be ready when other globals use it.
//
//  Approach B: pass a Logger& into every function that needs it.
//    - Pollutes every signature in the codebase.
//    - One forgotten parameter and you silently get a different log.
// ============================================================

namespace without_pattern {

class Logger {
    string name;
public:
    Logger(const string& n) : name(n) {
        cout << "    [+] Logger '" << name << "' constructed\n";
    }
    void log(const string& msg) {
        cout << "    [" << name << "] " << msg << "\n";
    }
};

// Global instance — looks convenient, but nothing prevents duplicates.
Logger globalLogger("global");

void moduleA() {
    globalLogger.log("module A doing work");
}

void moduleB() {
    // Oops. Someone in another file made a second logger.
    // Now half the program logs to "global" and half to "rogue".
    Logger rogue("rogue");
    rogue.log("module B doing work");
}

// Alternative: pass the logger explicitly. Verbose, but at least typed.
void moduleC(Logger& log) {
    log.log("module C doing work");
}

void demo() {
    section("1. WITHOUT Singleton — globals and parameter passing");

    moduleA();
    moduleB();
    cout << "  Notice: module B accidentally created its own logger.\n";
    cout << "  Nothing in the type system prevented that.\n\n";

    cout << "  Passing the logger by reference works, but every\n";
    cout << "  function in the call chain needs a Logger& parameter:\n";
    moduleC(globalLogger);
}

} // namespace without_pattern

// ============================================================
//  2. WITH the Singleton pattern
//
//  Rules of the pattern:
//    - Private constructor          → outsiders can't call `new Logger`
//    - Deleted copy / assignment    → no duplicates by copying
//    - Static getInstance()         → the ONE access point
//
//  The "Meyers Singleton" below uses a function-local static. The
//  C++ standard guarantees it is constructed exactly once, in a
//  thread-safe way, on first use.
// ============================================================

namespace with_pattern {

class Logger {
private:
    Logger() {
        cout << "    [+] THE Logger constructed (only ever once)\n";
    }

public:
    Logger(const Logger&)            = delete;
    Logger& operator=(const Logger&) = delete;

    static Logger& getInstance() {
        static Logger instance;     // constructed on first call, never again
        return instance;
    }

    void log(const string& msg) {
        cout << "    [LOG] " << msg << "\n";
    }
};

void moduleA() {
    Logger::getInstance().log("module A doing work");
}

void moduleB() {
    Logger::getInstance().log("module B doing work");
    // There is no way to write `Logger rogue;` — the constructor is private.
}

void moduleC() {
    Logger::getInstance().log("module C doing work");
}

void demo() {
    section("2. WITH Singleton — one logger, guaranteed");

    moduleA();
    moduleB();
    moduleC();

    Logger& a = Logger::getInstance();
    Logger& b = Logger::getInstance();
    cout << "  Two calls to getInstance() return the SAME object: "
         << (&a == &b ? "yes" : "no") << "\n";

    // Logger rogue;          // compile error: constructor is private
    // Logger copy = a;       // compile error: copy is deleted
}

} // namespace with_pattern

// ============================================================
//  Why the Singleton version is better
//   - The "only one" rule is enforced by the COMPILER, not by hope.
//   - No global variable to fight with at startup.
//   - No Logger& parameter polluting every function signature.
//   - Lazy: the Logger is built the first time someone asks for it.
//
//  Caveats worth telling students:
//   - Singletons are global state in disguise. Overuse makes code
//     hard to test (you can't swap in a fake logger easily).
//   - Use them for genuinely-unique resources (config, log sink,
//     connection pool), not as a shortcut around dependency passing.
// ============================================================

int main() {
    without_pattern::demo();
    with_pattern::demo();
    return 0;
}
