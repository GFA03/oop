# Lab 2 — Classes, Objects, and the Object Lifecycle

## Table of Contents

- [What is Object-Oriented Programming (OOP)?](#what-is-object-oriented-programming-oop)
- [Class, Object, Instance — what's the difference?](#class-object-instance--whats-the-difference)
- [Anatomy of a C++ Class](#anatomy-of-a-c-class)
- [Access Specifiers: `private` and `public`](#access-specifiers-private-and-public)
- [Static Members (`static`)](#static-members-static)
- [Constant Members (`const`)](#constant-members-const)
- [Constructors](#constructors)
  - [The Default Constructor](#the-default-constructor)
  - [Parameterized Constructors](#parameterized-constructors)
  - [The Copy Constructor](#the-copy-constructor)
  - [The Member Initializer List](#the-member-initializer-list)
- [The Destructor](#the-destructor)
- [The Assignment Operator (`operator=`)](#the-assignment-operator-operator)
- [The Scope Resolution Operator (`::`)](#the-scope-resolution-operator-)
- [Declaration vs. Implementation — why do we separate them?](#declaration-vs-implementation--why-do-we-separate-them)
- [The `this` Pointer](#the-this-pointer)

---

## What is Object-Oriented Programming (OOP)?

Up until now, in high school, you've been writing **procedural** code: a sequence of instructions executed one after another, with functions that operate on data passed in as parameters. Data and the functions that process it live separately — functions receive data, modify it, return it.

**Object-Oriented Programming** completely changes the way you think. Instead of separating data from functions, we group them together into a single unit called a **class**. The fundamental idea is this: in the real world, things have both **properties** (what they are) and **behaviors** (what they do). A bank account has a balance, a holder, a name — but it can also receive deposits, display information, etc. OOP lets us model these real-world entities directly in code.

The four fundamental principles of OOP are **encapsulation**, **abstraction**, **inheritance**, and **polymorphism**. In this lab we focus on the first two; the others will be explored in future labs.

---

## Class, Object, Instance — what's the difference?

These three terms are essential and are often confused by beginners.

A **class** is a **blueprint** (a "recipe", a "technical plan"). It describes what properties and behaviors something will have, but it is not that thing itself. Think of the `BankAccount` class as the blank form you fill out to open a bank account — it describes what information needs to be provided (name, balance, account type), but it doesn't represent anyone's actual account yet.

```cpp
class BankAccount {
private:
    char* name;
    double balance;
public:
    void print();
};
```

An **object** is a **concrete realization** of that blueprint. When you declare a variable of a class type, you create an object. Now the form is filled in — we have a real account, with real data.

```cpp
BankAccount x;  // x is an object of type BankAccount
```

An **instance** is, in practice, **synonymous with object**. When we say "`x` is an instance of the class `BankAccount`", we're saying the same thing as "`x` is an object of type `BankAccount`". The term "instance" emphasizes the relationship with the class it came from — as if you were saying "this specific account was created from the BankAccount blueprint". The term "object" puts the emphasis on the entity itself.

In one sentence: the **class** is the blueprint, the **object/instance** is the house built from that blueprint. You can build 100 houses (objects) from the same blueprint (class) — each has its own rooms furnished differently (different data), but they all follow the same structure.

---

## Anatomy of a C++ Class

A class in C++ is declared with the keyword `class`, followed by the class name and a block delimited by curly braces, ending with `;`:

```cpp
class BankAccount {
private:
    // data members (attributes) — CANNOT be accessed from outside
    static int noAccounts;
    const int id;
    char* name;
    std::string accountHolder;
    double balance;
    bool isInvestmentAccount;
    int noDeposits;
    int* deposits;

public:
    // member functions (methods) — CAN be called from outside
    BankAccount();
    BankAccount(char*, std::string);
    BankAccount(char*, std::string, double, bool, int, int*);
    BankAccount(const BankAccount &obj);
    BankAccount& operator=(const BankAccount &obj);
    ~BankAccount();

    void print();
};
```

Notice the structure: the `private` section contains the **data** (what an account "knows"), and the `public` section contains the **interface** (what an account "can do", what is visible from outside). This separation is called **encapsulation** — we hide the internal details and expose only the permitted operations.

---

## Access Specifiers: `private` and `public`

Inside a class, every member (data or function) has an **access level**:

**`private`** means the member can only be accessed **from inside the class** — that is, only from the class's own member functions. Code in `main()` or in other classes **cannot** directly read or modify a private member. If you try, the compiler gives an error. In C++, if you don't specify anything, class members are `private` by default.

**`public`** means the member can be accessed from **anywhere** — from `main()`, from other functions, from other classes.

Why not make everything `public`? For the same reason an ATM doesn't give you direct access to the bank's vault. You want the balance to be modifiable only through controlled operations (deposit, withdrawal), not through direct assignment. If someone were to write `account.balance = -999999;` from `main()`, it would break the program's logic. Encapsulation prevents this.

```cpp
BankAccount x;
// x.balance = 1000;    // ERROR! balance is private
x.print();              // OK — print() is public
```

---

## Static Members (`static`)

Look at this line in the class:

```cpp
static int noAccounts;
```

A **`static`** member does not belong to any individual object — it belongs to **the class itself**. There is only one copy of `noAccounts` shared by all `BankAccount` objects. No matter how many accounts you create, `noAccounts` exists only once in memory.

A static member is initialized **outside the class**, exactly once:

```cpp
int BankAccount::noAccounts = 1;
```

Why is this useful? In our case, `noAccounts` works as a global account counter. Every time a new object is created, the constructor reads the current value of `noAccounts`, assigns it to the object's `id` field, and then increments the counter. This way, every account receives a unique ID automatically, without the programmer specifying it manually.

Think of `static` as a whiteboard on the classroom wall — all students (objects) see it and share it, but it doesn't belong to any one of them in particular.

---

## Constant Members (`const`)

```cpp
const int id;
```

A member declared `const` **cannot be modified** after it has been initialized. Once an account receives ID 3, that ID stays 3 for the entire lifetime of the object. This reflects reality — an identification code doesn't change.

`const` members **must** be initialized through the constructor's **member initializer list** (see the dedicated section below). You cannot write `id = value;` in the constructor body — that would be an assignment, not an initialization, and the compiler does not allow that for `const`.

---

## Constructors

A **constructor** is a special function that is called **automatically** when an object is created. Its purpose is to **initialize** the object — to give all its members correct starting values. Without a constructor, a class's members would contain "garbage" (residual values left in memory).

Constructors have a few characteristics that distinguish them from regular functions: they have the **same name** as the class, they have **no return type** (not even `void`), and they can be **overloaded** — meaning you can have multiple constructors with different signatures.

### The Default Constructor

```cpp
BankAccount::BankAccount() : id(noAccounts++) {
    name = strcpy(new char[4], "N/A");
    accountHolder = "N/A";
    balance = 0.0;
    isInvestmentAccount = false;
    noDeposits = 0;
    deposits = nullptr;
}
```

This is the **parameterless constructor** (or "default constructor"). It is called when you create an object without providing any arguments: `BankAccount x;`. It sets "neutral" values for all fields: balance 0, no deposits, name "N/A".

Notice `strcpy(new char[4], "N/A")` — memory is dynamically allocated for a string of 3 characters + the null terminator `'\0'`, then the text "N/A" is copied into that area. Working with `char*` (C-strings) requires manual memory management, which is why the destructor is also important.

### Parameterized Constructors

```cpp
BankAccount::BankAccount(char* name, std::string accountHolder) : id(noAccounts++) {
    this->name = strcpy(new char[strlen(name) + 1], name);
    this->accountHolder = accountHolder;
    balance = 0.0;
    isInvestmentAccount = false;
    noDeposits = 0;
    deposits = nullptr;
}
```

This constructor takes only the account name and holder; the rest remain at default values. There is also a third constructor that takes **all** parameters:

```cpp
BankAccount::BankAccount(char* name, std::string accountHolder, double balance,
    bool isInvestmentAccount, int noDeposits, int* deposits) : id(noAccounts++) {
    // ... copies each value ...
}
```

Notice how the `deposits` array is **copied element by element** — the pointer itself is not copied; instead, a new array is allocated and the values are copied over. This is a **deep copy**. If we had only copied the pointer (`this->deposits = deposits;`), both objects would point to the same area of memory — modifying one would affect the other, and deleting one would invalidate the other's data.

### The Copy Constructor

```cpp
BankAccount::BankAccount(const BankAccount &obj) : id(noAccounts++) {
    this->name = strcpy(new char[strlen(obj.name) + 1], obj.name);
    this->accountHolder = obj.accountHolder;
    this->balance = obj.balance;
    this->isInvestmentAccount = obj.isInvestmentAccount;
    this->noDeposits = obj.noDeposits;
    if (noDeposits > 0) {
        this->deposits = new int[obj.noDeposits];
        for (int i = 0; i < obj.noDeposits; i++) {
            this->deposits[i] = obj.deposits[i];
        }
    } else {
        this->deposits = nullptr;
    }
}
```

The **copy constructor** is called when you create a new object as a copy of an existing one:

```cpp
BankAccount a("Account", "Ion", 500.0, false, 2, deps);
BankAccount b(a);   // calls the copy constructor
BankAccount c = a;  // this also calls the copy constructor (NOT operator=)
```

Its signature is always of the form `ClassName(const ClassName &object)`. The parameter is a **const reference** to the source object — a reference so that a copy isn't made (which would call the copy constructor again, creating infinite recursion), and `const` because we don't intend to modify the source.

Why is it necessary? If the class contains **pointers** (like `name` or `deposits`), the compiler-generated copy constructor only performs a **shallow copy** — it copies the pointer value, not the data it points to. The result: two objects pointing to the same memory. When the first object is destroyed, the memory is freed, and the second object is left with an invalid pointer (dangling pointer). That's why we write our own copy constructor to perform a **deep copy**.

### The Member Initializer List

```cpp
BankAccount::BankAccount() : id(noAccounts++) {
```

The text `: id(noAccounts++)` after the constructor signature is the **member initializer list**. It initializes members **before** the constructor body begins executing. It is **mandatory** for `const` members (which cannot be assigned to later) and for reference-type members.

The difference from assignment in the constructor body: the initializer list **constructs** the member directly with the given value, while assignment in the body means the member was already constructed with a default value and then overwritten — an extra step, and sometimes impossible (the `const` case).

---

## The Destructor

```cpp
BankAccount::~BankAccount() {
    if (name != nullptr) {
        delete[] name;
        name = nullptr;
    }
    if (deposits != nullptr) {
        delete[] deposits;
        deposits = nullptr;
    }
}
```

The **destructor** is the function that is called **automatically** when an object is destroyed — either when it goes out of scope, or when `delete` is called on a dynamically allocated object. Its signature is `~ClassName()`. It takes no parameters and returns nothing.

Its role: to **release the resources** the object acquired during its lifetime. In our case, the class dynamically allocated memory (with `new[]`) for `name` and `deposits` — if we don't free that memory with `delete[]`, we have a **memory leak**: the program consumes more and more memory without ever freeing it.

Setting pointers to `nullptr` after freeing is a good safety practice — it prevents a situation where someone might accidentally access already-freed memory (double-free or use-after-free).

---

## The Assignment Operator (`operator=`)

```cpp
BankAccount& BankAccount::operator=(const BankAccount &obj) {
    if (this == &obj) {
        return *this;
    }

    delete[] this->name;
    this->name = new char[strlen(obj.name) + 1];
    strcpy(this->name, obj.name);

    this->accountHolder = obj.accountHolder;
    this->balance = obj.balance;
    this->isInvestmentAccount = obj.isInvestmentAccount;
    this->noDeposits = obj.noDeposits;

    delete[] this->deposits;
    if (obj.noDeposits > 0) {
        this->deposits = new int[obj.noDeposits];
        for (int i = 0; i < obj.noDeposits; i++) {
            this->deposits[i] = obj.deposits[i];
        }
    } else {
        this->deposits = nullptr;
    }
    return *this;
}
```

The **assignment operator** is called when you assign an **already existing** object to another already existing object:

```cpp
BankAccount a, b;
b = a;  // operator= (b already exists, it's not being constructed now)
```

**Pay attention to the difference from the copy constructor:**

```cpp
BankAccount b(a);   // copy constructor — b is being created now
BankAccount b = a;  // also copy constructor — b is being created now
b = a;              // operator= — b already existed, it's being assigned a's values
```

The assignment operator must do three things that the copy constructor does not:

1. **Self-assignment check** (`if (this == &obj)`) — if someone writes `x = x;`, we don't want to delete the data and then try to copy from the just-emptied object.
2. **Free the old memory** — the destination object already had dynamically allocated data; it must be freed before allocating new memory, otherwise we lose the reference to it (memory leak).
3. **Return `*this`** — this enables **chaining** of assignments: `a = b = c;`.

---

## The Scope Resolution Operator (`::`)

The `::` operator (scope resolution operator) tells the compiler **who owns** a particular name. We use it in several situations:

**1. Implementing member functions outside the class:**

```cpp
void BankAccount::print() {
    // ...
}
```

Here `BankAccount::print()` means "the `print` function that belongs to the `BankAccount` class". Without `BankAccount::`, the compiler would think `print` is a global function, independent of any class.

**2. Initializing static members:**

```cpp
int BankAccount::noAccounts = 1;
```

Static members are declared inside the class, but must be **defined** (have memory allocated and a value assigned) outside the class, using `::`.

---

## Declaration vs. Implementation — why do we separate them?

In the lab code, notice that inside the class only the **declaration** (signature) of functions appears:

```cpp
class BankAccount {
public:
    void print();           // declaration only — we say WHAT exists
};
```

While the **implementation** (the function body, the actual code) appears outside the class:

```cpp
void BankAccount::print() {     // implementation — we say HOW it works
    std::cout << "Id: " << id << '\n';
    // ...
}
```

Why not write everything directly inside the class? There are several reasons:

**Readability:** When a class has 15 functions, you want to quickly see what functions it offers (the interface) without getting lost in hundreds of lines of implementation. The declarations inside the class work like a "table of contents" — you can see at a glance everything a `BankAccount` can do.

**Code organization in real projects:** In practice, the class declaration goes in a **header file** (`.h` or `.hpp`), and the implementation goes in a **source file** (`.cpp`). Other files in the project include only the header — they know what functions exist without seeing the implementation. This reduces compilation times and hides internal details.

**Inline vs. non-inline:** Functions defined **directly inside the class** are implicitly `inline` (the compiler may replace the function call with its body, for efficiency). Functions defined **outside the class** are not `inline` by default. For short functions (1-2 lines), defining them inside the class is reasonable. For complex functions, separate implementation is the rule.

**Note:** Both approaches produce correct code. The choice is one of **style, organization, and maintainability**, not correctness.

---

## The `this` Pointer

The lab code frequently uses `this->`:

```cpp
this->name = strcpy(new char[strlen(name) + 1], name);
this->accountHolder = accountHolder;
```

**`this`** is an implicit pointer available in any non-static member function. It **points to the current object** — that is, the object on which the function was called.

Why is it needed here? The constructor parameter is named `name`, and the class member is also named `name`. Without `this->`, the compiler would use the local variable (the parameter) in both cases. By writing `this->name`, we explicitly specify: "the `name` member of the current object, not the function parameter".

`this` is also used in the assignment operator:

```cpp
if (this == &obj) { return *this; }  // self-assignment check
return *this;                         // return current object
```

`this` is a pointer, so `*this` is the object itself (dereference).

---

## Quick Recap

| Concept | What it is | Where it appears in code |
|---|---|---|
| **Class** | Blueprint describing properties + behaviors | `class BankAccount { ... };` |
| **Object / Instance** | A concrete realization of the class, with its own data | `BankAccount x, y(...);` |
| **`private` / `public`** | Access control for class members | Sections inside the class |
| **`static`** | Member shared by all objects, not owned by any single one | `static int noAccounts;` |
| **`const`** | Member that cannot be modified after initialization | `const int id;` |
| **Constructor** | Function called automatically when an object is created | `BankAccount()`, `BankAccount(char*, ...)` |
| **Copy constructor** | Constructor that creates an object as a copy of another | `BankAccount(const BankAccount &obj)` |
| **Destructor** | Function called automatically when an object is destroyed | `~BankAccount()` |
| **`operator=`** | Overrides assignment to perform deep copy | `BankAccount& operator=(const BankAccount &obj)` |
| **`::`** | Tells the compiler who owns a name | `BankAccount::print()` |
| **`this`** | Pointer to the current object | `this->name = ...` |
| **Initializer list** | Initialization before the constructor body | `: id(noAccounts++)` |
| **Declaration vs. implementation** | Separating "what it does" from "how it does it" | Signature in class, body outside the class |
