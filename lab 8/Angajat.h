#ifndef ANGAJAT_H
#define ANGAJAT_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Angajat {
    string nume;
    double salariu;
public:
    Angajat();
    Angajat(string nume, double salariu);
    Angajat(const Angajat& obj);
    Angajat& operator=(const Angajat& obj);
    virtual ~Angajat();

    string getNume() const;
    double getSalariu() const;
    void setNume(const string& nume);
    void setSalariu(double salariu);

    friend istream& operator>>(istream& in, Angajat& obj);
    friend ostream& operator<<(ostream& out, const Angajat& obj);

    virtual double calculeazaPrima() = 0;
    virtual string getTip() const = 0;

    virtual void serializeaza(ostream& out) const;
    virtual void deserializeaza(istream& in);
};

#endif
