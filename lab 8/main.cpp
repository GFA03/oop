#include <iostream>
#include <fstream>
#include <vector>
#include "Angajat.h"
#include "Avocat.h"
#include "Politist.h"
#include "Judecator.h"
#include "Exceptii.h"

using namespace std;

const string FISIER = "angajati.txt";

Angajat* creeazaAngajat(const string& tip) {
    if (tip == "Avocat") return new Avocat();
    if (tip == "Politist") return new Politist();
    if (tip == "Judecator") return new Judecator();
    return nullptr;
}

void salveazaInFisier(const vector<Angajat*>& angajati) {
    ofstream fout(FISIER);
    if (!fout.is_open()) {
        throw FisierException();
    }
    fout << angajati.size() << endl;
    for (const auto& a : angajati) {
        fout << a->getTip() << endl;
        a->serializeaza(fout);
    }
    fout.close();
    cout << "Datele au fost salvate in " << FISIER << endl;
}

void incarcaDinFisier(vector<Angajat*>& angajati) {
    ifstream fin(FISIER);
    if (!fin.is_open()) {
        throw FisierException();
    }

    // eliberam angajatii existenti
    for (auto& a : angajati) {
        delete a;
    }
    angajati.clear();

    int n;
    fin >> n;
    fin.ignore();

    for (int i = 0; i < n; i++) {
        string tip;
        getline(fin, tip);
        Angajat* a = creeazaAngajat(tip);
        if (a != nullptr) {
            a->deserializeaza(fin);
            angajati.push_back(a);
        }
    }
    fin.close();
    cout << "Au fost incarcati " << angajati.size() << " angajati din " << FISIER << endl;
}

void afiseazaMeniu() {
    cout << endl;
    cout << "=== Sistem Calculare Prima ===" << endl;
    cout << "1. Adauga Avocat" << endl;
    cout << "2. Adauga Politist" << endl;
    cout << "3. Adauga Judecator" << endl;
    cout << "4. Afiseaza toti angajatii" << endl;
    cout << "5. Calculeaza prima pentru toti" << endl;
    cout << "6. Salveaza in fisier" << endl;
    cout << "7. Incarca din fisier" << endl;
    cout << "0. Iesire (salvare automata)" << endl;
    cout << "Optiune: ";
}

int main() {
    vector<Angajat*> angajati;

    // incercare incarcare automata la pornire
    try {
        incarcaDinFisier(angajati);
    } catch (FisierException& e) {
        cout << "Nu exista date salvate anterior. Se porneste fresh." << endl;
    }

    int optiune;
    do {
        afiseazaMeniu();
        cin >> optiune;
        cin.ignore();

        switch (optiune) {
            case 1: {
                Avocat* a = new Avocat();
                try {
                    cin >> *a;
                    angajati.push_back(a);
                    cout << "Avocat adaugat cu succes!" << endl;
                } catch (exception& e) {
                    cout << "Eroare: " << e.what() << endl;
                    delete a;
                }
                break;
            }
            case 2: {
                Politist* p = new Politist();
                try {
                    cin >> *p;
                    angajati.push_back(p);
                    cout << "Politist adaugat cu succes!" << endl;
                } catch (exception& e) {
                    cout << "Eroare: " << e.what() << endl;
                    delete p;
                }
                break;
            }
            case 3: {
                Judecator* j = new Judecator();
                try {
                    cin >> *j;
                    angajati.push_back(j);
                    cout << "Judecator adaugat cu succes!" << endl;
                } catch (exception& e) {
                    cout << "Eroare: " << e.what() << endl;
                    delete j;
                }
                break;
            }
            case 4: {
                if (angajati.empty()) {
                    cout << "Nu exista angajati." << endl;
                } else {
                    cout << endl << "--- Lista Angajati ---" << endl;
                    for (int i = 0; i < (int)angajati.size(); i++) {
                        cout << "Angajat #" << i + 1 << ":" << endl;
                        cout << *angajati[i] << endl;
                    }
                }
                break;
            }
            case 5: {
                if (angajati.empty()) {
                    cout << "Nu exista angajati." << endl;
                } else {
                    cout << endl << "--- Prima pentru fiecare angajat ---" << endl;
                    for (int i = 0; i < (int)angajati.size(); i++) {
                        cout << angajati[i]->getNume()
                             << " (" << angajati[i]->getTip() << ")"
                             << " - Prima: " << angajati[i]->calculeazaPrima()
                             << " RON" << endl;
                    }
                }
                break;
            }
            case 6: {
                try {
                    salveazaInFisier(angajati);
                } catch (FisierException& e) {
                    cout << "Eroare: " << e.what() << endl;
                }
                break;
            }
            case 7: {
                try {
                    incarcaDinFisier(angajati);
                } catch (FisierException& e) {
                    cout << "Eroare: " << e.what() << endl;
                }
                break;
            }
            case 0: {
                try {
                    salveazaInFisier(angajati);
                    cout << "Salvare automata efectuata." << endl;
                } catch (FisierException& e) {
                    cout << "Eroare la salvare: " << e.what() << endl;
                }
                cout << "La revedere!" << endl;
                break;
            }
            default:
                cout << "Optiune invalida!" << endl;
        }
    } while (optiune != 0);

    // eliberam memoria
    for (auto& a : angajati) {
        delete a;
    }
    angajati.clear();

    return 0;
}
