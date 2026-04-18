#include "Angajat.h"
#include "Exceptii.h"

Angajat::Angajat() : nume(""), salariu(0) {}

Angajat::Angajat(string nume, double salariu) {
    if (nume.empty()) {
        throw NumeInvalidException();
    }
    if (salariu < 0) {
        throw SalariuInvalidException();
    }
    this->nume = nume;
    this->salariu = salariu;
}

Angajat::Angajat(const Angajat& obj) : nume(obj.nume), salariu(obj.salariu) {}

Angajat& Angajat::operator=(const Angajat& obj) {
    if (this != &obj) {
        nume = obj.nume;
        salariu = obj.salariu;
    }
    return *this;
}

Angajat::~Angajat() {}

string Angajat::getNume() const { return nume; }
double Angajat::getSalariu() const { return salariu; }

void Angajat::setNume(const string& nume) {
    if (nume.empty()) {
        throw NumeInvalidException();
    }
    this->nume = nume;
}

void Angajat::setSalariu(double salariu) {
    if (salariu < 0) {
        throw SalariuInvalidException();
    }
    this->salariu = salariu;
}

istream& operator>>(istream& in, Angajat& obj) {
    cout << "Nume: ";
    getline(in, obj.nume);
    if (obj.nume.empty()) {
        throw NumeInvalidException();
    }
    cout << "Salariu: ";
    in >> obj.salariu;
    in.ignore();
    if (obj.salariu < 0) {
        throw SalariuInvalidException();
    }
    return in;
}

ostream& operator<<(ostream& out, const Angajat& obj) {
    out << "Tip: " << obj.getTip() << endl;
    out << "Nume: " << obj.nume << endl;
    out << "Salariu: " << obj.salariu << endl;
    return out;
}

void Angajat::serializeaza(ostream& out) const {
    out << nume << endl;
    out << salariu << endl;
}

void Angajat::deserializeaza(istream& in) {
    getline(in, nume);
    in >> salariu;
    in.ignore();
}
