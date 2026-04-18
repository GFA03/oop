#include "Avocat.h"

Avocat::Avocat() : Angajat(), nrCazuriCastigate(0) {}

Avocat::Avocat(string nume, double salariu, int nrCazuriCastigate)
    : Angajat(nume, salariu), nrCazuriCastigate(nrCazuriCastigate) {}

Avocat::Avocat(const Avocat& obj)
    : Angajat(obj), nrCazuriCastigate(obj.nrCazuriCastigate) {}

Avocat& Avocat::operator=(const Avocat& obj) {
    if (this != &obj) {
        Angajat::operator=(obj);
        nrCazuriCastigate = obj.nrCazuriCastigate;
    }
    return *this;
}

Avocat::~Avocat() {}

int Avocat::getNrCazuriCastigate() const { return nrCazuriCastigate; }

void Avocat::setNrCazuriCastigate(int nrCazuriCastigate) {
    this->nrCazuriCastigate = nrCazuriCastigate;
}

istream& operator>>(istream& in, Avocat& obj) {
    in >> (Angajat&)obj;
    cout << "Numar cazuri castigate: ";
    in >> obj.nrCazuriCastigate;
    in.ignore();
    return in;
}

ostream& operator<<(ostream& out, const Avocat& obj) {
    out << (const Angajat&)obj;
    out << "Numar cazuri castigate: " << obj.nrCazuriCastigate << endl;
    return out;
}

double Avocat::calculeazaPrima() {
    return getSalariu() * 0.1 * (nrCazuriCastigate / 10.0);
}

string Avocat::getTip() const {
    return "Avocat";
}

void Avocat::serializeaza(ostream& out) const {
    Angajat::serializeaza(out);
    out << nrCazuriCastigate << endl;
}

void Avocat::deserializeaza(istream& in) {
    Angajat::deserializeaza(in);
    in >> nrCazuriCastigate;
    in.ignore();
}
