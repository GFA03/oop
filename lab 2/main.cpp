#include <iostream>
#include <string.h>

class BankAccount {
private:
    static int noAccounts; // number of accounts
    const int id;
    char* name;
    std::string accountHolder;
    double balance;
    bool isInvestmentAccount;
    int noDeposits;
    int* deposits;
public:
    BankAccount();
    BankAccount(char*, std::string);
    BankAccount(char*, std::string, double, bool, int, int*);
    BankAccount(const BankAccount &obj);
    BankAccount& operator=(const BankAccount &obj);
    ~BankAccount();

    // methods
    void print();
};

int BankAccount::noAccounts = 1;

BankAccount::BankAccount() :id(noAccounts++) {
    name = strcpy(new char[4], "N/A");
    accountHolder = "N/A";
    balance = 0.0;
    isInvestmentAccount = false;
    noDeposits = 0;
    deposits = nullptr;
}

BankAccount::BankAccount(char* name, std::string accountHolder) : id(noAccounts++) {
    this->name = strcpy(new char[strlen(name) + 1], name);
    this->accountHolder = accountHolder;
    balance = 0.0;
    isInvestmentAccount = false;
    noDeposits = 0;
    deposits = nullptr;
}

BankAccount::BankAccount(char* name, std::string accountHolder, double balance, bool isInvestmentAccount, int noDeposits, int* deposits) : id(noAccounts++) {
    this->name = strcpy(new char[strlen(name) + 1], name);
    this->accountHolder = accountHolder;
    this->balance = balance;
    this->isInvestmentAccount = isInvestmentAccount;
    this->noDeposits = noDeposits;
    if (noDeposits > 0) {
        this->deposits = new int[noDeposits];
        for (int i = 0; i < noDeposits; i++) {
            this->deposits[i] = deposits[i];
        }
    } else {
        this->deposits = nullptr;
    }
}

BankAccount::BankAccount(const BankAccount &obj): id(noAccounts++) {
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

BankAccount& BankAccount::operator=(const BankAccount &obj) {
    if (this == &obj) {
        return *this;
    }
    
    if (this->name != nullptr) {
        delete[] this->name;
        this->name = nullptr;
    }
    this->name = new char[strlen(obj.name) + 1];
    strcpy(this->name, obj.name);
    this->accountHolder = obj.accountHolder;
    this->balance = obj.balance;
    this->isInvestmentAccount = obj.isInvestmentAccount;
    this->noDeposits = obj.noDeposits;
    if (this->deposits != nullptr) {
        delete[] this->deposits;
        this->deposits = nullptr;
    }
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

BankAccount::~BankAccount() {
    if (name != nullptr) {
        delete[] name;
        this->name = nullptr;
    }

    if (deposits != nullptr) {
        delete[] deposits;
        this->deposits = nullptr;
    }
}

void BankAccount::print() {
    std::cout << "Id: " << id << '\n';
    if (name) {
        std::cout << "Name: " << name << '\n';
    }
    std::cout << "Account Holder: " << accountHolder << '\n';
    std::cout << "Balance: " << balance << '\n';
    std::cout << "Is Investment Account: " << (isInvestmentAccount ? "Yes" : "No") << '\n';
    if (deposits) {
        std::cout << "Deposits: ";
        for (int i = 0; i < this->noDeposits; i++) {
            std::cout << deposits[i] << " ";
        }
    }
    std::cout << '\n';
}


int main() {
    int deps[] = {3, 4};
    BankAccount x, y("my personal account", "Alex Gavrila", 200.0, false, 2, deps);
    x.print();
}