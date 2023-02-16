#include "User.h"

User::User() {}

User::User(const std::string& name, const std::string& pwd, int num) : username(name), password(pwd), number(num) {}

void User::set_username(const std::string& name) {
    username = name;
}

std::string User::get_username() {
    return username;
}

void User::set_password(const std::string& pwd) {
    password = pwd;
}

std::string User::get_password() {
    return password;
}

void User::set_number(int num) {
    number = num;
}

int User::get_number() {
    return number;
}
