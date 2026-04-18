#ifndef JUDECATOR_H
#define JUDECATOR_H

#include "Angajat.h"

class Judecator : public Angajat {
    int aniExperienta;
public:
    Judecator();
    Judecator(string nume, double salariu, int aniExperienta);
    Judecator(const Judecator& obj);
    Judecator& operator=(const Judecator& obj);
    ~Judecator();

    int getAniExperienta() const;
    void setAniExperienta(int aniExperienta);

    friend istream& operator>>(istream& in, Judecator& obj);
    friend ostream& operator<<(ostream& out, const Judecator& obj);

    double calculeazaPrima() override;
    string getTip() const override;

    void serializeaza(ostream& out) const override;
    void deserializeaza(istream& in) override;
};

#endif
