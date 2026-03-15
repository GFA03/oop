#include <iostream>
#include <string.h>

class BankAccount {
private:
    static int noAccounts;
    const int id;
    char* name;
    std::string accountHolder;
    double balance;
    bool isInvestmentAccount;
    int noDeposits;
    int* deposits;

    // private helper - deep copies a deposit array
    void copyDeposits(int noDeposits, int* deposits);

public:
    // constructors & destructor
    BankAccount();
    BankAccount(char*, std::string);
    BankAccount(char*, std::string, double, bool, int, int*);
    BankAccount(const BankAccount& obj);
    BankAccount& operator=(const BankAccount& obj);
    ~BankAccount();

    // getters
    int getId() const;
    const char* getName() const;
    std::string getAccountHolder() const;
    double getBalance() const;
    bool getIsInvestmentAccount() const;
    int getNoDeposits() const;
    const int* getDeposits() const;

    // setters
    void setName(char*);
    void setAccountHolder(std::string);
    void setBalance(double);
    void setIsInvestmentAccount(bool);
    void setDeposits(int, int*);

    // operators
    friend std::ostream& operator<<(std::ostream&, const BankAccount&);
    friend std::istream& operator>>(std::istream&, BankAccount&);

    // methods
    void deposit(int amount);
    void withdraw(int amount);
    void addDeposit(int amount);
};

int BankAccount::noAccounts = 0;

// ─── private helper ────────────────────────────────────────────────────────────

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

// ─── constructors ──────────────────────────────────────────────────────────────

BankAccount::BankAccount() : id(++noAccounts) {
    name = strcpy(new char[4], "N/A");
    accountHolder = "N/A";
    balance = 0.0;
    isInvestmentAccount = false;
    noDeposits = 0;
    deposits = nullptr;
}

BankAccount::BankAccount(char* name, std::string accountHolder) : id(++noAccounts) {
    this->name = strcpy(new char[strlen(name) + 1], name);
    this->accountHolder = accountHolder;
    balance = 0.0;
    isInvestmentAccount = false;
    noDeposits = 0;
    deposits = nullptr;
}

BankAccount::BankAccount(char* name, std::string accountHolder, double balance, bool isInvestmentAccount, int noDeposits, int* deposits) : id(++noAccounts) {
    this->name = strcpy(new char[strlen(name) + 1], name);
    this->accountHolder = accountHolder;
    this->balance = balance;
    this->isInvestmentAccount = isInvestmentAccount;
    copyDeposits(noDeposits, deposits);
}

BankAccount::BankAccount(const BankAccount& obj) : id(++noAccounts) {
    name = strcpy(new char[strlen(obj.name) + 1], obj.name);
    accountHolder = obj.accountHolder;
    balance = obj.balance;
    isInvestmentAccount = obj.isInvestmentAccount;
    copyDeposits(obj.noDeposits, obj.deposits);
}

// ─── operator= ────────────────────────────────────────────────────────────────

BankAccount& BankAccount::operator=(const BankAccount& obj) {
    if (this == &obj)
        return *this;

    delete[] name;
    name = strcpy(new char[strlen(obj.name) + 1], obj.name);

    accountHolder = obj.accountHolder;
    balance = obj.balance;
    isInvestmentAccount = obj.isInvestmentAccount;

    delete[] deposits;
    copyDeposits(obj.noDeposits, obj.deposits);

    return *this;
}

// ─── destructor ───────────────────────────────────────────────────────────────

BankAccount::~BankAccount() {
    delete[] name;
    delete[] deposits;
}

// ─── getters ──────────────────────────────────────────────────────────────────

int BankAccount::getId() const {
    return id;
}

// returns const char* so the caller cannot modify internal memory
const char* BankAccount::getName() const {
    return name;
}

std::string BankAccount::getAccountHolder() const {
    return accountHolder;
}

double BankAccount::getBalance() const {
    return balance;
}

bool BankAccount::getIsInvestmentAccount() const {
    return isInvestmentAccount;
}

int BankAccount::getNoDeposits() const {
    return noDeposits;
}

// returns const int* so the caller cannot modify the internal array
const int* BankAccount::getDeposits() const {
    return deposits;
}

// ─── setters ──────────────────────────────────────────────────────────────────

void BankAccount::setName(char* name) {
    delete[] this->name;
    this->name = strcpy(new char[strlen(name) + 1], name);
}

void BankAccount::setAccountHolder(std::string accountHolder) {
    this->accountHolder = accountHolder;
}

void BankAccount::setBalance(double balance) {
    this->balance = balance;
}

void BankAccount::setIsInvestmentAccount(bool isInvestmentAccount) {
    this->isInvestmentAccount = isInvestmentAccount;
}

// deep copies the new deposit array, frees the old one
void BankAccount::setDeposits(int noDeposits, int* deposits) {
    delete[] this->deposits;
    copyDeposits(noDeposits, deposits);
}

// ─── operator<< ───────────────────────────────────────────────────────────────

std::ostream& operator<<(std::ostream& os, const BankAccount& obj) {
    os << "Id: "              << obj.id             << '\n';
    os << "Name: "            << obj.name           << '\n';
    os << "Account Holder: "  << obj.accountHolder  << '\n';
    os << "Balance: "         << obj.balance        << '\n';
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

// ─── operator>> ───────────────────────────────────────────────────────────────

std::istream& operator>>(std::istream& is, BankAccount& obj) {
    char buf[256];

    std::cout << "Name: ";
    is >> buf;
    obj.setName(buf);

    std::cout << "Account holder: ";
    std::string holder;
    is >> holder;
    obj.setAccountHolder(holder);

    std::cout << "Balance: ";
    double balance;
    is >> balance;
    obj.setBalance(balance);

    std::cout << "Investment account (1/0): ";
    bool isInv;
    is >> isInv;
    obj.setIsInvestmentAccount(isInv);

    std::cout << "Number of deposits: ";
    int n;
    is >> n;
    if (n > 0) {
        int* deps = new int[n];
        for (int i = 0; i < n; i++) {
            std::cout << "  Deposit " << i + 1 << ": ";
            is >> deps[i];
        }
        obj.setDeposits(n, deps);
        delete[] deps;
    }

    return is;
}

// ─── methods ──────────────────────────────────────────────────────────────────

// adds amount directly to balance
void BankAccount::deposit(int amount) {
    if (amount <= 0) {
        std::cout << "Deposit amount must be positive.\n";
        return;
    }
    balance += amount;
}

// subtracts amount from balance if sufficient funds
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

// records a deposit in the deposits array and adds to balance
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

// ─── main ─────────────────────────────────────────────────────────────────────

int main() {
    int deps[] = {100, 200};
    BankAccount account("Savings", "Alex Gavrila", 500.0, false, 2, deps);

    int option;
    while (true) {
        std::cout << "\n=== Bank Account Menu ===\n";
        std::cout << "1. Print account\n";
        std::cout << "2. Deposit\n";
        std::cout << "3. Withdraw\n";
        std::cout << "4. Add tracked deposit\n";
        std::cout << "5. Read account from keyboard\n";
        std::cout << "6. Change name\n";
        std::cout << "7. Change balance\n";
        std::cout << "0. Exit\n";
        std::cout << "Option: ";
        std::cin >> option;

        if (option == 0) break;

        switch (option) {
            case 1:
                std::cout << '\n' << account;
                break;

            case 2: {
                int amount;
                std::cout << "Amount to deposit: ";
                std::cin >> amount;
                account.deposit(amount);
                break;
            }

            case 3: {
                int amount;
                std::cout << "Amount to withdraw: ";
                std::cin >> amount;
                account.withdraw(amount);
                break;
            }

            case 4: {
                int amount;
                std::cout << "Deposit amount to record: ";
                std::cin >> amount;
                account.addDeposit(amount);
                break;
            }

            case 5:
                std::cin >> account;
                break;

            case 6: {
                char buf[256];
                std::cout << "New name: ";
                std::cin >> buf;
                account.setName(buf);
                break;
            }

            case 7: {
                double bal;
                std::cout << "New balance: ";
                std::cin >> bal;
                account.setBalance(bal);
                break;
            }

            default:
                std::cout << "Invalid option.\n";
        }
    }

    return 0;
}