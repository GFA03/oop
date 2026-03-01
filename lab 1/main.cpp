#include <iostream>

using namespace std;

void valueByCopy(int x) {
    cout << "Address of x in valueByCopy: " << &x << endl; // This will print the address of the local variable x in this function
    x = 10; // This will not affect the original variable
}

void valueByReference(int &x) {
    cout << "Address of x in valueByReference: " << &x << endl; // This will print the address of the original variable passed by reference
    x = 6; // This will change the original variable
}

void pointerExample(int *x) {
    cout << "Address stored in pointer x: " << x << endl; // This will print the address stored in the pointer
    cout << "Value pointed to by x: " << *x << endl; // This will print the value at the address stored in the pointer
    *x = 7; // This will change the value at the address stored in the pointer
}

int& returnByReference() {
    int* p = new int(42); // Dynamically allocate an integer and initialize it to 42
    cout << "Address of dynamically allocated integer in returnByReference(): " << p << endl; // Print the address of the dynamically allocated integer
    return *p; // Return a reference to the dynamically allocated integer
}

int main() {
    int a = 1, *b = &a;
    cout << "Value of a: " << a << endl;
    cout << "Address of a: " << &a << endl;
    cout << "Value of b (address of a): " << b << endl;
    cout << "Value pointed to by b (value of a): " << *b << endl << endl;

    valueByCopy(a);
    cout << "Value of a after valueByCopy: " << a << endl << endl; // a will still be 1

    valueByReference(a);
    cout << "Value of a after valueByReference: " << a << endl << endl; // a will be changed to 6

    pointerExample(b);
    cout << "Value of a after pointerExample: " << a << endl << endl; // a will be changed to 7

    b = new int(5); // Dynamically allocate an integer and assign its address to b
    cout << "Value of b (address of new integer): " << b << endl;
    cout << "Value pointed to by b (value of new integer): " << *b << endl << endl;
    delete b; // Free the dynamically allocated memory

    b = &a; // Point b back to a
    cout << "Value of b (address of a): " << b << endl;
    cout << "Value pointed to by b (value of a): " << *b << endl << endl;

    b = new int[5]; // Dynamically allocate an array of 5 integers
    for (int i = 0; i < 5; i++) {
        b[i] = i + 1; // Initialize the array with values 1 to 5
    }
    cout << "Values in the dynamically allocated array: ";
    for (int i = 0; i < 5; i++) {
        cout << b[i] << " "; // Print the values in the array
    }
    cout << endl << endl;
    delete[] b; // Free the dynamically allocated array

    int& ref = returnByReference(); // Get a reference to the dynamically allocated integer
    ref = 100; // This will change the value of the dynamically allocated integer to 100
    cout << "Value of ref: " << ref << endl; // This will print the value of ref, which is now 100
    cout << "Value of ref address: " << &ref << endl; 
    delete &ref; // Free the dynamically allocated integer

    cout << "Attempting to access freed memory (undefined behavior): " << ref << endl; // This will cause undefined behavior since ref points to memory that has been freed
    return 0;
}