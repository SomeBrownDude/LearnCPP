#ifndef COLOR_H
#define COLOR_H

#include <string>

class Color {
public:
    Color(std::string msg);

    void print_red();
    void print_green();

private:
    void print(int code);

    std::string msg;
    const int b_red_code   = 41;
    const int b_green_code = 42;
};

#endif