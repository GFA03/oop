# Laborator 2 — Clase, Obiecte și Ciclul de Viață al Obiectelor

## Cuprins

- [Ce este Programarea Orientată pe Obiecte (OOP)?](#ce-este-programarea-orientată-pe-obiecte-oop)
- [Clasă, Obiect, Instanță — care e diferența?](#clasă-obiect-instanță--care-e-diferența)
- [Anatomia unei clase în C++](#anatomia-unei-clase-în-c)
- [Specificatorii de acces: `private` și `public`](#specificatorii-de-acces-private-și-public)
- [Membrii statici (`static`)](#membrii-statici-static)
- [Membrii constanți (`const`)](#membrii-constanți-const)
- [Constructori](#constructori)
  - [Constructorul implicit (default)](#constructorul-implicit-default)
  - [Constructori cu parametri](#constructori-cu-parametri)
  - [Constructorul de copiere (copy constructor)](#constructorul-de-copiere-copy-constructor)
  - [Lista de inițializare (member initializer list)](#lista-de-inițializare-member-initializer-list)
- [Destructorul](#destructorul)
- [Operatorul de atribuire (`operator=`)](#operatorul-de-atribuire-operator)
- [Operatorul de rezoluție (`::`)](#operatorul-de-rezoluție-)
- [Declarare vs. Implementare — de ce separăm?](#declarare-vs-implementare--de-ce-separăm)
- [Pointer-ul `this`](#pointer-ul-this)

---

## Ce este Programarea Orientată pe Obiecte (OOP)?

Până acum, în liceu, ați scris cod **procedural**: o secvență de instrucțiuni executate una după alta, cu funcții care operează pe date transmise ca parametri. Datele și funcțiile care le prelucrează trăiesc separat — funcțiile primesc date, le modifică, le returnează.

**Programarea Orientată pe Obiecte** schimbă complet modul de gândire. În loc să separăm datele de funcții, le grupăm împreună într-o singură unitate numită **clasă**. Ideea fundamentală este următoarea: în lumea reală, lucrurile au atât **proprietăți** (ce sunt), cât și **comportamente** (ce fac). Un cont bancar are un sold, un titular, un nume — dar poate și să primească depuneri, să afișeze informații, etc. OOP ne permite să modelăm aceste entități din lumea reală direct în cod.

Cele patru principii fundamentale ale OOP sunt **încapsularea**, **abstractizarea**, **moștenirea** și **polimorfismul**. În acest laborator ne concentrăm pe primele două, iar pe celelalte le vom explora în laboratoarele viitoare.

---

## Clasă, Obiect, Instanță — care e diferența?

Aceste trei noțiuni sunt esențiale și sunt adesea confundate de începători.

**Clasa** este un **șablon** (o „rețetă", un „plan tehnic"). Ea descrie ce proprietăți și ce comportamente va avea ceva, dar nu este acel ceva în sine. Gândește-te la clasa `BankAccount` ca la formularul gol de deschidere a unui cont la bancă — descrie ce informații trebuie completate (nume, sold, tip de cont), dar nu reprezintă încă un cont real al cuiva.

```cpp
class BankAccount {
private:
    char* name;
    double balance;
public:
    void print();
};
```

**Obiectul** este o **realizare concretă** a acelui șablon. Când declari o variabilă de tipul unei clase, creezi un obiect. Acum formularul e completat — avem un cont real, cu date reale.

```cpp
BankAccount x;  // x este un obiect de tip BankAccount
```

**Instanța** este, în practică, **sinonim cu obiectul**. Când spunem „`x` este o instanță a clasei `BankAccount`", spunem același lucru ca „`x` este un obiect de tip `BankAccount`". Termenul „instanță" subliniază relația cu clasa din care provine — ca și cum ai spune „acest cont specific a fost creat după șablonul BankAccount". Termenul „obiect" pune accent pe entitatea în sine.

Într-o propoziție: **clasa** e planul, **obiectul/instanța** e casa construită după acel plan. Poți construi 100 de case (obiecte) după același plan (clasă) — fiecare are propriile camere mobilate diferit (date diferite), dar toate respectă aceeași structură.

---

## Anatomia unei clase în C++

O clasă în C++ se declară cu cuvântul cheie `class`, urmat de numele clasei și un bloc delimitat de acolade, încheiat cu `;`:

```cpp
class BankAccount {
private:
    // date membre (atribute) — NU pot fi accesate din exterior
    static int noAccounts;
    const int id;
    char* name;
    std::string accountHolder;
    double balance;
    bool isInvestmentAccount;
    int noDeposits;
    int* deposits;

public:
    // funcții membre (metode) — pot fi apelate din exterior
    BankAccount();
    BankAccount(char*, std::string);
    BankAccount(char*, std::string, double, bool, int, int*);
    BankAccount(const BankAccount &obj);
    BankAccount& operator=(const BankAccount &obj);
    ~BankAccount();

    void print();
};
```

Observă structura: secțiunea `private` conține **datele** (ce „știe" un cont), iar secțiunea `public` conține **interfața** (ce „poate face" un cont, ce e vizibil din exterior). Această separare se numește **încapsulare** — ascundem detaliile interne și expunem doar operațiile permise.

---

## Specificatorii de acces: `private` și `public`

Într-o clasă, fiecare membru (dată sau funcție) are un **nivel de acces**:

**`private`** înseamnă că membrul respectiv poate fi accesat **doar din interiorul clasei** — adică doar din funcțiile membre ale clasei. Codul din `main()` sau din alte clase **nu** poate citi sau modifica direct un membru privat. Dacă încerci, compilatorul dă eroare. În C++, dacă nu specifici nimic, membrii unei clase sunt `private` implicit.

**`public`** înseamnă că membrul poate fi accesat de **oriunde** — din `main()`, din alte funcții, din alte clase.

De ce nu facem totul `public`? Din același motiv pentru care un bancomat nu-ți dă acces direct la seiful băncii. Vrei ca soldul să poată fi modificat doar prin operații controlate (depunere, retragere), nu prin atribuire directă. Dacă cineva ar scrie `cont.balance = -999999;` din `main()`, ar strica logica programului. Încapsularea previne acest lucru.

```cpp
BankAccount x;
// x.balance = 1000;    // EROARE! balance este private
x.print();              // OK — print() este public
```

---

## Membrii statici (`static`)

Uită-te la această linie din clasă:

```cpp
static int noAccounts;
```

Un membru **`static`** nu aparține unui obiect individual — el aparține **clasei înseși**. Există o singură copie a lui `noAccounts` partajată de toate obiectele de tip `BankAccount`. Indiferent câte conturi creezi, `noAccounts` este unul singur, în memorie, o singură dată.

Inițializarea unui membru static se face **în afara clasei**, o singură dată:

```cpp
int BankAccount::noAccounts = 1;
```

De ce e util? În cazul nostru, `noAccounts` funcționează ca un contor global de conturi. De fiecare dată când se creează un obiect nou, constructorul citește valoarea curentă a lui `noAccounts` și o atribuie câmpului `id` al obiectului, apoi incrementează contorul. Astfel fiecare cont primește un ID unic, automat, fără ca programatorul să-l specifice manual.

Gândește-te la `static` ca la o tablă scrisă pe peretele clasei — toți elevii (obiectele) o văd și o partajează, dar nu e a niciunuia în particular.

---

## Membrii constanți (`const`)

```cpp
const int id;
```

Un membru declarat `const` **nu poate fi modificat** după ce a fost inițializat. Odată ce un cont primește ID-ul 3, acel ID rămâne 3 pe toată durata de viață a obiectului. Acest lucru reflectă realitatea — un cod de identificare nu se schimbă.

Membrii `const` **trebuie** inițializați prin **lista de inițializare** a constructorului (vezi secțiunea dedicată mai jos). Nu poți scrie `id = valoare;` în corpul constructorului — ar fi o atribuire, nu o inițializare, iar compilatorul nu permite asta pentru `const`.

---

## Constructori

Un **constructor** este o funcție specială care se apelează **automat** în momentul în care un obiect este creat. Scopul său este să **inițializeze** obiectul — să-i dea valori de pornire corecte tuturor membrilor. Fără constructor, membrii unei clase ar conține „gunoi" (valori reziduale din memorie).

Constructorii au câteva particularități care îi diferențiază de funcțiile obișnuite: au **același nume** ca și clasa, **nu au tip de return** (nici măcar `void`), și pot fi **supraîncărcați** (overloaded) — adică poți avea mai mulți constructori cu semnături diferite.

### Constructorul implicit (default)

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

Acesta este **constructorul fără parametri** (sau „default"). Se apelează când creezi un obiect fără să-i dai argumente: `BankAccount x;`. El pune valori „neutre" pentru toate câmpurile: sold 0, nicio depunere, numele „N/A".

Observă `strcpy(new char[4], "N/A")` — se alocă dinamic memorie pentru un șir de 3 caractere + terminatorul `'\0'`, apoi se copiază textul „N/A" în acea zonă. Lucrul cu `char*` (C-strings) necesită gestionare manuală a memoriei, motiv pentru care e importantă și existența destructorului.

### Constructori cu parametri

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

Acest constructor primește doar numele și titularul contului, restul rămân la valori implicite. Există și un al treilea constructor care primește **toți** parametrii:

```cpp
BankAccount::BankAccount(char* name, std::string accountHolder, double balance,
    bool isInvestmentAccount, int noDeposits, int* deposits) : id(noAccounts++) {
    // ... copiază fiecare valoare ...
}
```

Observă cum array-ul `deposits` este **copiat element cu element** — nu se copiază pointer-ul, ci se alocă un nou array și se copiază valorile. Aceasta este o **copie profundă** (deep copy). Dacă am fi copiat doar pointer-ul (`this->deposits = deposits;`), ambele obiecte ar fi arătat spre aceeași zonă de memorie — modificarea unuia l-ar afecta pe celălalt, iar ștergerea unuia ar invalida datele celuilalt.

### Constructorul de copiere (copy constructor)

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

**Constructorul de copiere** se apelează când creezi un obiect nou ca o copie a unuia existent:

```cpp
BankAccount a("Cont", "Ion", 500.0, false, 2, deps);
BankAccount b(a);   // se apelează constructorul de copiere
BankAccount c = a;  // și asta apelează constructorul de copiere (NU operatorul =)
```

Semnătura este întotdeauna de forma `NumeClasa(const NumeClasa &obiect)`. Parametrul este o **referință constantă** la obiectul sursă — referință ca să nu se facă o copie (care ar apela din nou constructorul de copiere, creând o recursie infinită), și `const` pentru că nu avem de gând să modificăm sursa.

De ce e necesar? Dacă clasa conține **pointeri** (cum e `name` sau `deposits`), constructorul de copiere generat automat de compilator face doar o **copie superficială** (shallow copy) — copiază valoarea pointer-ului, nu datele spre care arată. Rezultatul: două obiecte care indică aceeași memorie. La distrugerea primului obiect, memoria e eliberată, iar al doilea obiect rămâne cu un pointer invalid (dangling pointer). De aceea scriem noi constructorul de copiere pentru a face o **copie profundă** (deep copy).

### Lista de inițializare (member initializer list)

```cpp
BankAccount::BankAccount() : id(noAccounts++) {
```

Textul `: id(noAccounts++)` de după semnătura constructorului este **lista de inițializare**. Aceasta inițializează membrii **înainte** ca executarea corpului constructorului să înceapă. Este **obligatorie** pentru membrii `const` (care nu pot fi atribuiți ulterior) și pentru membrii de tip referință.

Diferența față de atribuire în corpul constructorului: lista de inițializare **construiește** membrul direct cu valoarea dată, în timp ce atribuirea în corp presupune că membrul a fost deja construit cu o valoare implicită și apoi i se suprascrie valoarea — un pas în plus, și uneori imposibil (cazul `const`).

---

## Destructorul

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

**Destructorul** este funcția care se apelează **automat** când un obiect este distrus — fie când iese din scope (domeniu de vizibilitate), fie când se apelează `delete` pe un obiect alocat dinamic. Semnătura sa este `~NumeClasa()`. Nu primește parametri și nu returnează nimic.

Rolul lui: să **elibereze resursele** pe care obiectul le-a achiziționat în timpul vieții sale. În cazul nostru, clasa a alocat memorie dinamic (cu `new[]`) pentru `name` și `deposits` — dacă nu eliberăm acea memorie cu `delete[]`, avem un **memory leak** (scurgere de memorie): programul consumă din ce în ce mai multă memorie fără să o elibereze vreodată.

Setarea pointer-ilor la `nullptr` după eliberare este o practică bună de siguranță — previne situația în care cineva ar încerca din greșeală să acceseze memoria deja eliberată (double-free sau use-after-free).

---

## Operatorul de atribuire (`operator=`)

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

**Operatorul de atribuire** se apelează când atribui un obiect **deja existent** unui alt obiect deja existent:

```cpp
BankAccount a, b;
b = a;  // operator= (b există deja, nu se construiește acum)
```

**Atenție la diferența față de constructorul de copiere:**

```cpp
BankAccount b(a);   // constructor de copiere — b se creează acum
BankAccount b = a;  // tot constructor de copiere — b se creează acum
b = a;              // operator= — b exista deja, i se atribuie valorile lui a
```

Operatorul de atribuire trebuie să facă trei lucruri pe care constructorul de copiere nu le face:

1. **Verificarea auto-atribuirii** (`if (this == &obj)`) — dacă cineva scrie `x = x;`, nu vrem să ștergem datele și apoi să încercăm să le copiem din obiectul tocmai golit.
2. **Eliberarea memoriei vechi** — obiectul destinație avea deja date alocate dinamic; trebuie eliberate înainte de a aloca altele noi, altfel pierdem referința către ele (memory leak).
3. **Returnarea `*this`** — permite **înlănțuirea** atribuirilor: `a = b = c;`.

---

## Operatorul de rezoluție (`::`)

Operatorul `::` (scope resolution operator) spune compilatorului **cui aparține** un anumit nume. Îl folosim în mai multe situații:

**1. Implementarea funcțiilor membre în afara clasei:**

```cpp
void BankAccount::print() {
    // ...
}
```

Aici `BankAccount::print()` înseamnă „funcția `print` care aparține clasei `BankAccount`". Fără `BankAccount::`, compilatorul ar crede că `print` este o funcție globală, independentă de orice clasă.

**2. Inițializarea membrilor statici:**

```cpp
int BankAccount::noAccounts = 1;
```

Membrii statici sunt declarați în clasă, dar trebuie **definiți** (li se alocă memorie și li se dă o valoare) în afara clasei, folosind `::`.

---

## Declarare vs. Implementare — de ce separăm?

În codul laboratorului, observi că în interiorul clasei apare doar **declararea** (semnătura) funcțiilor:

```cpp
class BankAccount {
public:
    void print();           // doar declarare — spunem CE există
};
```

Iar **implementarea** (corpul funcției, codul propriu-zis) apare în afara clasei:

```cpp
void BankAccount::print() {     // implementare — spunem CUM funcționează
    std::cout << "Id: " << id << '\n';
    // ...
}
```

De ce nu scriem totul direct în clasă? Sunt mai multe motive:

**Lizibilitatea:** Când clasa are 15 funcții, vrei să poți vedea rapid ce funcții oferă (interfața) fără să te pierzi în sute de linii de implementare. Declararea în clasă funcționează ca un „cuprins" — vezi dintr-o privire tot ce poate face un `BankAccount`.

**Organizarea codului în proiecte reale:** În practică, declararea clasei se pune într-un **fișier header** (`.h` sau `.hpp`), iar implementarea într-un **fișier sursă** (`.cpp`). Alte fișiere din proiect include doar header-ul — știu ce funcții există, fără să vadă implementarea. Asta reduce timpii de compilare și ascunde detaliile interne.

**Inline vs. non-inline:** Funcțiile definite **direct în clasă** sunt implicit `inline` (compilatorul poate înlocui apelul funcției cu corpul ei, pentru eficiență). Funcțiile definite **în afara clasei** nu sunt `inline` implicit. Pentru funcții scurte (1-2 linii), definirea în clasă e rezonabilă. Pentru funcții complexe, implementarea separată este regula.

**Notă:** Ambele variante produc cod corect. Alegerea e una de **stil, organizare și mentenanță**, nu de corectitudine.

---

## Pointer-ul `this`

În codul laboratorului apare frecvent `this->`:

```cpp
this->name = strcpy(new char[strlen(name) + 1], name);
this->accountHolder = accountHolder;
```

**`this`** este un pointer implicit disponibil în orice funcție membră non-statică. El **indică spre obiectul curent** — adică obiectul pe care a fost apelată funcția.

De ce e necesar aici? Parametrul constructorului se numește `name`, iar membrul clasei se numește tot `name`. Fără `this->`, compilatorul ar folosi variabila locală (parametrul) în ambele cazuri. Scriind `this->name`, specificăm explicit: „membrul `name` al obiectului curent, nu parametrul funcției".

`this` mai e folosit și în operatorul de atribuire:

```cpp
if (this == &obj) { return *this; }  // verificare auto-atribuire
return *this;                         // returnare obiect curent
```

`this` e un pointer, deci `*this` e obiectul în sine (dereferențiere).

---

## Recapitulare rapidă

| Concept | Ce este | Unde se vede în cod |
|---|---|---|
| **Clasă** | Șablon care descrie proprietăți + comportamente | `class BankAccount { ... };` |
| **Obiect / Instanță** | O realizare concretă a clasei, cu date proprii | `BankAccount x, y(...);` |
| **`private` / `public`** | Controlul accesului la membrii clasei | Secțiunile din clasă |
| **`static`** | Membru comun tuturor obiectelor, nu al unuia singur | `static int noAccounts;` |
| **`const`** | Membru care nu poate fi modificat după inițializare | `const int id;` |
| **Constructor** | Funcție apelată automat la crearea obiectului | `BankAccount()`, `BankAccount(char*, ...)` |
| **Copy constructor** | Constructor care creează un obiect ca copie a altuia | `BankAccount(const BankAccount &obj)` |
| **Destructor** | Funcție apelată automat la distrugerea obiectului | `~BankAccount()` |
| **`operator=`** | Suprascrie atribuirea pentru a face deep copy | `BankAccount& operator=(const BankAccount &obj)` |
| **`::`** | Spune compilatorului cui aparține un nume | `BankAccount::print()` |
| **`this`** | Pointer spre obiectul curent | `this->name = ...` |
| **Lista de inițializare** | Inițializare înainte de corpul constructorului | `: id(noAccounts++)` |
| **Declarare vs. implementare** | Separăm „ce face" de „cum face" | Semnătura în clasă, corp în afara clasei |
