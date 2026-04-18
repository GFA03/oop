#include "Politist.h"

Politist::Politist() : Angajat(), grad("agent") {}

Politist::Politist(string nume, double salariu, string grad)
    : Angajat(nume, salariu), grad(grad) {}

Politist::Politist(const Politist& obj)
    : Angajat(obj), grad(obj.grad) {}

Politist& Politist::operator=(const Politist& obj) {
    if (this != &obj) {
        Angajat::operator=(obj);
        grad = obj.grad;
    }
    return *this;
}

Politist::~Politist() {}

string Politist::getGrad() const { return grad; }

void Politist::setGrad(const string& grad) {
    this->grad = grad;
}

istream& operator>>(istream& in, Politist& obj) {
    in >> (Angajat&)obj;
    cout << "Grad (agent/inspector/comisar): ";
    getline(in, obj.grad);
    return in;
}

ostream& operator<<(ostream& out, const Politist& obj) {
    out << (const Angajat&)obj;
    out << "Grad: " << obj.grad << endl;
    return out;
}

double Politist::calculeazaPrima() {
    if (grad == "comisar") {
        return getSalariu() * 0.30;
    } else if (grad == "inspector") {
        return getSalariu() * 0.20;
    } else {
        return getSalariu() * 0.10;
    }
}

string Politist::getTip() const {
    return "Politist";
}

void Politist::serializeaza(ostream& out) const {
    Angajat::serializeaza(out);
    out << grad << endl;
}

void Politist::deserializeaza(istream& in) {
    Angajat::deserializeaza(in);
    getline(in, grad);
}
