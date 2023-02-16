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



/*

Classes: 

Start: class that handles all ouput and gets user input.
    functions: 
        main loop = loop for program 
        get inputs
        login() = login function 
        delete() = delete user function
        forgot_password() = forgot password function
        user_window() = gets input from user to modify user data
        etc. 
User: class that stores single user data eg. username, password, number. 
    functions: 
        getters and setters for username, password, and number

User_base: class that stores all users and user data (files). Can add new users, delete users, verify logins etc. 
    functions: 
        .add(User new user) = adds new user to database
        .delete(User del_user) = deletes user from database and all user data
        .check_login(User u) = checks if username and password matches with existing use. 
        .increment(User u, char inp) = based on input, will modify user number data
*/

const string ESCAPE_CHAR = "e";
const int MIN_USERNAME_LENGTH = 4;
const int MAX_USERNAME_LENGTH = 16;
const int MIN_PASSWORD_LENGTH = 6;
const int MAX_PASSWORD_LENGTH = 16;
const int MAX_PASSWORD_ATTEMPTS = 5;

template <typename T> 
ostream& operator<<(ostream& os, const vector<T>& v) 
{ 
    os << "[";
    for (int i = 0; i < v.size(); ++i) { 
        os << v[i]; 
        if (i != v.size() - 1) 
            os << ", "; 
    }
    os << "]\n";
    return os; 
}

void SetStdinEcho(bool enable = true) {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if (!enable)
        tty.c_lflag &= ~ECHO;
    else
        tty.c_lflag |= ECHO;

    (void)tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

string get(string out_message) {
    string input;
    cout << out_message;
    cin >> input;
    return input;
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

void confirm_new_password(string password) {
    SetStdinEcho(false);
    string entry = get("Confirm new password: ");

    while (entry != password) {
        cout << endl << "Sorry passwords do not match" << endl;
        cout << "Please try again: ";
        cin >> entry;
        cout << endl << endl;
    }
    SetStdinEcho(true);
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

    while(getline(file, curr_line)) {
        if(curr_line != line_to_del) file_ss << curr_line << endl;   
    }
    file.close();

    file.open(filename, std::ofstream::out | std::ofstream::trunc);
    file << file_ss.rdbuf();    
    file.close();
}


class User {  // stores user data (number, login info) // creates new user
public:
    User(){}
    User(string username, string password) : m_username(username), m_password(password) {}

    string get_username() { return m_username; }
    string get_password() { return m_password; }
    void set_username(string username) {m_username = username;}
    void set_password(string password) {m_password = password;}
    int get_num() { return number; }
    void set_num(int num) { number = num;}

    void save_user() {
        create_user_file();
        fstream login_file;
        fstream usernames_file;

        login_file.open(this->login_file, ios_base::app);
        usernames_file.open(this->usernames_file, ios_base::app);

        login_file << m_username << "," << m_password << endl;
        usernames_file << m_username << endl;
    }

    void increment() {
        fstream file(user_file);
        int number;
        file >> number;
        string input = "i";

        cout << "Enter 'i' to increment, 'd' to decrement, 'r' to reset, 's' to set any int, any other key to exit\n\n";
        cout << number << endl << endl;

        //loop asks for input, saves number to file and prints number
        while(input.find_first_not_of("idsr") == string::npos) { 
                cout << ">";
                cin >> input; 
                cout << endl;   
            
                file.open(user_file, std::ofstream::out | std::ofstream::trunc);

                if(input == "i") number++;
            
                if(input == "d") number--;

                if(input == "s") number = stoi(get("Enter int: "));

                if(input == "r") number = 0;

                file << number;
                cout << number << endl << endl;
                file.close();
        }
    }

    bool check_login(string user_inp, string pass_inp) {
        fstream login_file(this->login_file);
        string line;
        vector<string> usernames;
        vector<string> passwords;

        while(getline(login_file, line)) {
            int delimiter_pos = line.find_first_of(',');
            usernames.push_back(line.substr(0, delimiter_pos));
            passwords.push_back(line.substr(delimiter_pos + 1));
        } 

        if(find(usernames.begin(), usernames.end(), user_inp) != usernames.end()) {
            if(passwords.at(find(usernames.begin(), usernames.end(), user_inp) - usernames.begin()) == pass_inp) 
                return true;
        }

        return false;
    }

private:
    void create_user_file() {
        if (!file_exists(user_file)) {
            ofstream file(user_file);
            file << 0;
            file.close();
        }
    }

private:
    string m_username;
    string m_password;
    //
    string user_file = m_username + ".txt";
    string login_file = "account-logins.txt";
    string usernames_file = "usernames.txt";
    //

    int number = 0;
};



class User_base {
public:
    User get_curr_user() {
        return curr_user;
    }

    void add_user(User new_user) { //adds user to database
        create_user_file(new_user);
        fstream login_file;
        fstream usernames_file;

        login_file.open(login_file_name, ios_base::app);
        usernames_file.open(usernames_file_name, ios_base::app);

        login_file << new_user.get_username() << "," << new_user.get_password() << endl;
        usernames_file << new_user.get_username() << endl;
    }

    bool check_login(User u) {
        fstream login_file(login_file_name);
        string line;
        vector<string> usernames;
        vector<string> passwords;

        while(getline(login_file, line)) {
            int delimiter_pos = line.find_first_of(',');
            usernames.push_back(line.substr(0, delimiter_pos));
            passwords.push_back(line.substr(delimiter_pos + 1));
        } 

        if(find(usernames.begin(), usernames.end(), u.get_username()) != usernames.end()) {
            if(passwords.at(find(usernames.begin(), usernames.end(), u.get_username()) - usernames.begin()) == u.get_password())
                return true;
        }

        return false;       
    }

    void login(User active_user) {
        for(User i: users)
            if(i.get_username() == active_user.get_username()) curr_user = i;
    } 

    void logout() {
        User void_user;
        curr_user = void_user;
    }

    void save_data() {
        fstream file;
        file.open(curr_user.get_username() + ".txt", std::ofstream::out | std::ofstream::trunc);

        file << curr_user.get_num();    
        cout << "save data num: " << curr_user.get_num() << endl << endl;
        file.close();
    }

    void delete_user(User u) { //deletes user from database
        string login_to_del = u.get_username() + "," + u.get_password();
        
        delete_from_file(usernames_file_name, u.get_username());
        delete_from_file(login_file_name, login_to_del);
        
        string user_file = u.get_username() + ".txt";
        remove(user_file.c_str());
    }

    void increment(User u, string input,  int num = 0) {
        if(input == "i") u.set_num(u.get_num() + 1);
    
        if(input == "d") u.set_num(u.get_num() - 1);

        if(input == "s") u.set_num(num);

        if(input == "r") u.set_num(0);

        save_data();
    }

    void read_users() {
        fstream login_file(login_file_name);
        string line;
        vector<string> usernames;
        vector<string> passwords;

        while(getline(login_file, line)) {
            int delimiter_pos = line.find_first_of(',');
            usernames.push_back(line.substr(0, delimiter_pos));
            passwords.push_back(line.substr(delimiter_pos + 1));
        }

        for(int i = 0; i < usernames.size(); i++) {
            User u(usernames.at(i), passwords.at(i));
            fstream u_file(u.get_username() + ".txt");
            int num = 0;
            u_file >> num;

            u.set_num(num);

            users.push_back(u);
        }

        for(User i: users) {
            cout << i.get_username() << ",   " << i.get_num() << endl;
        }
        

    }



private: 
    void create_user_file(User u) {
        if (!file_exists(u.get_username() + ".txt")) {
            ofstream file(u.get_username() + ".txt");
            file << 0;
            file.close();
        }
    }

    string login_file_name = "account-logins.txt";
    string usernames_file_name = "usernames.txt";
    User curr_user;
    string curr_user_file_name = curr_user.get_username() + ".txt";

    vector<User> users;

};

class Start {  // login, new account, get user input, delete account
public:
    void main_loop() {
        string input;
        while (input != ESCAPE_CHAR) {
            cout << "Incr/Decr Program" << endl << "--------------------" << endl;
            cout << endl
                 << "Press 'l' to log in to existing account or press 'r' to register new account, "
                    "'f' if you forgot password, or 'e'/Ctrl + C to exit program"
                 << endl;
            cout << "for more commands, press a" << endl;

            input = get_input();

            if (input == "r") {
                create_acc();
            }

            if (input == "a") cout << "\n\nDelete account: 'd'\n\n";

            if (input == "l") {
                login();
                user_page(u_database.get_curr_user());
            }
            if (input == "f") {
            }
            if (input == "d") {
                
            }
        }
    }

private:
    string get_input() { return get(">"); }


    void login() {
        string username = get("Enter username: ");
        string password = get("Enter password: ");

        int password_attempts = 0;
        User user(username, password);

        while(!u_database.check_login(user)) {
            cout << "Username or password incorrect, please try again." << endl;
            username = get("Enter username: ");
            password = get("Enter password: ");

            password_attempts++;
            if(password_attempts == MAX_PASSWORD_ATTEMPTS) {
                cout << "Too many attemps, try again later!\n\n";
                exit(0);
            }
        }
        cout << "Login successful!\n\n";
        u_database.login(user);
    }

    void user_page(User active_user) {
        string input = "i";

        cout << "Enter 'i' to increment, 'd' to decrement, 'r' to reset, 's' to set any int, any other key to exit\n\n";
        cout << active_user.get_num() << endl << endl;

        //loop asks for input, saves number to file and prints number
        while(input.find_first_not_of("idsr") == string::npos) { 
            input = get_input();

            int new_number = active_user.get_num();

            if(input == "i") new_number++;
    
            if(input == "d") new_number--;

            if(input == "s") new_number = stoi(get("Enter int to set: "));

            if(input == "r") new_number = 0;

            active_user.set_num(new_number);
            u_database.login(active_user);
            u_database.save_data();
            cout << endl << endl << active_user.get_num() << endl << endl;
        }

        cout << "Saving data and exiting your account...\n\n";
        u_database.logout();
    }

    void create_acc() {
        string username;
        string password;

        username = get_new_username();
        password = get_new_password();
        confirm_new_password(password);

        User new_user(username, password);
        new_user.save_user();
    }

    void delete_acc() {}

    void forgot_password() {}

    void show_extra_commands() {}

private:
    string get_new_username() {
        string username;

        cout << "Enter new username: ";
        cin >> username;
        cout << endl;

        while (username.length() < MIN_USERNAME_LENGTH || username.length() > MAX_USERNAME_LENGTH ||
               !is_new_username(username)) {
            if (username.length() < MIN_USERNAME_LENGTH ||
                username.length() > MAX_USERNAME_LENGTH) {
                cout << "Username must have between " << MIN_USERNAME_LENGTH << " to "
                     << MAX_USERNAME_LENGTH << " characters" << endl;
                cout << "Please enter username again: ";
                cin >> username;
                cout << endl;
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
private: 
    User_base u_database;
};


int main() { 
    Start main_page;
    main_page.main_loop();
}
