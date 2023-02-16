#include <termios.h>
#include <unistd.h>

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

// program creates new acc or logs in to existing acc and allows user to store a number by
// incrementing or decrementing it. program will store acc's unique stored number in text file and
// when user logs in again, number will be displayed

const char ESCAPE_CHAR = 'e';
const int MIN_USERNAME_LENGTH = 4;
const int MAX_USERNAME_LENGTH = 16;
const int MIN_PASSWORD_LENGTH = 6;
const int MAX_PASSWORD_LENGTH = 16;
const int MAX_PASSWORD_ATTEMPTS = 5;

void SetStdinEcho(bool enable = true) {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if (!enable)
        tty.c_lflag &= ~ECHO;
    else
        tty.c_lflag |= ECHO;

    (void)tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

bool is_new_username(string username) {
    fstream login_file("usernames.txt");
    string existing_username;

    vector<string> existing_usernames;

    while (getline(login_file, existing_username)) {
        existing_usernames.push_back(existing_username);
    }
    login_file.close();

    bool res = binary_search(existing_usernames.begin(), existing_usernames.end(), username);
    return !res;
}

string get_new_username() {
    string username;

    username = get("Enter username: ");

    while (username.length() < MIN_USERNAME_LENGTH || username.length() > MAX_USERNAME_LENGTH ||
           !is_new_username(username)) {
        if (username.length() < MIN_USERNAME_LENGTH || username.length() > MAX_USERNAME_LENGTH) {
            cout << "Username must have between " << MIN_USERNAME_LENGTH << " to "
                 << MAX_USERNAME_LENGTH << " characters" << endl;
            username = get("Please enter username again: " );
        }
        if (!is_new_username(username)) {
            cout << "Username already exists!" << endl;
            cout << "Please choose a different username: ";
            cin >> username;
            cout << endl;
        }
    }
    return username;
}

void confirm_new_password(string password) {
    SetStdinEcho(false);
    string entry;

    cout << "Confirm new password: ";
    cin >> entry;
    cout << endl;

    while (entry != password) {
        cout << "Sorry passwords do not match" << endl;
        cout << "Please try again: ";
        cin >> entry;
        cout << endl << endl;
    }

    SetStdinEcho(true);
}

string get(string out_message) {
    string input;
    cout << out_message;
    cin >> input;
    return input;
}

string get_new_password() {
    string password;

    SetStdinEcho(false);
    cout << "Enter new password: ";
    cin >> password;
    cout << endl;

    while (password.length() < MIN_PASSWORD_LENGTH || password.length() > MAX_PASSWORD_LENGTH) {
        cout << "Password must have between " << MIN_PASSWORD_LENGTH << " to "
             << MAX_PASSWORD_LENGTH << " characters" << endl;
        cout << "Please enter username again: ";
        cin >> password;
        cout << endl << endl;
    }
    SetStdinEcho(true);
    return password;
}

bool check_login(string user_inp, string pass_inp) {
    fstream login_file("account-logins.txt");
    string line;
    vector<string> usernames;
    vector<string> passwords;

    while (getline(login_file, line)) {
        int delimiter_pos = line.find_first_of(',');
        usernames.push_back(line.substr(0, delimiter_pos));
        passwords.push_back(line.substr(delimiter_pos + 1));
    }

    if (find(usernames.begin(), usernames.end(), user_inp) != usernames.end()) {
        if (passwords.at(find(usernames.begin(), usernames.end(), user_inp) - usernames.begin()) ==
            pass_inp)
            return true;
    }
    return false;
}

string get_password(string username) {
    fstream login_file("account-logins.txt");
    string line;
    while (getline(login_file, line)) {
        string curr_username = line.substr(0, line.find_first_of(','));
        if (curr_username == username) return line.substr(line.find_first_of(',') + 1);
    }
    return "Fail";
}

void find_password(string username) {
    fstream login_file("account-logins.txt");
    string line;
    vector<string> usernames;
    vector<string> passwords;

    while (getline(login_file, line)) {
        int delimiter_pos = line.find_first_of(',');
        usernames.push_back(line.substr(0, delimiter_pos));
        passwords.push_back(line.substr(delimiter_pos + 1));
    }

    while (find(usernames.begin(), usernames.end(), username) == usernames.end()) {
        username = get("Username does not exist! Please Try again.");
    }
    cout << "Your password is: ";
    cout << passwords.at(find(usernames.begin(), usernames.end(), username) - usernames.begin())
         << "\n\n";
}

void create_user_file(string filename) {
    ofstream file(filename);
    cout << "Creating file..." << endl;
    file << 0;
    file.close();
}

void increment(string filename) {
    fstream file(filename);
    int number;
    file >> number;
    string input = "i";

    cout << "Enter 'i' to increment, 'd' to decrement, 'r' to reset, 's' to set any int, any other "
            "key to exit\n\n";
    cout << number << endl << endl;

    // loop asks for input, saves number to file and prints number
    while (input.find_first_not_of("idsr") == string::npos) {
        cout << ">";
        cin >> input;
        cout << endl;

        file.open(filename, std::ofstream::out | std::ofstream::trunc);

        if (input == "i") number++;

        if (input == "d") number--;

        if (input == "s") number = stoi(get("Enter int: "));

        if (input == "r") number = 0;

        file << number;
        cout << number << endl << endl;
        file.close();
    }
}

void save_account_login(string username, string password) {
    fstream login_file;
    fstream usernames_file;

    login_file.open("account-logins.txt", ios_base::app);
    usernames_file.open("usernames.txt", ios_base::app);

    login_file << username << "," << password << endl;
    usernames_file << username << endl;
}

bool file_exists(string filename) {
    ifstream file(filename);
    return file.good();
}

void delete_from_file(string filename, string line_to_del) {
    fstream file(filename);

    string curr_line;
    stringstream file_ss;

    while (getline(file, curr_line)) {
        if (curr_line != line_to_del) file_ss << curr_line << endl;
    }
    file.close();

    file.open(filename, std::ofstream::out | std::ofstream::trunc);
    file << file_ss.rdbuf();
    file.close();
}

void delete_acc(string username) {
    string username_file = "usernames.txt";
    string account_file = "account-logins.txt";

    string login_to_del = username + "," + get_password(username);

    delete_from_file(username_file, username);
    delete_from_file(account_file, login_to_del);

    string filename = username + ".txt";
    remove(filename.c_str());
}

string login() {
    string username = get("Enter username: ");
    string password = get("Enter password: ");

    int password_attempts = 0;

    while (!check_login(username, password)) {
        cout << "Username or password incorrect, please try again." << endl;
        username = get("Enter username: ");
        password = get("Enter password: ");
        password_attempts++;
        if (password_attempts == MAX_PASSWORD_ATTEMPTS) {
            cout << "Too many attemps, try again later!" << endl;
            exit(0);
        }
    }
    return username;
}

int main() {
    char input;
    while (input != ESCAPE_CHAR) {
        cout << "Incr/Decr Program" << endl << "--------------------" << endl;
        cout << endl
             << "Press 'l' to log in to existing account or press 'r' to register new account, 'f' "
                "if you forgot password, or 'e'/Ctrl + C to exit program"
             << endl;
        cout << "for more commands, press a" << endl;

        input = get(">")[0];

        if (input == 'r') {  // registering new account
            string username;
            string password;

            // getting username and password
            username = get_new_username();
            password = get_new_password();
            confirm_new_password(password);

            save_account_login(username, password);  // saving login to file

            cout << endl << "Thank you for creating new account!" << endl;
        }

        if (input == 'a') {
            cout << "Delete account: 'd'";
        }

        if (input == 'l') {  // Logging in to account
            string username = login();
            cout << "Account login successful!\n\n";
            string user_file = username + ".txt";
            if (!file_exists(user_file)) create_user_file(user_file);

            increment(user_file);
        }

        if (input == 'f') {
            string username = get("Forgot password? No worries, please enter username: ");
            find_password(username);
        }

        if (input == 'd') {
            string username = login();
            delete_acc(username);
            cout << "\n\nAccount deleted successfully!\n\n";
        }
    }

    cout << "Thank you for using Incr/Decr Program!";
    exit(0);
}
