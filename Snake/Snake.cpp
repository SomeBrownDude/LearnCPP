#include <ncurses.h>
#include <termios.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ostream>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>
using namespace std;

int rnum(int min, int max) {
    std::random_device rd;      // random device engine, usually based on /dev/random on Unix-like systems
    std::mt19937 engine(rd());  // Mersenne Twister 19937 generator
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(engine);
}

struct Position {  // stores any x, y position.
    Position(int x, int y) : x(x), y(y) {}
    Position() {}
    int x, y;
    const char* element;
};

void replace_line_in_file(string filename, string line, string new_line) {
    string temp_filename = filename + ".tmp";
    fstream original_file(filename, ios::in);
    fstream temp_file(temp_filename, ios::out);

    if (!original_file.is_open() || !temp_file.is_open()) {
        cout << "Error opening file" << endl;
        return;
    }

    string current_line;
    while (getline(original_file, current_line)) {
        if (current_line == line) {
            temp_file << new_line << endl;
        } else {
            temp_file << current_line << endl;
        }
    }

    original_file.close();
    temp_file.close();

    remove(filename.c_str());
    rename(temp_filename.c_str(), filename.c_str());
}

class Fruit {
public:
    Fruit(const char* fruit_ch) : fruit_char(fruit_ch) {}

    void spawn(const vector<Position>& available_spawns) { pos = available_spawns[rnum(0, available_spawns.size() - 1)]; }

    Position get_pos() { return pos; }

    void draw() {  // ! refresh()?? or just spawn and refresh later?
        mvprintw(pos.y, pos.x, fruit_char);
    }

private:
    Position pos;
    const char* fruit_char;
};

class Snake {
public:
    Snake() {}
    Snake(int y, int x, int len) {  // builds snake at x, y that is len segments long.
        Position segment;
        segments = vector<Position>(len, segment);

        for (int i = 0; i < segments.size(); i++) {
            segments[i].y = y;
            segments[i].x = x - i - 1;
        }
    }

    void move_head(int dir, int pos = 0) {
        if (dir == 'w' || dir == KEY_UP) {  // extra delay for vertical movement
            segments[pos].y--;
        }
        if (dir == 's' || dir == KEY_DOWN) {  // extra delay
            segments[pos].y++;
        }
        if (dir == 'd' || dir == KEY_RIGHT) {
            segments[pos].x++;
        }
        if (dir == 'a' || dir == KEY_LEFT) {
            segments[pos].x--;
        }
        return;
    }

    void grow() {  // todo: fix this shit, spawn diff based on dir
        Position new_segment;
        Position last_seg        = segments.back();
        Position second_last_seg = segments[segments.size() - 3];

        if (last_seg.x == second_last_seg.x) {
            if (last_seg.x == second_last_seg.x - 1) new_segment = Position(last_seg.x - 1, last_seg.y);
            if (last_seg.x == second_last_seg.x + 1) new_segment = Position(last_seg.x + 1, last_seg.y);
        }
        if (last_seg.y == second_last_seg.y) {
            if (last_seg.y == second_last_seg.y - 1) new_segment = Position(last_seg.x, last_seg.y + 1);
            if (last_seg.x == second_last_seg.x + 1) new_segment = Position(last_seg.x, last_seg.y + 1);
        }
        segments.push_back(new_segment);
    }

    void follow(const vector<Position>& old_snake) {
        for (int i = 1; i < segments.size(); i++) {
            segments[i].x = old_snake[i - 1].x;
            segments[i].y = old_snake[i - 1].y;
        }
    }

    bool check_head_hit_body() {
        for (int i = 1; i < segments.size(); i++) {
            if (segments[0].x == segments[i].x && segments[0].y == segments[i].y) return true;
        }
        return false;
    }

    bool is_opposite(int curr_dir, int new_dir) {
        if (curr_dir == 'w' || curr_dir == KEY_UP) {
            return new_dir == 's' || new_dir == KEY_DOWN;
        }
        if (curr_dir == 's' || curr_dir == KEY_DOWN) {  // extra delay
            return new_dir == 'w' || new_dir == KEY_UP;
        }
        if (curr_dir == 'd' || curr_dir == KEY_RIGHT) {
            return new_dir == 'a' || new_dir == KEY_LEFT;
        }
        if (curr_dir == 'a' || curr_dir == KEY_LEFT) {
            return new_dir == 'd' || new_dir == KEY_RIGHT;
        }
        return false;
    }

    vector<Position> move_once(int dir) {
        vector<Position> old_snake = segments;
        move_head(dir);
        follow(old_snake);
        return segments;
    }

    vector<Position> snake_pos() { return segments; }  // curr position of snake

private:
private:
    vector<Position> segments;
};

class Grid {
public:
    ~Grid() {}
    Grid(int width, int height) : WIDTH(width), HEIGHT(height) {
        vector<vector<string>> temp(HEIGHT, vector<string>(WIDTH, " "));
        grid = temp;

        fill_grid();
    }

    void spawn_snake(int y, int x, int len) {
        Snake new_snake(y, x, len);
        snake = new_snake;
    }

    bool is_opposite(int curr_dir, int new_dir) {
        if (curr_dir == 'w' || curr_dir == KEY_UP) {
            return new_dir == 's' || new_dir == KEY_DOWN;
        }
        if (curr_dir == 's' || curr_dir == KEY_DOWN) {  // extra delay
            return new_dir == 'w' || new_dir == KEY_UP;
        }
        if (curr_dir == 'd' || curr_dir == KEY_RIGHT) {
            return new_dir == 'a' || new_dir == KEY_LEFT;
        }
        if (curr_dir == 'a' || curr_dir == KEY_LEFT) {
            return new_dir == 'd' || new_dir == KEY_RIGHT;
        }
        return false;
    }

    void clear_grid() {
        vector<vector<string>> temp(HEIGHT, vector<string>(WIDTH, " "));
        grid = temp;

        fill_grid();
    }

    void write_to_grid(Position element) {
        int x      = element.x;
        int y      = element.y;
        grid[y][x] = "@";
    }

    void write_to_grid(vector<Position> element) {
        clear_grid();

        for (int i = 0; i < element.size(); i++) {
            int x = element[i].x;
            int y = element[i].y;

            i == 0 ? grid[y][x] = "0" : grid[y][x] = "+";
        }
    }

    bool hit_wall() {
        int x = snake.snake_pos()[0].x;
        int y = snake.snake_pos()[0].y;

        if (y == 0 || y == grid.size() - 1) return true;

        if (x == 0 || x == grid[y].size() - 1) return true;

        return false;
    }

    bool fruit_eaten(Position fruit) {
        vector<Position> s_pos = snake.snake_pos();
        if (s_pos[0].x == fruit.x && s_pos[0].y == fruit.y) return true;
        return false;
    }

    bool save_score(int score) {
        fstream scores_file("hscores.txt", ios::in | ios::out);
        int highscore = 0;

        if (!scores_file.good()) {
            cout << "File Error at: " << __LINE__ << endl;
            return false;
        }

        scores_file >> highscore;
        if (score > highscore) replace_line_in_file("hscores.txt", to_string(highscore), to_string(score));

        scores_file.clear();
        scores_file.seekg(0, ios::beg);
        scores_file >> highscore;

        scores_file.flush();
        scores_file.close();
        return score > highscore;
    }

    int get_highscore(string file) {
        fstream f(file);
        int highscore = 0;
        f >> highscore;
        f.close();
        return highscore;
    }

    void end_game(int score) {
        endwin();

        bool newhigh     = save_score(score);
        string final_msg = "Game over! \n\n\n\nScore: " + to_string(snake.snake_pos().size() - 1) + "\n\n\n\nHIGH SCORE: ";
        final_msg += to_string(get_highscore("hscores.txt"));

        if (newhigh) final_msg += "\n\n\nWow, NEW HIGH SCORE!!!";
        cout << "\033[32m" << final_msg << "\033[0m";
        cout << "\n\n\n";

        exit(0);
    }

    void move_snake() {
        int input;
        int new_dir = ERR;
        int curr_dir;

        vector<Position> snake_pos = snake.snake_pos();
        write_to_grid(snake_pos);
        Position new_fruit = spawn_fruit();
        write_to_grid(new_fruit);
        draw();

        while ((input = getch()) != 'e') {
            if (input != ERR) {
                curr_dir = new_dir;
                new_dir  = input;
            }
            if (new_dir != ERR) {
                if (is_opposite(curr_dir, new_dir)) new_dir = curr_dir;
                vector<Position> snake_pos1 = snake.move_once(new_dir);
                if (snake.check_head_hit_body() || hit_wall()) {
                    end_game(snake.snake_pos().size() - 1);
                }
                write_to_grid(snake_pos1);
                write_to_grid(new_fruit);

                if (fruit_eaten(new_fruit)) {
                    snake.grow();
                    new_fruit = spawn_fruit();
                }
                draw();
            }
            this_thread::sleep_for(chrono::milliseconds(90));
        }
    }

    // goes through all grid coordinates and finds coordinates which are empty.
    // stores these positions in a vector of positions and returns it.
    vector<Position> get_free_positions() {
        vector<Position> free_positions;
        for (int y = 1; y < HEIGHT - 1; y++) {
            for (int x = 1; x < WIDTH - 1; x++) {
                if (grid[y][x] == " ") {  // math to find equivalent position in vec
                    Position free_pos(x, y);
                    free_positions.push_back(free_pos);
                }
            }
        }
        return free_positions;
    }

    Position spawn_fruit() {
        Fruit new_fruit("0");
        vector<Position> available_positions = get_free_positions();

        new_fruit.spawn(available_positions);
        return new_fruit.get_pos();
    }

    void draw() {
        clear();
        for (auto i : grid) {
            for (auto j : i) printw(j.c_str());
            printw("\n");
        }
        attron(A_BOLD);
        mvprintw(20, 50, "Snake Game");
        mvprintw(21, 50, "By: Jonathan");
        mvprintw(23, 50, "Score: ");
        mvprintw(23, 58, to_string(snake.snake_pos().size() - 1).c_str());
        attroff(A_BOLD);
        refresh();
    }

private:
    void fill_grid() {
        for (int i = 0; i < grid.size(); i++) {
            if (i == 0 || i == grid.size() - 1) fill(grid[i].begin(), grid[i].end(), "#");
            grid[i].front() = "#";
            grid[i].back()  = "#";
        }
    }

    int WIDTH  = 0;
    int HEIGHT = 0;
    vector<vector<string>> grid;
    Snake snake;
};

const int WIDTH  = 40;
const int HEIGHT = 40;

int main() {
    initscr();
    noecho();
    curs_set(0);
    cbreak();
    keypad(stdscr, true);
    nodelay(stdscr, true);

    Grid game_board(WIDTH, HEIGHT);
    game_board.spawn_snake(20, 20, 1);  // initializing snake
    game_board.move_snake();            // game loop, gets input and moves snake

    getch();
    endwin();
}