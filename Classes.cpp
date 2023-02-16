#include <iostream>
#include <string>
#include <stdio.h>
using namespace std;


class Employee {
private: 
    string Name;
    string Company;
    int Age;

public: 
    Employee(string name, string company, int age) {
        Name = name;
        Company = company;
        Age = age;
    }

    Employee() {}

public: 

    void setName(string name) { Name = name;}
    string getName() {return Name;}   

    void setCompany(string company) { Company = company;}
    string getCompany() {return Company;}   

    void setAge(int age) { Age = age;}
    int getAge() {return Age;}



    void introduceYourself() {
        std::cout << "Name - " << Name << endl;
        std::cout << "Company - " << Company << endl;
        std::cout << "Age - " << Age << endl;

    }


};

int main() {
    Employee e1("Jonathan", "Jonathan's Company", 19);

    printf("Hello, my name is %s, from %s, and I am %d old", e1.getName().c_str(), e1.getCompany().c_str(), e1.getAge());

}