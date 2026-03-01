# Lab 1 — Pointers, References & Memory in C++

> **Course:** Object-Oriented Programming  
> **Topics:** Variables & addresses · References · Pointers · Pass by value / reference / pointer · Stack vs. Heap memory

---

## Table of Contents

1. [Memory Addresses](#1-memory-addresses)
2. [References](#2-references)
3. [Pointers](#3-pointers)
4. [Passing Arguments to Functions](#4-passing-arguments-to-functions)
   - [Pass by Value](#41-pass-by-value)
   - [Pass by Reference](#42-pass-by-reference)
   - [Pass by Pointer](#43-pass-by-pointer)
5. [Stack vs. Heap Memory](#5-stack-vs-heap-memory)
6. [Dynamic Memory Allocation](#6-dynamic-memory-allocation)
7. [Lab Code Walkthrough](#7-lab-code-walkthrough)
8. [Common Mistakes](#8-common-mistakes)
9. [Further Reading](#9-further-reading)

---

## 1. Memory Addresses

Every variable you declare lives somewhere in your computer's RAM. That location is called its **memory address**. You can think of RAM as a long street of numbered houses — each house holds one byte of data, and its street number is the address.

In C++ you can retrieve the address of any variable using the **address-of operator** `&`:

```cpp
int a = 1;
cout << &a;   // prints something like 0x7ffeefbff5ac
```

Key points:
- Addresses are printed in **hexadecimal** by default with `cout`.
- Two different variables always have different addresses (unless one is a reference — see below).
- The size a variable occupies at an address depends on its type (`int` is typically 4 bytes, `double` 8 bytes, etc.).

---

## 2. References

A **reference** is an alias — an alternative name for an existing variable. It is declared with `&` after the type:

```cpp
int a = 5;
int &ref = a;   // ref is another name for a

ref = 99;
cout << a;      // prints 99 — same variable!
```

Rules for references:
- A reference **must be initialised** when it is declared; you cannot have a "null reference".
- Once bound to a variable it **cannot be rebound** to another variable.
- `&ref` and `&a` return the **same address** — they are literally the same memory location.
- References are most useful as function parameters (see §4.2) and return types.

---

## 3. Pointers

A **pointer** is a variable whose *value* is a memory address. It is declared with `*` after the type:

```cpp
int a = 1;
int *b = &a;    // b holds the address of a
```

Two essential operators:

| Operator | Name | Meaning |
|---|---|---|
| `&variable` | address-of | gives the address where `variable` is stored |
| `*pointer` | dereference | gives the value *at* the address stored in the pointer |

```cpp
cout << b;    // address of a  (e.g. 0x7fff...)
cout << *b;   // value of a    (1)

*b = 42;      // changes a through the pointer
cout << a;    // 42
```

A pointer can also be **null** (`nullptr`) meaning it does not point to anything. Always check for `nullptr` before dereferencing.

```cpp
int *p = nullptr;
if (p != nullptr) cout << *p;   // safe guard
```

### Pointer vs. Reference — quick comparison

| Feature | Reference | Pointer |
|---|---|---|
| Syntax to declare | `int &r = a;` | `int *p = &a;` |
| Can be null | ❌ No | ✅ Yes (`nullptr`) |
| Can be rebound | ❌ No | ✅ Yes |
| Needs `*` to access value | ❌ No (transparent) | ✅ Yes |

---

## 4. Passing Arguments to Functions

When you call a function, C++ must decide **how** to hand the argument over. There are three mechanisms.

### 4.1 Pass by Value

A **copy** of the argument is made. The function works on the copy; the original is untouched.

```cpp
void valueByCopy(int x) {
    x = 10;   // only the local copy changes
}

int a = 1;
valueByCopy(a);
cout << a;    // still 1
```

Notice: `&x` inside the function prints a **different address** than `&a` in `main` — they are two separate variables.

**When to use:** When the function only needs to *read* the value and you don't want it to modify the original. Also useful when you intentionally want a working copy.

---

### 4.2 Pass by Reference

The function receives a **reference** to the original variable — no copy is made.

```cpp
void valueByReference(int &x) {
    x = 6;    // modifies the original
}

int a = 1;
valueByReference(a);
cout << a;    // 6
```

Notice: `&x` inside the function prints the **same address** as `&a` in `main`.

**When to use:** When the function needs to modify the original, or when passing a large object and you want to avoid the cost of copying it (use `const &` in that case if no modification is needed).

---

### 4.3 Pass by Pointer

The function receives a **copy of the address** (the pointer value itself is copied, but it still points to the original variable).

```cpp
void pointerExample(int *x) {
    *x = 7;   // dereferences the pointer → changes original
}

int a = 1;
pointerExample(&a);
cout << a;    // 7
```

Notice: the address *stored in* `x` equals `&a`, but `x` itself (the pointer variable) lives at a different address than `b` in `main`.

**When to use:** When the address itself needs to change (e.g. `int **`), when interfacing with C APIs, or when `nullptr` is a meaningful "no argument" sentinel.

### Summary table

| Mechanism | Syntax (caller) | Syntax (callee) | Copies original? | Can modify original? |
|---|---|---|---|---|
| By value | `f(a)` | `void f(int x)` | ✅ Yes | ❌ No |
| By reference | `f(a)` | `void f(int &x)` | ❌ No | ✅ Yes |
| By pointer | `f(&a)` | `void f(int *x)` | ❌ No (address copied) | ✅ Yes (via `*x`) |

---

## 5. Stack vs. Heap Memory

Your program's memory is divided into several regions. The two you interact with most are the **stack** and the **heap**.

### Stack

- Memory is managed **automatically** by the CPU as functions are called and return.
- Variables declared inside a function (local variables, parameters) live on the stack.
- When a function returns, its stack frame is **destroyed** — all its local variables vanish.
- Fast to allocate/deallocate; size is limited (typically a few MB).

```
main() stack frame:
  [ a = 1 ] [ b = 0x... ]

valueByCopy() stack frame (while executing):
  [ x = 1 ]   ← copy of a, different address
```

### Heap

- Also called **free store** or **dynamic memory**.
- You explicitly request memory with `new` and release it with `delete`.
- Memory persists until you free it — it is **not** tied to any function's lifetime.
- Slower than stack; size limited only by available RAM.
- **Forgetting to `delete` causes a memory leak.**

```
Heap (after new int(5)):
  [ 5 ]   ← b points here; this memory outlives any single function
```

### Visual comparison

```
High addresses
┌──────────────────┐
│      Stack       │  ← grows downward
│  (auto managed)  │
├──────────────────┤
│       ...        │
├──────────────────┤
│      Heap        │  ← grows upward
│  (you manage it) │
├──────────────────┤
│   Global / BSS   │
├──────────────────┤
│   Code / Text    │
└──────────────────┘
Low addresses
```

---

## 6. Dynamic Memory Allocation

### Single variable

```cpp
int *p = new int(5);    // allocates 1 int on the heap, initialised to 5
cout << *p;             // 5
delete p;               // frees the memory
p = nullptr;            // good practice: avoid dangling pointer
```

### Array

```cpp
int *arr = new int[5];          // allocates array of 5 ints on the heap
for (int i = 0; i < 5; i++)
    arr[i] = i + 1;             // fills with 1, 2, 3, 4, 5

delete[] arr;                   // note: delete[] for arrays, NOT delete
arr = nullptr;
```

> ⚠️ **Always pair `new` with `delete`, and `new[]` with `delete[]`.** Mixing them is **undefined behaviour**.

### What happens without `delete`?

The memory remains allocated until the program exits — this is a **memory leak**. In long-running programs or embedded systems, leaks eventually exhaust all available memory and crash the system.

---

## 7. Lab Code Walkthrough

Below is a step-by-step explanation of what the lab program does:

```cpp
int a = 1, *b = &a;
```
- `a` is an `int` on the stack, initialised to `1`.
- `b` is a pointer on the stack, initialised to hold the address of `a`.

---

```cpp
cout << "Value of b (address of a): " << b << endl;
cout << "Value pointed to by b (value of a): " << *b << endl;
```
- `b` prints the **address** stored in the pointer (same as `&a`).
- `*b` dereferences the pointer, printing the **value at that address** (`1`).

---

```cpp
valueByCopy(a);
// a is still 1
```
- `a` is copied into the parameter `x`. `x = 10` inside the function only changes the local copy.

---

```cpp
valueByReference(a);
// a is now 6
```
- `x` is a reference to `a`. `x = 6` directly modifies `a`.

---

```cpp
pointerExample(b);
// a is now 7
```
- `b` (which holds `&a`) is passed to `pointerExample`. Inside, `*x = 7` writes `7` to the address pointed to — which is `a`.

---

```cpp
b = new int(5);
// ...
delete b;
```
- `new int(5)` allocates an integer on the **heap** and returns its address, which is stored in `b`. `b` no longer points to `a`.
- `delete b` frees that heap memory.

---

```cpp
b = &a;
```
- Points `b` back to `a` on the stack.

---

```cpp
b = new int[5];
for (int i = 0; i < 5; i++) b[i] = i + 1;
// ...
delete[] b;
```
- `new int[5]` allocates a contiguous block of 5 integers on the heap.
- `b[i]` is equivalent to `*(b + i)` — pointer arithmetic.
- `delete[] b` frees the entire array.

---

```cpp
int& ref = returnByReference();
```
- Create a dynamically allocated variable inside a function and then use it in other parts of the code.

---

## 8. Common Mistakes

| Mistake | Description | Fix |
|---|---|---|
| Dangling pointer | Using a pointer after `delete` | Set to `nullptr` after deleting |
| Memory leak | Forgetting `delete` / `delete[]` | Every `new` must have a matching `delete` |
| `delete` vs `delete[]` | Using `delete` on an array | Use `delete[]` for anything allocated with `new[]` |
| Dereferencing `nullptr` | Accessing `*p` when `p` is null | Check `p != nullptr` first |
| Uninitialised pointer | `int *p;` then `*p = 5;` | Always initialise pointers before use |
| Taking address of a temporary | `int *p = &(a + 1);` | Only take addresses of named variables |

---

## 9. Further Reading

### Official References
- [cppreference — Pointer declaration](https://en.cppreference.com/w/cpp/language/pointer)
- [cppreference — Reference declaration](https://en.cppreference.com/w/cpp/language/reference)
- [cppreference — new expression](https://en.cppreference.com/w/cpp/language/new)
- [cppreference — delete expression](https://en.cppreference.com/w/cpp/language/delete)
- [cppreference — Memory model](https://en.cppreference.com/w/cpp/language/memory_model)
- [Memory management in C: The heap and the stack](https://web.archive.org/web/20170829060314/http://www.inf.udec.cl/%7Eleo/teoX.pdf)

### Tutorials & Guides
- [LearnCpp.com — Chapter 9: Compound Types (References & Pointers)](https://www.learncpp.com/cpp-tutorial/introduction-to-pointers/)
- [LearnCpp.com — Dynamic Memory Allocation](https://www.learncpp.com/cpp-tutorial/dynamic-memory-allocation-with-new-and-delete/)
- [LearnCpp.com — The Stack and the Heap](https://www.learncpp.com/cpp-tutorial/the-stack-and-the-heap/)
- [GeeksForGeeks — Pointers in C++](https://www.geeksforgeeks.org/cpp-pointers/)
- [GeeksForGeeks — References in C++](https://www.geeksforgeeks.org/references-in-cpp/)

### Videos
- [The Cherno — Pointers in C++](https://www.youtube.com/watch?v=DTxHyVn0ODg)
- [The Cherno — References in C++](https://www.youtube.com/watch?v=IzoFn3dfsPA)
- [The Cherno — Stack vs Heap Memory](https://www.youtube.com/watch?v=wJ1L2nSIV1s)

### Books
- *C++ Primer* (Lippman, Lajoie, Moo) — Chapters 2 & 12
- *A Tour of C++* (Bjarne Stroustrup) — Chapter 1