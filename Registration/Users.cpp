#include <termios.h>
#include <unistd.h>
#include "Color.h"
#include "User.h"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ostream>  
#include <sstream>
#include <string>
#include <vector>
using namespace std;

const string ESCAPE_CHAR        = "e";
const int MIN_USERNAME_LENGTH   = 4;
const int MAX_USERNAME_LENGTH   = 16;
const int MIN_PASSWORD_LENGTH   = 6;
const int MAX_PASSWORD_LENGTH   = 16;
const int MAX_PASSWORD_ATTEMPTS = 5;

void SetStdinEcho(bool enable = true) {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag &= ~ECHO;
    
    !enable ? tty.c_lflag &= ~ECHO : tty.c_lflag |= ECHO;

    (void)tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

string get(string out_message) {
    string input;
    cout << out_message;
    cin >> input;
    return input;
}

void delete_from_file(string filename, string line_to_del) {
    fstream file(filename);
    string curr_line;
    stringstream file_ss;

    while (getline(file, curr_line))
        if (curr_line != line_to_del) file_ss << curr_line << endl;
    file.close();

    file.open(filename, std::ofstream::out | std::ofstream::trunc);
    file << file_ss.rdbuf();
    file.close();
}

void replace_line_in_file(string filename, string line, string new_line) {
    fstream file(filename, ios::in | ios::out);

    if (!file.is_open()) {
        cout << "Error opening file" << endl;
        return;
    }

    string current_line;
    streampos pos;
    while (getline(file, current_line)) {
        pos = file.tellg();
        if (current_line == line) {
            file.seekp(static_cast<streampos>(static_cast<int>(pos) - current_line.length() - 1));
            file << new_line << endl;
            break;
        }
    }

    file.close();
}

class Userbase {
public:
    Userbase(const std::string& filename) : file_name(filename) {}

    void add(User u) {
        std::ofstream out_file(file_name, std::ios::app);
        out_file << u.get_username() << "," << u.get_password() << "," << u.get_number() << std::endl;
        out_file.close();
    }

    void delete_user(User u) {
        std::vector<User> users;
        std::ifstream in_file(file_name);
        std::string line;

        while (getline(in_file, line)) {
            string username;
            username = line.substr(0, line.find(','));
            if (u.get_username() == username) delete_from_file(file_name, line);
        }
        in_file.close();
    }

    bool check_login(const std::string& username, const std::string& password) {
        ifstream in_file(file_name);
        string line;

        while (getline(in_file, line)) {
            string uname, pwd;

            auto pos = line.find(',');
            uname    = line.substr(0, pos);
            pwd      = line.substr(pos + 1, line.find_last_of(',') - (pos + 1));

            if (username == uname && password == pwd) {
                in_file.close();
                return true;
            }
        }
        in_file.close();
        return false;
    }

    void open(User& u) {  // this function will open user by making 'u' equivalent to existing user that matches 'u'
        fstream file(file_name);
        string line;

        while (getline(file, line)) {
            if (u.get_username() == line.substr(0, line.find(','))) {
                u.set_number(stoi(line.substr(line.find_last_of(',') + 1)));
                break;
            }
        }
        file.close();
    }

    void update_user_data(User u) {
        fstream file(file_name);
        string line;

        while (getline(file, line)) {
            if (u.get_username() == line.substr(0, line.find(','))) {
                string new_line = line;
                new_line.replace(line.find_last_of(',') + 1, line.length() - 1, to_string(u.get_number()));

                replace_line_in_file(file_name, line, new_line);
                break;
            }
        }

        file.close();
    }

    bool is_new_username(string username) {
        ifstream in_file(file_name);
        string line;

        while (getline(in_file, line)) {
            string uname = line.substr(0, line.find(','));
            if (username == uname) {
                in_file.close();
                return false;
            }
        }
        in_file.close();
        return true;
    }

    string find_password(string username) {
        fstream file(file_name);
        string line;

        while (getline(file, line)) {
            if (username == line.substr(0, line.find(','))) {
                file.close();
                return line.substr(line.find(',') + 1, line.find_last_of(',') - (line.find(',') + 1));
            }
        }
        file.close();
        return "Find-Password Error";
    }

private:
    string file_name;
};

class Start_window {
public:
    void main_loop() {
        string input;

        while (input != ESCAPE_CHAR) {
            cout << "Incr/Decr Program" << endl << "--------------------" << endl;
            cout << endl
                 << "Press 'l' to log in to existing account or press 'r' to register new account, "
                    "'f' if you forgot password, or 'e'/Ctrl + C to exit program"
                 << endl
                 << "for more commands, press a\n\n";

            input = get(">");

            if (input == "r") register_new_acc();

            if (input == "a") cout << "\n\nDelete account: 'd'\n\n";

            if (input == "l") {
                User u = login();
                open_user(u);
            }
            if (input == "f") forgot_password();

            if (input == "d") delete_user();
        }
    }

private:
    void register_new_acc() {
        Userbase ubase("account-logins.txt");
        string username;
        string password;

        username = get_new_username();
        password = get_new_password();

        User new_user(username, password);
        ubase.add(new_user);

        cout << "Your new account is registered!\n\n";
    }

    void delete_user() {
        User user_to_del;
        Userbase ubase("account-logins.txt");

        cout << "Login to existing account to delete it: " << endl;
        user_to_del = login();

        string delete_msg = "User " + user_to_del.get_username() + " successfully deleted!";
        ubase.delete_user(user_to_del);
        Color del_msg(delete_msg);
        del_msg.print_red();
    }

    string get_new_username() {
        Userbase ubase("account-logins.txt");
        string username;

        cout << "Enter new username: ";
        cin >> username;
        cout << endl;

        while (username.length() < MIN_USERNAME_LENGTH || username.length() > MAX_USERNAME_LENGTH || !ubase.is_new_username(username)) {
            if (username.length() < MIN_USERNAME_LENGTH || username.length() > MAX_USERNAME_LENGTH) {
                cout << "Username must have between " << MIN_USERNAME_LENGTH << " to " << MAX_USERNAME_LENGTH << " characters" << endl;
                cout << "Please enter username again: ";
                cin >> username;
                cout << endl;
            }
            if (!ubase.is_new_username(username)) {
                cout << "Username already exists!" << endl;
                cout << "Please choose a different username: ";
                cin >> username;
                cout << endl;
            }
        }

        return username;
    }

    string get_new_password() {
        string password;

        SetStdinEcho(false);
        cout << "Enter new password: ";
        cin >> password;
        cout << endl;

        while (password.length() < MIN_PASSWORD_LENGTH || password.length() > MAX_PASSWORD_LENGTH) {
            cout << "Password must have between " << MIN_PASSWORD_LENGTH << " to " << MAX_PASSWORD_LENGTH << " characters" << endl;
            cout << "Please enter username again: ";
            cin >> password;
            cout << endl << endl;
        }
        SetStdinEcho(true);
        return password;
    }

    User login() {
        string username = get("Enter username: ");
        string password = get("Enter password: ");
        Userbase ubase("account-logins.txt");
        int password_attempts = 0;

        while (!ubase.check_login(username, password)) {
            cout << "Username or password incorrect, please try again." << endl;
            username = get("Enter username: ");
            password = get("Enter password: ");

            password_attempts++;
            if (password_attempts == MAX_PASSWORD_ATTEMPTS) {
                cout << "Too many attemps, try again later!\n\n";
                exit(0);
            }
        }
        User user(username, password);
        cout << "Login successful!\n\n";
        return user;
    }

    void open_user(User active_user) {
        Userbase ubase("account-logins.txt");
        string input = "i";

        ubase.open(active_user);
        // now active_user has access to user's data (before only had access to username and
        // password)

        cout << "Enter 'i' to increment, 'd' to decrement, 'r' to reset, 's' to set any int, any "
                "other key to exit\n\n";
        cout << active_user.get_number() << endl << endl;

        // loop asks for input, saves number to file and prints number
        while (input.find_first_not_of("idsr") == string::npos) {
            input = get(">");

            int new_number = active_user.get_number();

            if (input == "i") new_number++;

            if (input == "d") new_number--;

            if (input == "s") new_number = stoi(get("Enter int to set: "));

            if (input == "r") new_number = 0;

            active_user.set_number(new_number);
            ubase.update_user_data(active_user);

            cout << endl << endl << active_user.get_number() << endl << endl;
        }

        cout << "Saving data and exiting your account...\n\n";
    }

    void forgot_password() {
        Userbase ubase("account-logins.txt");

        string username = get("Forgot password? No worries, please enter username: ");
        cout << endl;

        while (ubase.is_new_username(username)) {
            username = get("Username does not exist, please enter valid username (or 'b' to go back): ");
            cout << endl;
            if (username == "b") return;
        }
        Color pwd(ubase.find_password(username));
        cout << "Your password is: ";
        pwd.print_green();
        cout << endl;
    }
};

int main() {
    Start_window win;
    win.main_loop();
}
