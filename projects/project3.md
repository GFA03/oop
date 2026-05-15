# Tema 3 — Programare Orientată pe Obiecte

## Scopul temei

Tema 3 finalizează seria de proiecte prin introducerea **template-urilor**, a **smart pointer-elor** și a **design pattern-urilor**. Veți rafina codul astfel încât să fie **generic**, **safe** din punct de vedere al gestionării memoriei și **bine structurat** prin folosirea unor pattern-uri consacrate. Aplicația va fi demonstrată printr-un **meniu interactiv**.

---

## ⚠️ Reguli stricte

> **Nerespectarea acestor reguli duce la depunctare sau invalidarea cerinței.**

- ❌ **FĂRĂ variabile globale** — orice variabilă globală (vector, array, variabilă simplă) duce automat la **nota 0**
- ✅ **Default values** sunt citite din fișiere. No more variabile globale care să țină setări de configurație.
- ❌ **FĂRĂ atribute publice**
- ❌ **FĂRĂ cod de umplutură / cod fără sens**
- ❌ **FĂRĂ copy-paste**
- ❌ **FĂRĂ `new` / `delete` direct în cod** — toată alocarea dinamică se face prin smart pointers (`std::unique_ptr`, `std::shared_ptr`, `std::weak_ptr`). Excepție: `new` e permis doar în interiorul `std::make_unique` / `std::make_shared` (oricum nu îl scrieți voi).
- ❌ **FĂRĂ pointeri raw care dețin resurse** — pointerii raw pot fi folosiți doar ca *observatori* (non-owning), nu pentru ownership

---

## 🤖 Politica privind utilizarea AI

**NU aveți voie să folosiți AI pentru generarea de cod. Deloc.**

Codul depus trebuie să fie scris în întregime de voi. Veți fi puși să explicați și să modificați codul la cerere. Dacă nu știți ce ați scris, se va vedea.

AI-ul poate fi folosit **exclusiv în mod educațional**, pentru a vă explica concepte pe care nu le înțelegeți. Cu toate acestea, **vă recomandăm puternic** să discutați între voi și să apelați la AI doar ca ultimă resursă.

---

## 📅 Termene limită

| Eveniment | Termen | Penalizare |
|---|---|---|
| **Alegerea temei proiectului** | **10.05.2026 (Saptamana 10) 23:59** | -2 puncte pentru întârziere |
| **Trimiterea proiectului** | **24.05.2026 (Saptamana 12) 23:59** | -2 puncte pentru întârziere |

> Tema proiectului se alege pe **Teams**, în locul indicat acolo. Dacă nu alegeți tema la timp, se scad **2 puncte** din nota finală a temei.

---

## Cerințe tehnice

### Structura claselor (moștenită din Tema 2)

- **Minim 5 clase** care modelează o aplicație coerentă
- **Minim 3 clase** să fie legate prin **moștenire** (ierarhie `is-a`)
- Toate clasele trebuie să respecte **encapsularea** — niciun atribut public, acces controlat exclusiv prin metode
- 

### Ce trebuie să conțină fiecare clasă

- Pana acum ati fost obligati sa supraincarcati tot. La proiectul acesta vreau sa vad un proiect cat mai aproape de realitate. Adica, definiti doar ce aveti nevoie si mentineti standarde bune de cod (e.g. fara functii mai lungi de 50 de linii, numiti variabilele corespunzator).

Acesta este ultimul proiect la disciplina OOP, folositi cat mai multe concepte OOP si C++ (puteti cauta pe net concepte cat mai multe)

### 🆕 Template-uri (obligatoriu)

- **Minim 1 clasă template** proprie, scrisă de voi, **utilă funcțional** în proiect (ex: `Repository<T>`, `Cache<K,V>`, `Inventory<T>`, `EventQueue<T>`). Nu se acceptă template-uri triviale gen `Pair<A,B>` care doar țin două valori fără să fie folosite.
- **Minim 2 funcții template** (libere sau membre) folosite efectiv în logica aplicației
- Template-ul trebuie **instantiat cu cel puțin 2 tipuri diferite** în `main` / meniu, ca să se vadă că e generic
- **Cel puțin o specializare** (totală sau parțială) sau **un constraint** (`static_assert`, `if constexpr`, sau concepts dacă folosiți C++20)

Va sugerez sa folositi compilatoare C++ cat mai noi, C++20+ ca sa fiti siguri ca va merg toate notiunile pe care le veti gasi pe net.

Nota: Cu cat folositi o versiune C++ mai noua, cu atat veti gasi si veti putea folosi elemente noi (e.g. 
- C++11: auto, range-based loops, lambda expressions, std::move, enum class
- C++17: structured bindings, std::optional, std::filesystem
- C++20: concepts, ranges library, modules, coroutines
)

### 🆕 Smart pointers (obligatoriu)

- Toată alocarea dinamică din proiect se face exclusiv prin smart pointers
- **`std::unique_ptr`** — folosit pentru ownership exclusiv (ex: o clasă deține o resursă internă)
- **`std::shared_ptr`** — folosit acolo unde ownership-ul este partajat (ex: același obiect referit din mai multe colecții)
- **`std::weak_ptr`** — folosit cel puțin într-un loc pentru a sparge un ciclu de referințe sau pentru o referință observator
- Containerele polimorfice (vectori de pointeri la clasa de bază) trebuie să folosească `std::unique_ptr<Base>` sau `std::shared_ptr<Base>`, **nu** `Base*`
- Folosiți `std::make_unique` / `std::make_shared` — **fără `new` direct**

### 🆕 Design patterns (minim 2)

Trebuie să implementați și să folosiți efectiv **cel puțin 2 design pattern-uri** distincte. Pattern-ul trebuie să rezolve o problemă reală din proiect, nu să fie pus de formă.

Lista de pattern-uri acceptate (alegeți minim 2):

- **Singleton** — pentru o resursă globală unică (ex: logger, manager de configurație, conexiune). Atenție: implementare thread-safe (Meyers Singleton acceptat), constructor `private`, copy/move dezactivate.
- **Factory / Factory Method** — pentru crearea polimorfică a obiectelor în funcție de un input (ex: tip citit din fișier)
- **Builder** — pentru construirea pas cu pas a unui obiect cu mulți parametri opționali
- **Observer** — pentru notificarea mai multor obiecte la schimbări de stare (ex: `Subject` — `Observer`)
- **Strategy** — pentru a alege la runtime între mai mulți algoritmi (ex: criterii de sortare, politici de preț)
- **Decorator** — pentru a adăuga dinamic responsabilități unui obiect
- **Adapter** — pentru a face compatibile două interfețe diferite
- **Command** — pentru a încapsula o acțiune ca obiect (util pentru undo/redo, cozi de comenzi în meniu)
- **Iterator custom** — peste o structură proprie (nu peste `std::vector`)

### Alte cerințe

- **Meniu interactiv** care să permită demonstrarea tuturor funcționalităților, inclusiv a pattern-urilor
- Toate erorile trebuie tratate (try / catch); folosiți excepții proprii (clase derivate din `std::exception`) acolo unde are sens
- Folosiți cât mai mult **STL** (containere, algoritmi, `std::function`, `std::optional` etc.)
- **Regula celor 5** sau **regula celor 0** — dacă o clasă deține resurse, definește toate cele 5 (ctor copy, ctor move, op= copy, op= move, destructor); dacă folosește doar smart pointers și tipuri standard, lăsați compilatorul să le genereze (`= default`)

---

## Scenariu de utilizare

- Crearea de obiecte și apelarea/testarea **tuturor** funcțiilor membru publice în `main` — dacă nu le apelăm, la ce le-am mai definit?
- Polimorfismul trebuie **demonstrat explicit** în meniu sau în `main` (ex: colecție de `unique_ptr<Base>` care apelează metode virtuale)
- Template-urile trebuie **instanțiate cu mai multe tipuri** ca să se vadă că sunt generice
- Pattern-urile trebuie **utilizate efectiv** prin meniu (ex: opțiune care declanșează Observer-ul, sau care folosește Factory pentru a crea obiecte)

---

## Cerințe de proiect

- **Proiectul trebuie să fie în fișiere separate!!! (nu totul în `main.cpp`)**
- Organizare clară: `include/` pentru `.h` / `.hpp`, `src/` pentru `.cpp`, `data/` pentru fișiere de input
- Proiectul trebuie să fie găzduit pe un **repository de `git`** la care să avem acces (nu e obligatoriu să fie GitHub)
- Obligatoriu fișier **`.gitignore`** configurat corespunzător
- **NU** faceți commit prin upload de fișiere din browser — nu se ia în considerare `.gitignore` în acest caz
- **Commit-uri** cât mai granulare (fiecare mesaj din commit ar trebui să fie o singură acțiune, e.g. "added Repository<T> template", în loc de un singur commit cu tot proiectul)
- **README.md** care să descrie: aplicația, lista de pattern-uri folosite cu locația lor în cod, instrucțiuni de compilare
- Compilare cu **minim C++17** (recomandat C++20); fără warning-uri la `-Wall -Wextra -Wpedantic`
- **Code review** la 2 proiecte ale colegilor

---

## Bonus

- **+1 punct** dacă folosiți **3 sau mai multe** design pattern-uri implementate corect
- **+1 punct** dacă folosiți **C++20 concepts** pentru a constrânge template-urile sau alte functionalitati specifice C++20
- **+1 punct** dacă scrieți **teste unitare** (Google Test, Catch2 sau framework propriu) pentru cel puțin 2 clase / template-uri
