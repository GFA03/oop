#include "Judecator.h"

Judecator::Judecator() : Angajat(), aniExperienta(0) {}

Judecator::Judecator(string nume, double salariu, int aniExperienta)
    : Angajat(nume, salariu), aniExperienta(aniExperienta) {}

Judecator::Judecator(const Judecator& obj)
    : Angajat(obj), aniExperienta(obj.aniExperienta) {}

Judecator& Judecator::operator=(const Judecator& obj) {
    if (this != &obj) {
        Angajat::operator=(obj);
        aniExperienta = obj.aniExperienta;
    }
    return *this;
}

Judecator::~Judecator() {}

int Judecator::getAniExperienta() const { return aniExperienta; }

void Judecator::setAniExperienta(int aniExperienta) {
    this->aniExperienta = aniExperienta;
}

istream& operator>>(istream& in, Judecator& obj) {
    in >> (Angajat&)obj;
    cout << "Ani experienta: ";
    in >> obj.aniExperienta;
    in.ignore();
    return in;
}

ostream& operator<<(ostream& out, const Judecator& obj) {
    out << (const Angajat&)obj;
    out << "Ani experienta: " << obj.aniExperienta << endl;
    return out;
}

double Judecator::calculeazaPrima() {
    return getSalariu() * 0.05 * aniExperienta;
}

string Judecator::getTip() const {
    return "Judecator";
}

void Judecator::serializeaza(ostream& out) const {
    Angajat::serializeaza(out);
    out << aniExperienta << endl;
}

void Judecator::deserializeaza(istream& in) {
    Angajat::deserializeaza(in);
    in >> aniExperienta;
    in.ignore();
}
