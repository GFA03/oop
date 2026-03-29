# Tema 2 — Programare Orientată pe Obiecte

## Scopul temei

Tema 2 extinde fundamentele din Tema 1 prin introducerea **moștenirii** și a **polimorfismului**. Veți modela o aplicație reală (ex: Netflix, un magazin de jocuri video, o platformă de muzică, un sistem de rezervări etc.) folosind o ierarhie de clase bine definită. Aplicația va fi demonstrată printr-un **meniu interactiv**.

---

## ⚠️ Reguli stricte

> **Nerespectarea acestor reguli duce la depunctare sau invalidarea cerinței.**

- ❌ **FĂRĂ variabile globale** — orice variabilă globală (vector, array, variabilă simplă) duce automat la **nota 0**
- ✅ **Default values** sunt citite din fisiere. No more variabile globale care sa tina default configs.
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

| Eveniment | Termen | Penalizare |
|---|---|---|
| **Alegerea temei proiectului** | **05.04.2026 (Saptamana 6) 23:59** | -2 puncte pentru întârziere |
| **Trimiterea proiectului** | **26.04.2026 (Saptamana 8) 23:59**  | -2 puncte pentru întârziere |

> Tema proiectului se alege pe **Teams**, în locul indicat acolo. Dacă nu alegeți tema la timp, se scad **2 puncte** din nota finală a temei.

---

## Cerințe tehnice

### Structura claselor

- **Minim 5 clase** care modelează o aplicație coerentă (ex: Netflix, magazin de jocuri, platformă de muzică, sistem de rezervări etc.)
- **Minim 3 clase** să fie legate prin **moștenire** (ierarhie `is-a`)
- **Cel puțin o clasă abstractă / interfață** (cu metode pur virtuale `= 0`)
- Toate clasele trebuie să respecte **encapsularea** — niciun atribut public, acces controlat exclusiv prin metode

### Ce trebuie să conțină fiecare clasă

Fiecare clasă din proiect trebuie să aibă **obligatoriu**:

- Constructor implicit (fără parametri)
- Constructor cu parametri
- Constructor de copiere
- `operator=` de copiere
- Destructor
- `operator<<` (citire din `std::cin`)
- `operator>>` (afișare în `std::cout`)

### Concepte OOP obligatorii

- **Moștenire** — minim 3 clase să formeze o ierarhie (poate fi liniară sau arborescentă)
- **Virtualizare** — metode `virtual` în clasa de bază
- **Polimorfism** — demonstrat explicit: apel de metode virtuale prin pointeri sau referințe la clasa de bază
- **Clasă abstractă** — cel puțin o clasă cu metode pur virtuale care nu poate fi instanțiată direct

### Alte cerințe

- Setters și Getters pentru minim 2 clase
- Cât mai multe **`const`** (unde este cazul) și **funcții private**
- Minim **3 funcții membru publice** pentru funcționalități netriviale specifice temei, dintre care cel puțin 1-2 mai complexe *(nu doar citiri/afișări sau adăugat/șters elemente)*
- **Meniu interactiv** care să permită demonstrarea tuturor funcționalităților

---

## Scenariu de utilizare

- Crearea de obiecte și apelarea/testarea **tuturor** funcțiilor membru publice în `main` — dacă nu le apelăm, la ce le-am mai definit?
- Polimorfismul trebuie **demonstrat explicit** în meniu sau în `main` (ex: colecție de pointeri la clasa de bază care apelează metode virtuale)
- Fișierul `tastatura.txt` va conține **doar** exemple de date de intrare de la tastatură (dacă există); datele din fișiere se pun în fișiere separate

---

## Cerințe de proiect

- Proiectul trebuie să fie găzduit pe un **repository de `git`** la care să avem acces (nu e obligatoriu să fie GitHub)
- Obligatoriu fișier **`.gitignore`** configurat corespunzător
- **NU** faceți commit prin upload de fișiere din browser — nu se ia în considerare `.gitignore` în acest caz
- **Commit-uri** cât mai granulare (fiecare mesaj din commit ar trebui să fie o singură acțiune, e.g. "added Movie class", în loc de un singur commit cu tot proiectul)
- **Code review** la 2 proiecte ale colegilor
