#ifndef EXCEPTII_H
#define EXCEPTII_H

#include <exception>
#include <string>

using namespace std;

class SalariuInvalidException : public exception {
    string mesaj;
public:
    SalariuInvalidException(const string& mesaj = "Salariul nu poate fi negativ!")
        : mesaj(mesaj) {}

    const char* what() const noexcept override {
        return mesaj.c_str();
    }
};

class NumeInvalidException : public exception {
    string mesaj;
public:
    NumeInvalidException(const string& mesaj = "Numele nu poate fi gol!")
        : mesaj(mesaj) {}

    const char* what() const noexcept override {
        return mesaj.c_str();
    }
};

class FisierException : public exception {
    string mesaj;
public:
    FisierException(const string& mesaj = "Fisierul nu poate fi deschis!")
        : mesaj(mesaj) {}

    const char* what() const noexcept override {
        return mesaj.c_str();
    }
};

#endif
