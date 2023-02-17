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

const string SNAKE_HEAD   = "0";
const string SNAKE_BODY   = "+";
const string FRUIT_CHAR   = "@";
const int HEIGHT          = 30;
const int WIDTH           = 40;
const int SNAKE_X         = WIDTH / 2;
const int SNAKE_Y         = HEIGHT / 2;
const int SNAKE_START_LEN = 1;
const int DELAY           = 50;  // Snake speed (lower = faster, 90 seems to be good speed)

int rnum(int min, int max) {
    std::random_device rd;      // random device engine, usually based on /dev/random on Unix-like systems
    std::mt19937 engine(rd());  // Mersenne Twister 19937 generator
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(engine);
}

struct Position {  // stores any x, y position.
    Position() {}
    Position(int x, int y) : x(x), y(y) {}
    int x, y;

    bool operator==(const Position other) const { return x == other.x && y == other.y; }
};

class Fruit {
public:
    Fruit() {}
    Position spawn(const vector<Position>& available_spawns) {
        pos = available_spawns[rnum(0, available_spawns.size() - 1)];
        return pos;
    }

private:
    Position pos;
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

    void grow() {
        Position new_segment;
        segments.push_back(new_segment);
        Position last_seg        = segments.back();
        Position second_last_seg = segments[segments.size() - 2];

        // This checks x and y coordinates of the last 2 segments to determine where to grow next segment
        if (last_seg.x == second_last_seg.x) {
            if (last_seg.x == second_last_seg.x - 1)
                new_segment = Position(last_seg.x - 1, last_seg.y);
            if (last_seg.x == second_last_seg.x + 1)
                new_segment = Position(last_seg.x + 1, last_seg.y);
        }
        if (last_seg.y == second_last_seg.y) {
            if (last_seg.y == second_last_seg.y - 1)
                new_segment = Position(last_seg.x, last_seg.y + 1);
            if (last_seg.x == second_last_seg.x + 1)
                new_segment = Position(last_seg.x, last_seg.y + 1);
        }
        segments[segments.size() - 1] = new_segment;
    }

    bool check_head_hit_body() {
        for (int i = 1; i < segments.size(); i++) {
            if (segments[0].x == segments[i].x && segments[0].y == segments[i].y)
                return true;
        }
        return false;
    }

    bool fruit_eaten(Position fruit_pos) {
        if (segments[0].x == fruit_pos.x && segments[0].y == fruit_pos.y)
            return true;
        return false;
    }

    bool hit_wall() {
        if (segments[0].x == 0 || segments[0].x == WIDTH)
            return true;
        if (segments[0].x == 0 || segments[0].y == HEIGHT)
            return true;

        return false;
    }

    void move_once(int dir) {  // moves snake one position based on direction (moves head, then all segments follow head)
        vector<Position> old_snake = segments;
        move_head(dir);
        follow(old_snake);
    }

    vector<Position> snake_pos() { return segments; }  // curr position of snake
private:
    void follow(const vector<Position>& old_snake) {
        for (int i = 1; i < segments.size(); i++) {
            segments[i].x = old_snake[i - 1].x;
            segments[i].y = old_snake[i - 1].y;
        }
    }

    void move_head(int dir) {
        if (dir == 'w' || dir == KEY_UP) {
            segments[0].y--;
        }
        if (dir == 's' || dir == KEY_DOWN) {
            segments[0].y++;
        }
        if (dir == 'd' || dir == KEY_RIGHT) {
            segments[0].x++;
        }
        if (dir == 'a' || dir == KEY_LEFT) {
            segments[0].x--;
        }
    }

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

    void loop() {
        int input;
        // int new_dir = ERR;
        int curr_dir;

        vector<Position> snake_pos = snake.snake_pos();
        write_to_grid(snake_pos);
        Position new_fruit_pos = spawn_fruit();
        write_to_grid(new_fruit_pos, FRUIT_CHAR);
        draw();

        while (true) {
            if (!is_opposite(curr_dir, input) && input != ERR)
                curr_dir = input;

            snake.move_once(curr_dir);

            if (snake.check_head_hit_body() || snake.hit_wall() || curr_dir == 'q') {
                end_game(snake.snake_pos().size() - 1);

            write_to_grid(snake.snake_pos());
            write_to_grid(new_fruit_pos, FRUIT_CHAR);

            if (snake.fruit_eaten(new_fruit_pos)) {
                snake.grow();
                new_fruit_pos = spawn_fruit();
            }
            draw();

            // if (new_dir != ERR) {
            //     if (is_opposite(curr_dir, new_dir)) new_dir = curr_dir;

            //     snake.move_once(new_dir);
            //     if (snake.check_head_hit_body() || snake.hit_wall()) {
            //         end_game(snake.snake_pos().size() - 1);
            //     }

            //     write_to_grid(snake.snake_pos());
            //     write_to_grid(new_fruit_pos, FRUIT_CHAR);

            //     if (snake.fruit_eaten(new_fruit_pos)) {
            //         snake.grow();
            //         new_fruit_pos = spawn_fruit();
            //     }
            //     draw();
            // }
            this_thread::sleep_for(chrono::milliseconds(DELAY));
        }
    }

    bool is_opposite(int curr_dir, int new_dir) {  // checks if user tries to make snake in the opposite direction (impossible)
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

    void write_to_grid(Position point, const string& element) {  // writes fruit
        int x      = point.x;
        int y      = point.y;
        grid[y][x] = element;
    }

    void write_to_grid(vector<Position> element) {  // writes snake
        clear_grid();

        for (int i = 0; i < element.size(); i++) {
            int x = element[i].x;
            int y = element[i].y;

            i == 0 ? grid[y][x] = SNAKE_HEAD : grid[y][x] = SNAKE_BODY;
        }
    }

    bool save_score(int score) {
        fstream scores_file("hscores.txt", ios::in | ios::out);
        int highscore = 0;

        if (!scores_file.good()) {
            cout << "File Error at: " << __LINE__ << endl;
            return false;
        }

        scores_file >> highscore;
        if (score > highscore)
            replace_line_in_file("hscores.txt", to_string(highscore), to_string(score));

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

        if (newhigh)
            final_msg += "\n\n\nWow, NEW HIGH SCORE!!!";
        cout << "\033[32m" << final_msg << "\033[0m";
        cout << "\n\n\n";

        exit(0);
    }

    // goes through all grid coordinates and finds coordinates which are empty.
    // stores these positions in a vector of positions and returns it.
    vector<Position> get_free_positions() {
        vector<Position> free_positions;
        for (int y = 1; y < HEIGHT - 1; y++) {
            for (int x = 1; x < WIDTH - 1; x++) {
                if (grid[y][x] == " ") {
                    Position free_pos(x, y);
                    free_positions.push_back(free_pos);
                }
            }
        }
        return free_positions;
    }

    Position spawn_fruit() {
        Fruit new_fruit;
        vector<Position> available_positions = get_free_positions();

        return new_fruit.spawn(available_positions);
    }

    void draw() {
        std::string output_buffer;
        for (const auto& row : grid) {
            for (const auto& cell : row) {
                output_buffer += cell;
            }
            output_buffer += "\n";
        }
        mvprintw(0, 0, output_buffer.c_str());
        attron(A_BOLD);
        mvprintw(HEIGHT / 2, WIDTH + 9, "Snake Game");
        mvprintw(HEIGHT / 2 + 1, WIDTH + 9, "By: Jonathan (q to quit)");
        mvprintw(HEIGHT / 2 + 2, WIDTH + 9, "(WASD or arrow keys to change direction)");
        mvprintw(HEIGHT / 2 + 4, WIDTH + 9, "Score: ");
        mvprintw(HEIGHT / 2 + 5, WIDTH + 9, to_string(snake.snake_pos().size() - 1).c_str());
        attroff(A_BOLD);
        refresh();
    }

private:
    void fill_grid() {
        for (int i = 0; i < grid.size(); i++) {
            if (i == 0 || i == grid.size() - 1)
                fill(grid[i].begin(), grid[i].end(), "#");
            grid[i].front() = "#";
            grid[i].back()  = "#";
        }
    }

    int WIDTH  = 0;
    int HEIGHT = 0;
    vector<vector<string>> grid;
    Snake snake;
};

int main() {
    initscr();
    noecho();
    curs_set(0);
    cbreak();
    keypad(stdscr, true);
    nodelay(stdscr, true);

    Grid game(WIDTH, HEIGHT);
    game.spawn_snake(SNAKE_Y, SNAKE_X, SNAKE_START_LEN);
    game.loop();

    getch();
    endwin();
}
