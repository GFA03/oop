# Lab 3 — Operator Overloading, Friend Functions, and Methods

## Table of Contents

- [What's new in Lab 3?](#whats-new-in-lab-3)
- [Operator Overloading — the general idea](#operator-overloading--the-general-idea)
- [Friend Functions (`friend`)](#friend-functions-friend)
- [Overloading `operator<<` (output)](#overloading-operator-output)
- [Overloading `operator>>` (input)](#overloading-operator-input)
- [Behavioral Methods](#behavioral-methods)
  - [`deposit` and `withdraw`](#deposit-and-withdraw)
  - [`addDeposit`](#adddeposit)
- [Private Helper Methods](#private-helper-methods)
- [Pre-increment vs. Post-increment (`++i` vs. `i++`)](#pre-increment-vs-post-increment-i-vs-i)
- [Quick Recap](#quick-recap)

---

## What's new in Lab 3?

Lab 2 introduced the core structure of a C++ class: access specifiers, static and const members, constructors, the destructor, and `operator=`. Lab 3 builds on that foundation by adding:

- **Operator overloading** — giving your own meaning to built-in operators like `<<` and `>>` for your class.
- **Friend functions** — non-member functions that are granted access to `private` members.
- **Behavioral methods** — functions that implement real-world operations (`deposit`, `withdraw`, `addDeposit`).
- **Private helper methods** — internal utilities that reduce code duplication inside the class.
- **Pre-increment vs. post-increment** — a subtle but important distinction in how `++` works.

---

## Operator Overloading — the general idea

In C++, operators like `+`, `=`, `<<`, and `>>` are just functions with special syntax. The expression `a + b` is, behind the scenes, a call to `operator+(a, b)`. Because they're functions, you can **overload** them — that is, define what they mean for your own types.

Without overloading, if you try to write:

```cpp
BankAccount acc("Savings", "Alex");
std::cout << acc;  // ERROR — the compiler doesn't know how to print a BankAccount
```

the compiler doesn't know how to handle it. By overloading `operator<<`, you teach it.

The general signature for a binary operator overloaded as a non-member function is:

```cpp
ReturnType operator@(LeftType lhs, RightType rhs);
```

where `@` is the operator symbol. For `operator<<`:

```cpp
std::ostream& operator<<(std::ostream& os, const BankAccount& obj);
```

The key rules:
- You **cannot** invent new operators — only overload existing ones.
- You **cannot** change operator precedence or associativity.
- At least one operand must be a user-defined type (you can't overload `+` for two `int`s).

---

## Friend Functions (`friend`)

`operator<<` and `operator>>` need access to the `private` members of `BankAccount` (like `id`, `name`, `balance`, etc.), but they are **not member functions** of the class — they are standalone functions. Normally, a non-member function cannot touch `private` members. The `friend` keyword resolves this:

```cpp
class BankAccount {
    // ...
    friend std::ostream& operator<<(std::ostream&, const BankAccount&);
    friend std::istream& operator>>(std::istream&, BankAccount&);
};
```

Declaring a function as `friend` inside the class grants it full access to all `private` and `protected` members of that class, as if it were a member function itself. The `friend` declaration appears inside the class body but the function itself is defined outside and is **not** a member — it does not receive a `this` pointer, and it is not called with dot notation.

Think of it this way: the class itself chooses who its friends are. Just as you might give a trusted friend a key to your house, the class gives the friend function access to its private data.

**Why not make `operator<<` a member function instead?**

For `operator<<`, the left-hand operand is `std::ostream` (e.g., `std::cout`), not a `BankAccount`. If it were a member function of `BankAccount`, it would be called as `acc.operator<<(std::cout)`, which would force the syntax `acc << std::cout` — the opposite of what we want. As a non-member friend, both operands are explicit parameters and the natural syntax `std::cout << acc` works correctly.

---

## Overloading `operator<<` (output)

```cpp
std::ostream& operator<<(std::ostream& os, const BankAccount& obj) {
    os << "Id: "             << obj.id             << '\n';
    os << "Name: "           << obj.name           << '\n';
    os << "Account Holder: " << obj.accountHolder  << '\n';
    os << "Balance: "        << obj.balance        << '\n';
    os << "Investment Account: " << (obj.isInvestmentAccount ? "Yes" : "No") << '\n';
    if (obj.deposits) {
        os << "Deposits (" << obj.noDeposits << "): ";
        for (int i = 0; i < obj.noDeposits; i++)
            os << obj.deposits[i] << " ";
        os << '\n';
    } else {
        os << "Deposits: none\n";
    }
    return os;
}
```

The function takes a reference to `std::ostream` and a `const` reference to the object, writes formatted data into the stream, and **returns the stream by reference**. Returning `os` is what enables chaining:

```cpp
std::cout << acc1 << '\n' << acc2;
// equivalent to: (std::cout << acc1) << '\n' << acc2;
// the result of the first << is std::cout itself, so chaining works
```

The second parameter is `const BankAccount&` — a const reference — because printing should never modify the object. The ampersand (`&`) avoids an unnecessary copy.

---

## Overloading `operator>>` (input)

```cpp
std::istream& operator>>(std::istream& is, BankAccount& obj) {
    char buf[256];

    std::cout << "Name: ";
    is >> buf;
    obj.setName(buf);

    std::cout << "Account holder: ";
    std::string holder;
    is >> holder;
    obj.setAccountHolder(holder);

    // ... (balance, investment flag, deposits) ...

    return is;
}
```

`operator>>` is symmetric to `operator<<`: it takes a reference to `std::istream` and a **non-const** reference to the object (because it modifies the object by filling in its fields from the stream), and returns the stream by reference to allow chaining:

```cpp
std::cin >> acc1 >> acc2;
```

Notice that `operator>>` uses the public setters (`setName`, `setAccountHolder`, etc.) rather than accessing private members directly. This is valid — but since the function is declared `friend`, it *could* access private members directly too. Using setters is a stylistic choice that reuses existing validation logic.

---

## Behavioral Methods

Beyond storing and displaying data, a class should also model the **actions** the real-world entity can perform. Lab 3 adds three such methods.

### `deposit` and `withdraw`

```cpp
void BankAccount::deposit(int amount) {
    if (amount <= 0) {
        std::cout << "Deposit amount must be positive.\n";
        return;
    }
    balance += amount;
}

void BankAccount::withdraw(int amount) {
    if (amount <= 0) {
        std::cout << "Withdrawal amount must be positive.\n";
        return;
    }
    if (amount > balance) {
        std::cout << "Insufficient funds.\n";
        return;
    }
    balance -= amount;
}
```

These methods apply **input validation** before modifying state. The pattern is:
1. Check preconditions; report and return early if violated.
2. Perform the operation.

This is encapsulation in action — the caller cannot modify `balance` directly, so the only path through is these controlled functions, which enforce the rule that balance cannot go negative.

### `addDeposit`

```cpp
void BankAccount::addDeposit(int amount) {
    if (amount <= 0) {
        std::cout << "Deposit amount must be positive.\n";
        return;
    }
    int* newDeposits = new int[noDeposits + 1];
    for (int i = 0; i < noDeposits; i++)
        newDeposits[i] = deposits[i];
    newDeposits[noDeposits] = amount;

    delete[] deposits;
    deposits = newDeposits;
    noDeposits++;
    balance += amount;
}
```

`addDeposit` does more than `deposit` — it also **records** the deposit in the history array. Because the array has a fixed size, growing it requires:
1. Allocating a new, larger array.
2. Copying the existing elements.
3. Appending the new value.
4. Freeing the old array and replacing the pointer.

This is a classic dynamic array growth pattern. Notice that the old memory is freed with `delete[]` before the pointer is replaced — forgetting this step would cause a memory leak.

---

## Private Helper Methods

```cpp
void BankAccount::copyDeposits(int noDeposits, int* deposits) {
    this->noDeposits = noDeposits;
    if (noDeposits > 0) {
        this->deposits = new int[noDeposits];
        for (int i = 0; i < noDeposits; i++)
            this->deposits[i] = deposits[i];
    } else {
        this->deposits = nullptr;
    }
}
```

The logic for deep-copying a deposit array is needed in multiple places: the parameterized constructor, the copy constructor, and `operator=`. Instead of duplicating those lines three times (which would mean three places to update if the logic ever changes), the code extracts it into a **private helper method** `copyDeposits`.

It is `private` because it is an internal implementation detail — it only makes sense in the context of constructing or assigning a `BankAccount`, and exposing it publicly would be misleading.

The principle at work here is **DRY** — *Don't Repeat Yourself*. Duplicated code is a maintenance hazard: if you find a bug in the copy logic, you'd have to fix it in three places instead of one.

---

## Pre-increment vs. Post-increment (`++i` vs. `i++`)

In the constructors, `noAccounts` is incremented to assign a unique ID to each object:

```cpp
BankAccount::BankAccount() : id(++noAccounts) { ... }
```

Why `++noAccounts` and not `noAccounts++`? The difference lies in **what value the expression evaluates to**:

- `noAccounts++` — **post-increment**: evaluates to the *current* value, then increments. If `noAccounts` starts at `0`, the first object would get `id = 0`.
- `++noAccounts` — **pre-increment**: increments first, then evaluates to the *new* value. If `noAccounts` starts at `0`, the first object gets `id = 1`.

Combined with the initialization `int BankAccount::noAccounts = 0;`, using `++noAccounts` means IDs start from `1` — a more natural numbering for account identifiers. Using `noAccounts++` with an initial value of `0` would assign ID `0` to the first account, which is less intuitive; you'd have to start at `1` to get ID `1` for the first object.

For plain `int`, the compiler optimizes both to the same machine code when the return value is discarded (e.g., a standalone `i++;` statement). But in an initializer list expression — where the value *is* used — the distinction matters. Prefer `++i` when you use the result, as it is more explicit about intent and, for user-defined types that overload `++`, avoids creating a temporary copy.

---

## Quick Recap

| Concept | What it is | Where it appears in code |
|---|---|---|
| **Operator overloading** | Giving custom meaning to operators for user-defined types | `operator<<`, `operator>>` |
| **`friend` function** | Non-member function with access to `private` members | `friend std::ostream& operator<<(...)` |
| **`operator<<`** | Overloaded output operator; writes object to a stream | `std::cout << account;` |
| **`operator>>`** | Overloaded input operator; reads object from a stream | `std::cin >> account;` |
| **Behavioral methods** | Member functions that implement real-world actions | `deposit()`, `withdraw()`, `addDeposit()` |
| **Input validation** | Checking preconditions before modifying state | `if (amount <= 0) { ... return; }` |
| **Dynamic array growth** | Allocate larger array, copy, append, free old | Inside `addDeposit()` |
| **Private helper method** | Internal utility to avoid code duplication | `copyDeposits()` |
| **DRY principle** | Don't Repeat Yourself — extract shared logic | `copyDeposits` used in 3 constructors |
| **Pre-increment (`++i`)** | Increments first, returns new value | `: id(++noAccounts)` |
| **Post-increment (`i++`)** | Returns current value, then increments | (avoided here) |
