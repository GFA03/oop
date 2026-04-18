#ifndef POLITIST_H
#define POLITIST_H

#include "Angajat.h"

class Politist : public Angajat {
    string grad; // "agent", "inspector", "comisar"
public:
    Politist();
    Politist(string nume, double salariu, string grad);
    Politist(const Politist& obj);
    Politist& operator=(const Politist& obj);
    ~Politist();

    string getGrad() const;
    void setGrad(const string& grad);

    friend istream& operator>>(istream& in, Politist& obj);
    friend ostream& operator<<(ostream& out, const Politist& obj);

    double calculeazaPrima() override;
    string getTip() const override;

    void serializeaza(ostream& out) const override;
    void deserializeaza(istream& in) override;
};

#endif
