#include <iostream>
#include <string.h>
#include <vector>

class Animal {
    static int noAnimals;
    const int id;
    std::string name;
    std::string breed;
    int noLegs;

public:
    Animal();
    Animal(std::string name, std::string breed, int noLegs);
    Animal(const Animal& obj);
    Animal& operator=(const Animal& obj);

    int getId() const;

    bool operator==(const Animal& other) const;
    bool operator<(const Animal& other) const;

    friend std::istream& operator>>(std::istream& in, Animal& obj);
    friend std::ostream& operator<<(std::ostream& out, const Animal& obj);
};

int Animal::noAnimals = 0;

Animal::Animal() : id(++noAnimals) {
    this->name = "N/A";
    this->breed = "N/A";
    this->noLegs = 0;
}

Animal::Animal(std::string name, std::string breed, int noLegs) : id(++noAnimals) {
    this->name = name;
    this->breed = breed;
    this->noLegs = noLegs;
}

Animal::Animal(const Animal& obj) : id(++noAnimals) {
    this->name = obj.name;
    this->breed = obj.breed;
    this->noLegs = obj.noLegs;
}

Animal& Animal::operator=(const Animal& obj) {
    if (this == &obj)
        return *this;
    this->name = obj.name;
    this->breed = obj.breed;
    this->noLegs = obj.noLegs;
    return *this;
}

int Animal::getId() const {
    return id;
}

// two animals are equal if same breed and number of legs
bool Animal::operator==(const Animal& other) const {
    return this->breed == other.breed && this->noLegs == other.noLegs;
}

// sort animals alphabetically by name
bool Animal::operator<(const Animal& other) const {
    return this->name < other.name;
}

std::istream& operator>>(std::istream& in, Animal& obj) {
    char buff[256];

    std::cout << "  Name: ";
    in.getline(buff, 256);
    obj.name = buff;

    std::cout << "  Breed: ";
    in.getline(buff, 256);
    obj.breed = buff;

    std::cout << "  Number of legs: ";
    in >> obj.noLegs;
    in.ignore();

    return in;
}

std::ostream& operator<<(std::ostream& out, const Animal& obj) {
    out << "  [Animal #" << obj.id << "] "
        << obj.name << " | " << obj.breed
        << " | " << obj.noLegs << " legs";
    return out;
}

// ─────────────────────────────────────────────────────────────────────────────

class Person {
    static int noPersons;
    const int id;
    char* name;
    int satisfaction;
    Animal* pet;

    void copyName(const char* src);

public:
    Person();
    Person(char* name);
    Person(char* name, int satisfaction);
    Person(char* name, int satisfaction, Animal* pet);
    Person(const Person& obj);
    Person& operator=(const Person& obj);
    ~Person();

    void assignPet(Animal* animal);
    void removePet();

    // pre-increment:  ++p  increments satisfaction, returns updated self
    Person& operator++();
    // post-increment: p++  increments satisfaction, returns copy before increment
    Person  operator++(int);

    // p += animal  →  assign pet (delegates to assignPet)
    Person& operator+=(Animal* animal);

    // !p  →  true if person has no pet
    bool operator!() const;

    bool operator==(const Person& other) const;
    bool operator<(const Person& other) const;

    friend std::istream& operator>>(std::istream& in, Person& obj);
    friend std::ostream& operator<<(std::ostream& out, const Person& obj);
};

int Person::noPersons = 0;

void Person::copyName(const char* src) {
    this->name = new char[strlen(src) + 1];
    strcpy(this->name, src);
}

Person::Person() : id(++noPersons) {
    copyName("N/A");
    this->satisfaction = 0;
    this->pet = nullptr;
}

Person::Person(char* name) : id(++noPersons) {
    copyName(name);
    this->satisfaction = 0;
    this->pet = nullptr;
}

Person::Person(char* name, int satisfaction) : id(++noPersons) {
    copyName(name);
    this->satisfaction = satisfaction;
    this->pet = nullptr;
}

Person::Person(char* name, int satisfaction, Animal* pet) : id(++noPersons) {
    copyName(name);
    this->satisfaction = satisfaction;
    this->pet = (pet != nullptr) ? new Animal(*pet) : nullptr;
}

Person::Person(const Person& obj) : id(++noPersons) {
    copyName(obj.name);
    this->satisfaction = obj.satisfaction;
    this->pet = (obj.pet != nullptr) ? new Animal(*obj.pet) : nullptr;
}

Person& Person::operator=(const Person& obj) {
    if (this == &obj)
        return *this;
    delete[] this->name;
    copyName(obj.name);
    this->satisfaction = obj.satisfaction;
    delete this->pet;
    this->pet = (obj.pet != nullptr) ? new Animal(*obj.pet) : nullptr;
    return *this;
}

Person::~Person() {
    delete[] name;
    delete pet;
}

void Person::assignPet(Animal* animal) {
    delete this->pet;
    this->pet = (animal != nullptr) ? new Animal(*animal) : nullptr;
    if (this->pet != nullptr)
        this->satisfaction += 10;
}

void Person::removePet() {
    delete this->pet;
    this->pet = nullptr;
}

Person& Person::operator++() {
    this->satisfaction++;
    return *this;
}

// the dummy int parameter is how C++ distinguishes post from pre
Person Person::operator++(int) {
    Person copy = *this;   // save state before increment
    this->satisfaction++;
    return copy;           // return the old state
}

Person& Person::operator+=(Animal* animal) {
    assignPet(animal);
    return *this;
}

bool Person::operator!() const {
    return this->pet == nullptr;
}

bool Person::operator==(const Person& other) const {
    return strcmp(this->name, other.name) == 0;
}

// sort by satisfaction ascending
bool Person::operator<(const Person& other) const {
    return this->satisfaction < other.satisfaction;
}

std::istream& operator>>(std::istream& in, Person& obj) {
    char buff[256];

    std::cout << "  Name: ";
    in.getline(buff, 256);
    delete[] obj.name;
    obj.name = new char[strlen(buff) + 1];
    strcpy(obj.name, buff);

    std::cout << "  Satisfaction grade: ";
    in >> obj.satisfaction;
    in.ignore();

    return in;
}

std::ostream& operator<<(std::ostream& out, const Person& obj) {
    out << "  [Person #" << obj.id << "] "
        << obj.name << " | satisfaction: " << obj.satisfaction;
    if (obj.pet != nullptr)
        out << "\n    Pet: " << *obj.pet;
    else
        out << "\n    Pet: none";
    return out;
}

// ─────────────────────────────────────────────────────────────────────────────

class Menu {
    std::vector<Animal*> animals;
    std::vector<Person*> persons;

    void printAnimals() const;
    void printPersons() const;
    int  pickAnimal() const;
    int  pickPerson() const;

public:
    Menu() = default;
    ~Menu();
    void run();
    void animalMenu();
    void peopleMenu();
    void assignMenu();
};

Menu::~Menu() {
    for (size_t i = 0; i < animals.size(); i++) delete animals[i];
    for (size_t i = 0; i < persons.size(); i++) delete persons[i];
}

void Menu::printAnimals() const {
    if (animals.empty()) {
        std::cout << "  No animals registered.\n";
        return;
    }
    for (size_t i = 0; i < animals.size(); i++)
        std::cout << "  " << i << ". " << *animals[i] << '\n';
}

void Menu::printPersons() const {
    if (persons.empty()) {
        std::cout << "  No persons registered.\n";
        return;
    }
    for (size_t i = 0; i < persons.size(); i++)
        std::cout << "  " << i << ". " << *persons[i] << '\n';
}

int Menu::pickAnimal() const {
    if (animals.empty()) {
        std::cout << "  No animals available.\n";
        return -1;
    }
    printAnimals();
    std::cout << "  Pick animal index (-1 to cancel): ";
    int idx;
    std::cin >> idx;
    std::cin.ignore();
    if (idx < 0 || idx >= (int)animals.size()) return -1;
    return idx;
}

int Menu::pickPerson() const {
    if (persons.empty()) {
        std::cout << "  No persons available.\n";
        return -1;
    }
    printPersons();
    std::cout << "  Pick person index (-1 to cancel): ";
    int idx;
    std::cin >> idx;
    std::cin.ignore();
    if (idx < 0 || idx >= (int)persons.size()) return -1;
    return idx;
}

void Menu::run() {
    while (true) {
        std::cout << "\n=== Main Menu ===\n";
        std::cout << "0 - Exit\n";
        std::cout << "1 - Animals\n";
        std::cout << "2 - People\n";
        std::cout << "3 - Assign animal to person\n";
        std::cout << "Option: ";

        int option;
        std::cin >> option;
        std::cin.ignore();

        switch (option) {
            case 0:
                std::cout << "Goodbye!\n";
                return;
            case 1:
                animalMenu();
                break;
            case 2:
                peopleMenu();
                break;
            case 3:
                assignMenu();
                break;
            default:
                std::cout << "Invalid option.\n";
        }
    }
}

void Menu::animalMenu() {
    while (true) {
        std::cout << "\n--- Animal Menu ---\n";
        std::cout << "0 - Back\n";
        std::cout << "1 - Create animal\n";
        std::cout << "2 - List animals\n";
        std::cout << "3 - Edit animal\n";
        std::cout << "4 - Remove animal\n";
        std::cout << "5 - Compare two animals (operator==)\n";
        std::cout << "6 - Sort animals by name (operator<)\n";
        std::cout << "Option: ";

        int option;
        std::cin >> option;
        std::cin.ignore();

        switch (option) {
            case 0:
                return;
            case 1: {
                Animal* a = new Animal();
                std::cin >> *a;
                animals.push_back(a);
                std::cout << "  Animal created.\n";
                break;
            }
            case 2:
                printAnimals();
                break;
            case 3: {
                int idx = pickAnimal();
                if (idx == -1) break;
                std::cin >> *animals[idx];
                std::cout << "  Animal updated.\n";
                break;
            }
            case 4: {
                int idx = pickAnimal();
                if (idx == -1) break;
                for (size_t i = 0; i < persons.size(); i++)
                    persons[i]->removePet();
                delete animals[idx];
                animals.erase(animals.begin() + idx);
                std::cout << "  Animal removed and unassigned from all persons.\n";
                break;
            }
            case 5: {
                std::cout << "  First animal:\n";
                int a = pickAnimal();
                if (a == -1) break;
                std::cout << "  Second animal:\n";
                int b = pickAnimal();
                if (b == -1) break;
                if (*animals[a] == *animals[b])
                    std::cout << "  Same breed and number of legs.\n";
                else
                    std::cout << "  Different animals.\n";
                break;
            }
            case 6: {
                // bubble sort using operator< to keep it explicit for students
                for (size_t i = 0; i < animals.size(); i++)
                    for (size_t j = 0; j + 1 < animals.size() - i; j++)
                        if (*animals[j + 1] < *animals[j])
                            std::swap(animals[j], animals[j + 1]);
                std::cout << "  Animals sorted by name.\n";
                printAnimals();
                break;
            }
            default:
                std::cout << "  Invalid option.\n";
        }
    }
}

void Menu::peopleMenu() {
    while (true) {
        std::cout << "\n--- People Menu ---\n";
        std::cout << "0 - Back\n";
        std::cout << "1 - Create person\n";
        std::cout << "2 - List people\n";
        std::cout << "3 - Edit person\n";
        std::cout << "4 - Remove person\n";
        std::cout << "5 - Boost satisfaction  ++p  (pre-increment)\n";
        std::cout << "6 - Boost satisfaction  p++  (post-increment)\n";
        std::cout << "7 - Check if person has no pet  (!p)\n";
        std::cout << "8 - Compare two persons (operator==)\n";
        std::cout << "9 - Sort persons by satisfaction (operator<)\n";
        std::cout << "Option: ";

        int option;
        std::cin >> option;
        std::cin.ignore();

        switch (option) {
            case 0:
                return;
            case 1: {
                Person* p = new Person();
                std::cin >> *p;
                persons.push_back(p);
                std::cout << "  Person created.\n";
                break;
            }
            case 2:
                printPersons();
                break;
            case 3: {
                int idx = pickPerson();
                if (idx == -1) break;
                std::cin >> *persons[idx];
                std::cout << "  Person updated.\n";
                break;
            }
            case 4: {
                int idx = pickPerson();
                if (idx == -1) break;
                delete persons[idx];
                persons.erase(persons.begin() + idx);
                std::cout << "  Person removed.\n";
                break;
            }
            case 5: {
                // pre: ++p returns reference to already-incremented object
                int idx = pickPerson();
                if (idx == -1) break;
                Person& updated = ++(*persons[idx]);
                std::cout << "  After ++p:\n" << updated << '\n';
                break;
            }
            case 6: {
                // post: p++ returns copy of object before increment
                int idx = pickPerson();
                if (idx == -1) break;
                Person before = (*persons[idx])++;
                std::cout << "  Copy returned by p++ (old value):\n" << before << '\n';
                std::cout << "  Person now:\n" << *persons[idx] << '\n';
                break;
            }
            case 7: {
                int idx = pickPerson();
                if (idx == -1) break;
                if (!(*persons[idx]))
                    std::cout << "  Person has no pet.\n";
                else
                    std::cout << "  Person has a pet.\n";
                break;
            }
            case 8: {
                std::cout << "  First person:\n";
                int a = pickPerson();
                if (a == -1) break;
                std::cout << "  Second person:\n";
                int b = pickPerson();
                if (b == -1) break;
                if (*persons[a] == *persons[b])
                    std::cout << "  Same name.\n";
                else
                    std::cout << "  Different persons.\n";
                break;
            }
            case 9: {
                for (size_t i = 0; i < persons.size(); i++)
                    for (size_t j = 0; j + 1 < persons.size() - i; j++)
                        if (*persons[j + 1] < *persons[j])
                            std::swap(persons[j], persons[j + 1]);
                std::cout << "  Persons sorted by satisfaction.\n";
                printPersons();
                break;
            }
            default:
                std::cout << "  Invalid option.\n";
        }
    }
}

void Menu::assignMenu() {
    while (true) {
        std::cout << "\n--- Assign Menu ---\n";
        std::cout << "0 - Back\n";
        std::cout << "1 - Assign animal to person  (assignPet)\n";
        std::cout << "2 - Assign animal to person  (operator+=)\n";
        std::cout << "3 - Remove animal from person\n";
        std::cout << "Option: ";

        int option;
        std::cin >> option;
        std::cin.ignore();

        switch (option) {
            case 0:
                return;
            case 1: {
                std::cout << "\nSelect person:\n";
                int pidx = pickPerson();
                if (pidx == -1) break;
                std::cout << "\nSelect animal:\n";
                int aidx = pickAnimal();
                if (aidx == -1) break;
                persons[pidx]->assignPet(animals[aidx]);
                std::cout << "  Assigned via assignPet(). Satisfaction +10.\n";
                break;
            }
            case 2: {
                std::cout << "\nSelect person:\n";
                int pidx = pickPerson();
                if (pidx == -1) break;
                std::cout << "\nSelect animal:\n";
                int aidx = pickAnimal();
                if (aidx == -1) break;
                *persons[pidx] += animals[aidx];
                std::cout << "  Assigned via operator+=. Satisfaction +10.\n";
                break;
            }
            case 3: {
                std::cout << "\nSelect person:\n";
                int pidx = pickPerson();
                if (pidx == -1) break;
                persons[pidx]->removePet();
                std::cout << "  Pet removed.\n";
                break;
            }
            default:
                std::cout << "  Invalid option.\n";
        }
    }
}

int main() {
    Menu menu;
    menu.run();
}