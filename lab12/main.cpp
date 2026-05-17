/*
 * Lab 12 — Design Patterns: index / how to use this lab
 *
 * Each pattern lives in its own self-contained .cpp file with its own
 * main() function. Compile and run them one at a time, for example:
 *
 *     g++ -std=c++17 singleton.cpp        -o singleton        && ./singleton
 *     g++ -std=c++17 factory.cpp          -o factory          && ./factory
 *     g++ -std=c++17 abstract_factory.cpp -o abstract_factory && ./abstract_factory
 *     g++ -std=c++17 adapter.cpp          -o adapter          && ./adapter
 *     g++ -std=c++17 observer.cpp         -o observer         && ./observer
 *     g++ -std=c++17 strategy.cpp         -o strategy         && ./strategy
 *
 * Every file follows the same shape:
 *
 *     namespace without_pattern { ... demo();  }   // the painful way
 *     namespace with_pattern    { ... demo();  }   // the pattern
 *
 *     int main() {
 *         without_pattern::demo();
 *         with_pattern::demo();
 *     }
 *
 * Read the two demos side by side — that contrast is the point of the lab.
 *
 * Patterns covered, grouped by GoF category:
 *
 *   CREATIONAL — "how objects are built"
 *     singleton.cpp        one Logger for the whole program
 *     factory.cpp          create shapes from a string name
 *     abstract_factory.cpp create families of widgets (Windows / macOS)
 *
 *   STRUCTURAL — "how objects fit together"
 *     adapter.cpp          plug an incompatible legacy API into our interface
 *
 *   BEHAVIORAL — "how objects collaborate"
 *     observer.cpp         a Stock notifies many displays when its price changes
 *     strategy.cpp         swap discount-calculation rules at runtime
 *
 * This file itself doesn't run any demo — it just prints the index so
 * you can compile it as a sanity check that the toolchain works.
 */

#include <iostream>

int main() {
    std::cout
        << "Lab 12 — Design Patterns\n"
        << "  See the top of main.cpp for build commands.\n"
        << "  Files: singleton, factory, abstract_factory,\n"
        << "         adapter, observer, strategy.\n";
    return 0;
}
