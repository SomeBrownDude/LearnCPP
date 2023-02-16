#include "Color.h"
#include <iostream>

using namespace std;

Color::Color(string msg) : msg(msg) {}

void Color::print_red() {
    print(b_red_code);
}

void Color::print_green() {
    print(b_green_code);
}

void Color::print(int code) {
    string color_string = "\033[";
    cout << color_string << code << "m" << msg << color_string << "0m" << endl;
}