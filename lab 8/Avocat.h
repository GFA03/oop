#ifndef AVOCAT_H
#define AVOCAT_H

#include "Angajat.h"

class Avocat : public Angajat {
    int nrCazuriCastigate;
public:
    Avocat();
    Avocat(string nume, double salariu, int nrCazuriCastigate);
    Avocat(const Avocat& obj);
    Avocat& operator=(const Avocat& obj);
    ~Avocat();

    int getNrCazuriCastigate() const;
    void setNrCazuriCastigate(int nrCazuriCastigate);

    friend istream& operator>>(istream& in, Avocat& obj);
    friend ostream& operator<<(ostream& out, const Avocat& obj);

    double calculeazaPrima() override;
    string getTip() const override;

    void serializeaza(ostream& out) const override;
    void deserializeaza(istream& in) override;
};

#endif
