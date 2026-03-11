# Laborator 3 — Supraîncărcarea Operatorilor, Funcții Friend și Metode

## Cuprins

- [Ce e nou în Laboratorul 3?](#ce-e-nou-în-laboratorul-3)
- [Supraîncărcarea operatorilor — ideea generală](#supraîncărcarea-operatorilor--ideea-generală)
- [Funcții friend (`friend`)](#funcții-friend-friend)
- [Supraîncărcarea `operator<<` (ieșire)](#supraîncărcarea-operator-ieșire)
- [Supraîncărcarea `operator>>` (intrare)](#supraîncărcarea-operator-intrare)
- [Metode comportamentale](#metode-comportamentale)
  - [`deposit` și `withdraw`](#deposit-și-withdraw)
  - [`addDeposit`](#adddeposit)
- [Metode private ajutătoare](#metode-private-ajutătoare)
- [Pre-incrementare vs. Post-incrementare (`++i` vs. `i++`)](#pre-incrementare-vs-post-incrementare-i-vs-i)
- [Recapitulare rapidă](#recapitulare-rapidă)

---

## Ce e nou în Laboratorul 3?

Laboratorul 2 a introdus structura de bază a unei clase în C++: specificatori de acces, membri statici și constanți, constructori, destructor și `operator=`. Laboratorul 3 construiește pe această fundație adăugând:

- **Supraîncărcarea operatorilor** — definirea unui comportament propriu pentru operatori deja existenți (`<<`, `>>`) aplicați tipului tău.
- **Funcții friend** — funcții nemembre care primesc acces la membrii `private` ai clasei.
- **Metode comportamentale** — funcții care implementează operații din lumea reală (`deposit`, `withdraw`, `addDeposit`).
- **Metode private ajutătoare** — utilități interne care reduc duplicarea codului.
- **Pre-incrementare vs. post-incrementare** — o distincție subtilă, dar importantă, în funcționarea operatorului `++`.

---

## Supraîncărcarea operatorilor — ideea generală

În C++, operatorii precum `+`, `=`, `<<` și `>>` sunt, la bază, funcții cu sintaxă specială. Expresia `a + b` este, în culise, un apel la `operator+(a, b)`. Deoarece sunt funcții, pot fi **supraîncărcați** (overloaded) — adică poți defini ce înseamnă pentru propriile tipuri.

Fără supraîncărcare, dacă scrii:

```cpp
BankAccount acc("Economii", "Alex");
std::cout << acc;  // EROARE — compilatorul nu știe cum să afișeze un BankAccount
```

compilatorul nu știe cum să procedeze. Supraîncărcând `operator<<`, îl înveți.

Semnătura generală pentru un operator binar supraîncărcat ca funcție nemembră este:

```cpp
TipReturn operator@(TipStanga lhs, TipDreapta rhs);
```

unde `@` este simbolul operatorului. Pentru `operator<<`:

```cpp
std::ostream& operator<<(std::ostream& os, const BankAccount& obj);
```

Regulile de bază:
- **Nu** poți inventa operatori noi — poți supraîncărca doar pe cei existenți.
- **Nu** poți schimba precedența sau asociativitatea operatorilor.
- Cel puțin un operand trebuie să fie de tip definit de utilizator (nu poți supraîncărca `+` pentru doi `int`).

---

## Funcții friend (`friend`)

`operator<<` și `operator>>` au nevoie de acces la membrii `private` ai lui `BankAccount` (cum ar fi `id`, `name`, `balance` etc.), dar **nu sunt funcții membre** ale clasei — sunt funcții de sine stătătoare. În mod normal, o funcție nemembră nu poate accesa membrii `private`. Cuvântul cheie `friend` rezolvă această problemă:

```cpp
class BankAccount {
    // ...
    friend std::ostream& operator<<(std::ostream&, const BankAccount&);
    friend std::istream& operator>>(std::istream&, BankAccount&);
};
```

Declarând o funcție ca `friend` în interiorul clasei, îi acordăm acces complet la toți membrii `private` și `protected` ai clasei, ca și cum ar fi o funcție membră. Declarația `friend` apare în corpul clasei, dar funcția în sine este definită în afară și **nu** este membră — nu primește un pointer `this` și nu se apelează cu notația punct.

Gândește-te astfel: clasa alege ea cine îi sunt prietenii. La fel cum ai putea da unui prieten de încredere cheia de la casă, clasa îi oferă funcției friend acces la datele sale private.

**De ce nu facem `operator<<` funcție membră?**

Pentru `operator<<`, operandul din stânga este `std::ostream` (de exemplu `std::cout`), nu un `BankAccount`. Dacă ar fi funcție membră a lui `BankAccount`, s-ar apela ca `acc.operator<<(std::cout)`, ceea ce ar impune sintaxa `acc << std::cout` — exact opusul a ce vrem. Ca funcție nemembră friend, ambii operanzi sunt parametri expliciti și sintaxa naturală `std::cout << acc` funcționează corect.

---

## Supraîncărcarea `operator<<` (ieșire)

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

Funcția primește o referință la `std::ostream` și o referință `const` la obiect, scrie date formatate în flux și **returnează fluxul prin referință**. Returnarea lui `os` permite înlănțuirea:

```cpp
std::cout << acc1 << '\n' << acc2;
// echivalent cu: (std::cout << acc1) << '\n' << acc2;
// rezultatul primului << este chiar std::cout, deci înlănțuirea funcționează
```

Al doilea parametru este `const BankAccount&` — o referință constantă — deoarece afișarea nu ar trebui să modifice obiectul. Ampersand-ul (`&`) evită o copie inutilă.

---

## Supraîncărcarea `operator>>` (intrare)

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

    // ... (sold, tip investiție, depuneri) ...

    return is;
}
```

`operator>>` este simetric cu `operator<<`: primește o referință la `std::istream` și o referință **non-const** la obiect (deoarece îl modifică, completând câmpurile din flux) și returnează fluxul prin referință pentru a permite înlănțuirea:

```cpp
std::cin >> acc1 >> acc2;
```

Observă că `operator>>` folosește setter-ele publice (`setName`, `setAccountHolder` etc.) în loc să acceseze direct membrii privați. Acest lucru este valid — dar deoarece funcția este declarată `friend`, ar putea accesa și direct membrii privați. Folosirea setter-elor este o alegere stilistică ce reutilizează logica de validare existentă.

---

## Metode comportamentale

Dincolo de stocarea și afișarea datelor, o clasă ar trebui să modeleze și **acțiunile** pe care entitatea din lumea reală le poate efectua. Laboratorul 3 adaugă trei astfel de metode.

### `deposit` și `withdraw`

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

Aceste metode aplică **validarea intrărilor** înainte de a modifica starea. Tiparul este:
1. Verifică precondiția; raportează și ieși timpuriu dacă nu e îndeplinită.
2. Efectuează operația.

Aceasta este încapsularea în acțiune — apelantul nu poate modifica `balance` direct, deci singura cale este prin aceste funcții controlate, care impun regula că soldul nu poate deveni negativ.

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

`addDeposit` face mai mult decât `deposit` — **înregistrează** și depunerea în array-ul istoric. Deoarece array-ul are o dimensiune fixă, mărirea lui necesită:
1. Alocarea unui nou array mai mare.
2. Copierea elementelor existente.
3. Adăugarea valorii noi la final.
4. Eliberarea array-ului vechi și înlocuirea pointer-ului.

Acesta este tiparele clasice de creștere a unui array dinamic. Observă că memoria veche este eliberată cu `delete[]` înainte de a fi înlocuit pointer-ul — uitarea acestui pas ar cauza un memory leak.

---

## Metode private ajutătoare

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

Logica pentru copierea profundă a unui array de depuneri este necesară în mai multe locuri: constructorul cu parametri, constructorul de copiere și `operator=`. În loc să duplicăm acele linii de trei ori (ceea ce ar însemna trei locuri de actualizat dacă logica s-ar schimba), codul o extrage într-o **metodă privată ajutătoare** `copyDeposits`.

Este `private` deoarece este un detaliu intern de implementare — are sens doar în contextul construirii sau atribuirii unui `BankAccount`, iar expunerea ei public ar fi înșelătoare.

Principiul aplicat aici este **DRY** — *Don't Repeat Yourself* (Nu te repeta). Codul duplicat este un risc de mentenanță: dacă găsești un bug în logica de copiere, ar trebui să-l repari în trei locuri în loc de unul.

---

## Pre-incrementare vs. Post-incrementare (`++i` vs. `i++`)

În constructori, `noAccounts` este incrementat pentru a atribui un ID unic fiecărui obiect:

```cpp
BankAccount::BankAccount() : id(++noAccounts) { ... }
```

De ce `++noAccounts` și nu `noAccounts++`? Diferența constă în **valoarea la care se evaluează expresia**:

- `noAccounts++` — **post-incrementare**: se evaluează la valoarea *curentă*, apoi incrementează. Dacă `noAccounts` pornește de la `0`, primul obiect ar primi `id = 0`.
- `++noAccounts` — **pre-incrementare**: incrementează mai întâi, apoi se evaluează la *noua* valoare. Dacă `noAccounts` pornește de la `0`, primul obiect primește `id = 1`.

Combinat cu inițializarea `int BankAccount::noAccounts = 0;`, folosirea `++noAccounts` înseamnă că ID-urile pornesc de la `1` — o numerotare mai naturală pentru identificatorii de cont. Folosind `noAccounts++` cu valoarea inițială `0` s-ar atribui ID `0` primului cont, ceea ce este mai puțin intuitiv; ar trebui să pornești de la `1` pentru ca primul obiect să primească ID `1`.

Pentru tipuri simple (`int`), compilatorul optimizează ambele la același cod mașină când valoarea de return este ignorată (de exemplu, o instrucțiune standalone `i++;`). Dar într-o expresie din lista de inițializare — unde valoarea *este* folosită — distincția contează. Preferă `++i` când folosești rezultatul, deoarece e mai explicit ca intenție și, pentru tipuri definite de utilizator care supraîncarcă `++`, evită crearea unui temporar inutil.

---

## Recapitulare rapidă

| Concept | Ce este | Unde se vede în cod |
|---|---|---|
| **Supraîncărcarea operatorilor** | Definirea unui comportament propriu al operatorilor pentru tipuri definite de utilizator | `operator<<`, `operator>>` |
| **Funcție `friend`** | Funcție nemembră cu acces la membrii `private` | `friend std::ostream& operator<<(...)` |
| **`operator<<`** | Operator de ieșire supraîncărcat; scrie obiectul într-un flux | `std::cout << account;` |
| **`operator>>`** | Operator de intrare supraîncărcat; citește obiectul dintr-un flux | `std::cin >> account;` |
| **Metode comportamentale** | Funcții membre care implementează acțiuni din lumea reală | `deposit()`, `withdraw()`, `addDeposit()` |
| **Validarea intrărilor** | Verificarea precondiților înainte de modificarea stării | `if (amount <= 0) { ... return; }` |
| **Creșterea array-ului dinamic** | Alochează array mai mare, copiază, adaugă, eliberează vechiul | În interiorul `addDeposit()` |
| **Metodă privată ajutătoare** | Utilitar intern pentru evitarea duplicării codului | `copyDeposits()` |
| **Principiul DRY** | Nu te repeta — extrage logica comună | `copyDeposits` folosit în 3 constructori |
| **Pre-incrementare (`++i`)** | Incrementează mai întâi, returnează noua valoare | `: id(++noAccounts)` |
| **Post-incrementare (`i++`)** | Returnează valoarea curentă, apoi incrementează | (evitată aici) |
