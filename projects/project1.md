# Tema 1 — Programare Orientată pe Obiecte

## Scopul temei

Tema 1 are ca obiectiv principal înțelegerea fundamentelor C++ și a modului în care puteți modela lumea reală printr-o clasă. Veți construi o aplicație cu un **meniu interactiv** prin care să demonstrați funcționalitatea implementată.

---

## ⚠️ Reguli stricte

> **Nerespectarea acestor reguli duce la depunctare sau invalidarea cerinței.**

- ❌ **FĂRĂ variabile globale**
- ❌ **FĂRĂ atribute publice**
- ❌ **FĂRĂ cod de umplutură / cod fără sens**
- ❌ **FĂRĂ copy-paste**

---

## 🤖 Politica privind utilizarea AI

**NU aveți voie să folosiți AI pentru generarea de cod. Deloc.**

Codul depus trebuie să fie scris în întregime de voi. Veți fi puși să explicați și să modificați codul la cerere. Dacă nu știți ce ați scris, se va vedea.

AI-ul poate fi folosit **exclusiv în mod educațional**, pentru a vă explica concepte pe care nu le înțelegeți. Cu toate acestea, **vă recomandăm puternic** să discutați între voi și să apelați la AI doar ca ultimă resursă.

---

## 📅 Termene limită

| Eveniment | Termen |
|---|---|
| **Alegerea temei proiectului** | **08.03.2026** |
| **Trimiterea proiectului** | **22.03.2026** |

---

## Cerințe tehnice

- Definirea a **minim 4 clase** (fara mostenire la proiectul 1)
- Fiecare clasă trebuie să conțină minim 4 variabile, iar in tot proiectul trebuie să aveți:
    - Int/Long
    - Bool
    - Char
    - Char*
    - Float
    - Int*/Float*
    - Double
    - Const
    - Static 
- **Constructori de inițializare cu parametri** pentru fiecare clasă
- Pentru toate clasele: constructor de copiere, `operator=` de copiere, destructor
- supraîncărcarea operatorilor **`operator<<`** si **`operator>>`** pentru toate clasele
- Setters și Getters pentru minim 2 clase
- Cât mai multe **`const`** (unde este cazul) și **funcții private**
- Minim **3 funcții membru publice** pentru funcționalități netriviale specifice temei, dintre care cel puțin 1-2 mai complexe *(nu doar citiri/afișări sau adăugat/șters elemente)*
- **Meniu interactiv** care să permită demonstrarea tuturor funcționalităților

---

## Scenariu de utilizare

- Crearea de obiecte și apelarea tuturor funcțiilor membru publice în `main`
- Fișierul `tastatura.txt` va conține **doar** exemple de date de intrare de la tastatură (dacă există); datele din fișiere se pun în fișiere separate

---

## Cerințe de proiect

- **Commit-uri** cât mai granulare (fiecare mesaj din commit ar trebui să fie o singură acțiune, e.g. "added Student class", in loc de un singur commit cu tot proiectul)
- **Code review** la 2 proiecte ale colegilor