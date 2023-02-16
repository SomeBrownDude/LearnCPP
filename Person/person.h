#ifndef PERSON_H_
#define PERSON_H_

#include <iostream>
#include <string>
using namespace std;

class Person {
public:
    Person();
    Person(string newName);
    Person(string name, int age);
    string toString();
    void setName(string newName);
    string getName();
private: 
    string name;
    int age;
};



#endif