#include "person.h"
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

Person::Person() {
    name = "undefined";
    age = 0;
}

Person::Person(string newName) {
    name = newName;
    age = 0;
}

Person::Person(string name, int age): name(name), age(age) {}

string Person::toString() {
    stringstream ss;

    ss << "Name: ";
    ss << name;
    ss << "; age: ";
    ss << age;

    return ss.str();
}

void Person::setName(string newName) {
    name = newName;
}

string Person::getName() {
    return name;
}
