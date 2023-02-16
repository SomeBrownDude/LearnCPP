#include <iostream>
#include "person.h"
#include <string>
using namespace std;

class Animal {
public: 
    void speak() {cout << "Grrrr" << endl;}
};

class Cat:public Animal {
public: 
    void jump() {cout << "Cat Jumping!" << endl;}
};

class Tiger: public Cat {
public: 
    void attackAntelope() {cout << "Attacking!" << endl;}
};

class Frog {
public:
    Frog(string name): name(name) {}
    string getName() {return name;}
    void info() {cout << "My name is: " << getName() << endl;}
private:
    string name;
};

class Machine{
private: 
    int id;
public: 
    Machine(): id(0) {cout << "Machine created" << endl;}
    Machine(int id): id(id) {cout << "Machine created" << endl;}
    void info() {cout << "ID: " << id << endl;} 
};

class Vehicle: public Machine {
public: 
    Vehicle() {cout << "Vehicle created" << endl;}
    Vehicle(int id): Machine(id) {cout << "Vehicle created" << endl;}
};

class Car: public Vehicle {
public: 
    Car(): Vehicle(999) {cout << "Car created" << endl;}
};

int main() {
    Car v;
    v.info();
}