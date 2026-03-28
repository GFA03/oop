# Lab 4 — Multiple Classes, Composition, and More Operator Overloading

> **Course:** Object-Oriented Programming
> **Topics:** Two interacting classes · Composition (has-a) · Deep copy across classes · More operator overloads (`++`, `+=`, `!`, `==`, `<`) · `std::vector` of pointers · Menu-driven design

---

## Table of Contents

1. [What's new in Lab 4?](#1-whats-new-in-lab-4)
2. [The Two Classes: Animal and Person](#2-the-two-classes-animal-and-person)
3. [Composition — "has-a" vs. "is-a"](#3-composition--has-a-vs-is-a)
4. [Deep Copy Across Class Boundaries](#4-deep-copy-across-class-boundaries)
5. [Additional Operator Overloads](#5-additional-operator-overloads)
   - [Comparison operators (`==` and `<`)](#51-comparison-operators--and-)
   - [Pre-increment and Post-increment (`++`)](#52-pre-increment-and-post-increment-)
   - [Compound assignment (`+=`)](#53-compound-assignment-)
   - [Logical negation (`!`)](#54-logical-negation-)
6. [Managing Collections with `std::vector`](#6-managing-collections-with-stdvector)
7. [The Menu Class — Application Architecture](#7-the-menu-class--application-architecture)
8. [Lab Code Walkthrough](#8-lab-code-walkthrough)
9. [Common Mistakes](#9-common-mistakes)
10. [Quick Recap](#10-quick-recap)

---

## 1. What's new in Lab 4?

Labs 2 and 3 focused on a single class (`BankAccount`) and its internal mechanics. Lab 4 raises the complexity by introducing **two classes that interact with each other** — `Animal` and `Person` — plus a third class (`Menu`) that orchestrates the whole application.

New concepts:

- **Composition** — a `Person` *owns* an `Animal` via a pointer member.
- **Deep copy across classes** — copying a `Person` also copies its `Animal`.
- **More operator overloads** — `==`, `<`, pre-`++`, post-`++`, `+=`, and `!`.
- **`std::vector<T*>`** — a resizable container of pointers for managing dynamic collections.
- **Menu-driven design** — a class whose sole job is to drive user interaction.

---

## 2. The Two Classes: Animal and Person

### Animal

```cpp
class Animal {
    static int noAnimals;
    const int id;
    std::string name;
    std::string breed;
    int noLegs;
public:
    Animal();
    Animal(std::string name, std::string breed, int noLegs);
    Animal(const Animal& obj);
    Animal& operator=(const Animal& obj);
    int getId() const;
    bool operator==(const Animal& other) const;
    bool operator<(const Animal& other) const;
    friend std::istream& operator>>(std::istream&, Animal&);
    friend std::ostream& operator<<(std::ostream&, const Animal&);
};
```

`Animal` uses only `std::string` members — no raw pointers — so its copy constructor and `operator=` don't need to perform manual deep copies. The compiler-generated behavior for `std::string` already does the right thing; only the `const int id` (which must be initialized via the member initializer list) requires attention.

### Person

```cpp
class Person {
    static int noPersons;
    const int id;
    char* name;          // raw pointer → needs manual memory management
    int satisfaction;
    Animal* pet;         // pointer to an Animal → owns it, must deep-copy it

    void copyName(const char* src);
public:
    // ... constructors, destructor, operators ...
};
```

`Person` uses two pointers: `char* name` (a C-string, managed manually) and `Animal* pet` (a dynamically allocated `Animal` object). Both require careful handling in the copy constructor, `operator=`, and destructor — otherwise you get dangling pointers or memory leaks.

---

## 3. Composition — "has-a" vs. "is-a"

Two of the most fundamental relationships between classes are **composition** ("has-a") and **inheritance** ("is-a"). Lab 4 focuses on composition.

A `Person` **has-a** `Animal`. The `Animal` is not a `Person`, and a `Person` is not an `Animal` — they are different things, but a person can own a pet.

In code, composition is expressed by storing a pointer (or value) of one class inside another:

```cpp
class Person {
    Animal* pet;   // Person "has-a" Animal
};
```

**Why a pointer and not a direct member?**

Using `Animal pet;` (a value, not a pointer) would mean every `Person` always has exactly one `Animal` — there would be no way to express "this person has no pet". A pointer can be `nullptr`, making the absence of a pet representable. It also makes ownership transfer and replacement straightforward.

**Ownership:** when `Person` stores `Animal* pet`, the `Person` is responsible for the animal's lifetime. When the `Person` is created, it allocates the `Animal`; when it is destroyed, it deletes the `Animal`. This is called **owning a resource**.

```cpp
Person::~Person() {
    delete[] name;
    delete pet;    // Person is responsible for freeing its Animal
}
```

---

## 4. Deep Copy Across Class Boundaries

When a `Person` is copied, its `Animal` must also be copied — otherwise two `Person` objects would share the same `Animal` in memory, and destroying one would leave the other with a dangling pointer.

### Copy constructor

```cpp
Person::Person(const Person& obj) : id(++noPersons) {
    copyName(obj.name);
    this->satisfaction = obj.satisfaction;
    this->pet = (obj.pet != nullptr) ? new Animal(*obj.pet) : nullptr;
}
```

The key line: `new Animal(*obj.pet)` — this calls `Animal`'s **copy constructor**, allocating a brand-new `Animal` on the heap that is an independent copy of the source animal. Modifying one won't affect the other.

### Assignment operator

```cpp
Person& Person::operator=(const Person& obj) {
    if (this == &obj) return *this;   // self-assignment guard

    delete[] this->name;
    copyName(obj.name);
    this->satisfaction = obj.satisfaction;

    delete this->pet;                                        // free old animal
    this->pet = (obj.pet != nullptr) ? new Animal(*obj.pet) : nullptr;

    return *this;
}
```

`operator=` must:
1. Guard against self-assignment (`this == &obj`).
2. Free the **existing** resources before replacing them (otherwise: memory leak).
3. Deep-copy all pointer members from the source.

### assignPet — replacing the owned Animal

```cpp
void Person::assignPet(Animal* animal) {
    delete this->pet;                                      // free the old pet first
    this->pet = (animal != nullptr) ? new Animal(*animal) : nullptr;
    if (this->pet != nullptr)
        this->satisfaction += 10;
}
```

Notice: the incoming `animal` pointer is not stored directly. Instead, `new Animal(*animal)` creates an independent copy. This means the `Person` is not dependent on the lifetime of the external `Animal*` it was given — it owns its own copy.

---

## 5. Additional Operator Overloads

### 5.1 Comparison operators (`==` and `<`)

Both classes overload `==` and `<` to give objects a natural ordering and equality notion.

**`Animal::operator==`** — two animals are equal if they share the same breed and number of legs:

```cpp
bool Animal::operator==(const Animal& other) const {
    return this->breed == other.breed && this->noLegs == other.noLegs;
}
```

**`Animal::operator<`** — animals are ordered alphabetically by name:

```cpp
bool Animal::operator<(const Animal& other) const {
    return this->name < other.name;
}
```

**`Person::operator==`** — two persons are equal if they have the same name:

```cpp
bool Person::operator==(const Person& other) const {
    return strcmp(this->name, other.name) == 0;
}
```

**`Person::operator<`** — persons are ordered by satisfaction (ascending):

```cpp
bool Person::operator<(const Person& other) const {
    return this->satisfaction < other.satisfaction;
}
```

Once `operator<` is defined, you can use it to implement any sorting algorithm — the `Menu` class uses a simple bubble sort, calling `operator<` explicitly on pointers:

```cpp
if (*animals[j + 1] < *animals[j])
    std::swap(animals[j], animals[j + 1]);
```

Note: `animals[j]` is a pointer, so `*animals[j]` dereferences it to get the `Animal` object, allowing the overloaded `<` to be invoked.

---

### 5.2 Pre-increment and Post-increment (`++`)

Both increment forms are overloaded on `Person` to increase `satisfaction` by one.

**Pre-increment (`++p`)** — increments the object and returns a reference to *itself* (the already-updated object):

```cpp
Person& Person::operator++() {
    this->satisfaction++;
    return *this;
}
```

**Post-increment (`p++`)** — saves a copy of the current state, increments, then returns the *old* copy:

```cpp
Person Person::operator++(int) {   // the dummy int parameter distinguishes post from pre
    Person copy = *this;
    this->satisfaction++;
    return copy;
}
```

The difference in use:

```cpp
Person& updated = ++p;   // updated IS p, already incremented
Person before   = p++;   // before is a copy of p BEFORE the increment
```

The `Menu` demonstrates both forms side by side (options 5 and 6 in the people menu) so you can see the difference in what they return.

**Why does post-increment return by value and pre-increment by reference?**

Post-increment *must* return a copy (the old state), so it cannot return a reference — the local `copy` variable would be destroyed when the function returns. Pre-increment modifies and returns `*this`, which outlives the function call, so a reference is safe and avoids an unnecessary copy.

---

### 5.3 Compound assignment (`+=`)

`operator+=` with an `Animal*` argument assigns a pet to the person — delegating to `assignPet`:

```cpp
Person& Person::operator+=(Animal* animal) {
    assignPet(animal);
    return *this;
}
```

This lets you write:

```cpp
person += &someAnimal;   // same effect as person.assignPet(&someAnimal)
```

The operator is a thin wrapper; the actual logic lives in `assignPet`. This keeps the code DRY — there is only one place where the "assign pet" logic is implemented.

---

### 5.4 Logical negation (`!`)

`operator!` returns `true` if the person has no pet — a natural reading: "not a pet owner":

```cpp
bool Person::operator!() const {
    return this->pet == nullptr;
}
```

Usage:

```cpp
if (!person)
    std::cout << "No pet.\n";
```

This is a unary operator (takes no arguments beyond `this`) and is declared `const` because checking for a pet does not modify the object.

---

## 6. Managing Collections with `std::vector`

The `Menu` class stores the animals and persons in two `std::vector`s of pointers:

```cpp
class Menu {
    std::vector<Animal*> animals;
    std::vector<Person*> persons;
};
```

### Why `vector` of pointers?

A `std::vector<Animal>` (values) would work, but using pointers offers:
- **Identity stability** — the object's address doesn't change when the vector resizes.
- **Polymorphism readiness** — pointer containers are needed when objects may be of derived types (relevant in future labs with inheritance).
- **Explicit ownership control** — you decide when to `new` and `delete`.

The trade-off: you must manually `delete` each pointer. The destructor does this:

```cpp
Menu::~Menu() {
    for (size_t i = 0; i < animals.size(); i++) delete animals[i];
    for (size_t i = 0; i < persons.size(); i++) delete persons[i];
}
```

### Common `vector` operations used

| Operation | Code | Effect |
|---|---|---|
| Add element | `animals.push_back(a)` | Appends `a` to the end |
| Access by index | `animals[i]` | Returns element at position `i` |
| Get size | `animals.size()` | Returns number of elements (`size_t`) |
| Check if empty | `animals.empty()` | Returns `true` if no elements |
| Remove by index | `animals.erase(animals.begin() + idx)` | Removes element at `idx`, shifts remainder left |

---

## 7. The Menu Class — Application Architecture

The `Menu` class separates **user interaction** from **business logic**. `Animal` and `Person` know nothing about menus or console I/O (beyond their own `operator<<`/`>>`); `Menu` knows how to present choices and dispatch operations.

```cpp
class Menu {
    std::vector<Animal*> animals;
    std::vector<Person*> persons;

    void printAnimals() const;
    void printPersons() const;
    int  pickAnimal() const;
    int  pickPerson() const;

public:
    Menu() = default;
    ~Menu();
    void run();
    void animalMenu();
    void peopleMenu();
    void assignMenu();
};
```

`Menu() = default;` — this tells the compiler to generate the default constructor automatically. It's equivalent to writing `Menu() {}` but signals intent explicitly.

The menu hierarchy:

```
run()           → main loop (Exit / Animals / People / Assign)
 ├── animalMenu()  → CRUD + compare + sort for animals
 ├── peopleMenu()  → CRUD + ++/+=/!/==/< for persons
 └── assignMenu()  → assign / remove pet
```

Each sub-menu runs in its own `while (true)` loop, returning when the user selects `0 - Back`. This pattern makes it easy to add new options without restructuring the whole program.

---

## 8. Lab Code Walkthrough

### Creating an Animal

```cpp
Animal* a = new Animal();
std::cin >> *a;
animals.push_back(a);
```

- `new Animal()` calls the default constructor; `a` is a pointer to the heap-allocated object.
- `std::cin >> *a` dereferences the pointer and calls the overloaded `operator>>`.
- `push_back(a)` stores the pointer in the vector (not a copy of the `Animal`).

---

### Assigning a pet — two syntaxes

Option 1 via method:
```cpp
persons[pidx]->assignPet(animals[aidx]);
```

Option 2 via operator:
```cpp
*persons[pidx] += animals[aidx];
```

Both end up calling `assignPet`, which deep-copies the `Animal`. The person's satisfaction increases by 10 in both cases.

---

### Removing an animal (and its effect on persons)

```cpp
for (size_t i = 0; i < persons.size(); i++)
    persons[i]->removePet();
delete animals[idx];
animals.erase(animals.begin() + idx);
```

Before the `Animal` is deleted from the collection, every `Person`'s pet is removed. This prevents persons from holding a `Person::pet` that points to a now-deleted `Animal` — a dangling pointer. Because each `Person` owns its *own copy* of the `Animal` (deep copy), `removePet` can delete it independently:

```cpp
void Person::removePet() {
    delete this->pet;
    this->pet = nullptr;
}
```

---

### Sorting

```cpp
// Bubble sort — animals by name (operator<)
for (size_t i = 0; i < animals.size(); i++)
    for (size_t j = 0; j + 1 < animals.size() - i; j++)
        if (*animals[j + 1] < *animals[j])
            std::swap(animals[j], animals[j + 1]);
```

`std::swap` swaps the **pointers** inside the vector (cheap — just two pointer assignments). The objects themselves stay in place on the heap.

---

## 9. Common Mistakes

| Mistake | Description | Fix |
|---|---|---|
| Shallow copy of `Animal*` | Storing the raw pointer instead of `new Animal(*src)` | Always deep-copy owned pointer members |
| Forgetting to `delete pet` in `operator=` | The old pet leaks before the new one is assigned | Free old resources before allocating new ones |
| `delete` instead of `delete[]` | Using `delete` on `char* name` allocated with `new char[n]` | Use `delete[]` for anything allocated with `new[]` |
| Missing self-assignment guard | `x = x;` frees data then tries to copy from the freed area | Check `if (this == &obj)` at the top of `operator=` |
| Returning a reference to a local in post-`++` | The local copy is destroyed on return | Post-`++` must return **by value**, not by reference |
| Storing the passed `Animal*` directly | The person would depend on the caller's animal lifetime | Always copy: `new Animal(*animal)` |
| Not deleting vector elements in destructor | Pointers in a `vector` are NOT automatically freed | Loop and `delete` each pointer before the `vector` is destroyed |

---

## 10. Quick Recap

| Concept | What it is | Where it appears in code |
|---|---|---|
| **Composition** | A class contains another class as an owned member | `Animal* pet` inside `Person` |
| **Owning pointer** | A pointer the class is responsible for allocating and freeing | `pet`, `name` in `Person` |
| **Deep copy across classes** | Copying a `Person` also copies its `Animal` via `new Animal(*src)` | Copy constructor & `operator=` of `Person` |
| **`operator==`** | Custom equality — what "same" means for the type | Animal: same breed+legs; Person: same name |
| **`operator<`** | Custom ordering — enables sorting | Animal: alphabetical by name; Person: by satisfaction |
| **Pre-increment `++p`** | Increments and returns reference to updated self | `Person& operator++()` |
| **Post-increment `p++`** | Saves copy, increments, returns old copy by value | `Person operator++(int)` |
| **`operator+=`** | Compound assignment; delegates to `assignPet` | `*person += animal;` |
| **`operator!`** | Unary negation; true when person has no pet | `if (!person)` |
| **`std::vector<T*>`** | Resizable container of pointers; requires manual `delete` | `animals`, `persons` in `Menu` |
| **Menu class** | Separates user interaction from class logic | `run()`, `animalMenu()`, `peopleMenu()`, `assignMenu()` |
| **`= default`** | Asks the compiler to generate the default constructor | `Menu() = default;` |
