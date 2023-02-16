#ifndef USER_H
#define USER_H

#include <string>

class User {
public:
    User();
    User(const std::string& name, const std::string& pwd, int num = 0);

    void set_username(const std::string& name);
    std::string get_username();
    void set_password(const std::string& pwd);
    std::string get_password();
    void set_number(int num);
    int get_number();

private:
    std::string username;
    std::string password;
    int number;
};

#endif
