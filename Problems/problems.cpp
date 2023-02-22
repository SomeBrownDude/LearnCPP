#include <stdio.h>

#include <algorithm>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <thread>
#include <vector>
using namespace std;

// Sum of 3/5 multiples: Prob: 1
int sum_of_3_5_multiples(int n) {
    int res = 0;
    for (int i = 1; i < n; i++) {
        if (i % 3 == 0 || i % 5 == 0)
            res += i;
    }
    return res;
}

// Sum of even fib nums <= 4 mil: Prob: 2
int even_fib_nums(int n) {
    vector<int> fibs = {1, 2};
    for (int i = 1; fibs[i] <= n; i++) {
        fibs.push_back(fibs[i] + fibs[i - 1]);
    }
    int sum = 0;
    for (int i = 0; i < fibs.size(); i++) {
        if (fibs[i] % 2 == 0)
            sum += fibs[i];
    }
    return sum;
}

bool isPrime(long long n) {
    if (n <= 1) {
        return false;
    }
    if (n <= 3) {
        return true;
    }
    if (n % 2 == 0 || n % 3 == 0) {
        return false;
    }
    for (long long i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}

bool isPrime(int n) {
    if (n <= 1) {
        return false;
    }
    if (n <= 3) {
        return true;
    }
    if (n % 2 == 0 || n % 3 == 0) {
        return false;
    }
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}

// largest prime factor of a number, -1 if no prime factor Prob: 3
long long largest_prime_factor(long long n) {
    for (long long i = sqrt(n); i > 1; i--) {
        if (isPrime(i) && n % i == 0) {
            return i;
        }
    }
    return -1;
}

bool is_palindrome(const string &s) {
    string rstring = s;
    reverse(rstring.begin(), rstring.end());
    return rstring == s;
}

// largest palindrome product of two numbers with 3 digits Prob: 4
int largest_palindrome_product() {
    vector<int> three_digit_prods;
    for (int i = 999; i >= 100; i--) {
        for (int j = 999; j >= 100; j--) {
            three_digit_prods.push_back(i * j);
        }
    }
    vector<int> palindromes;
    for (int n : three_digit_prods) {
        if (is_palindrome(to_string(n)))
            palindromes.push_back(n);
    }
    return *max_element(palindromes.begin(), palindromes.end());
}

void replace_digits() {}

// Prob: 5
int eight_prime_family() {
    for (int i = 2;; i++) {  // 56993 is the highest fam mem of the lowest seven prime family num
    }
}

int gcd(int a, int b) { return b == 0 ? a : gcd(b, a % b); }

vector<vector<int>> readMatrixFromFile(string file_name, int num_lines) {
    vector<vector<int>> matrix;  // create a vector of vectors to store the matrix

    // open the file for reading
    ifstream infile(file_name);

    // loop over each line in the file
    for (int i = 0; i < num_lines; i++) {
        string line;
        getline(infile, line);  // read a line from the file

        vector<int> row;  // create a vector to store the row

        // use a stringstream to split the line into integers
        stringstream ss(line);
        int val;
        char sep;
        while (ss >> val) {
            row.push_back(val);  // add the integer to the row
            ss >> sep;           // read the comma separator
        }

        matrix.push_back(row);  // add the row to the matrix
    }

    return matrix;
}

// prob: 71 Project Euler
int ordered_factions() {
    int res        = 0;
    double closest = 2 / 5.;

    for (int d = 2; d <= 1000000; d++) {
        for (int n = d * closest; n < d; n++) {
            double frac = static_cast<double>(n) / d;
            if (frac > 3 / 7.)
                break;

            if (gcd(n, d) == 1 && frac > closest && frac < 3 / 7.) {
                closest = frac;
                res     = n;
            }
        }
    }

    return res;
}

// ! fix
// prob: 81
int min_path_sum(const vector<vector<int>> &matrix) {
    int sum = 0;
    for (int y = 0, x = 0; y < matrix.size() && x < matrix[y].size();) {
        int right = matrix[y][x + 1];
        int down  = matrix[y + 1][x];

        if (right < down) {
            sum += right;
            x++;
        } else {
            sum += down;
            y++;
        }
    }
    return sum;
}

int rev_num(int n) {
    string num = to_string(n);
    reverse(num.begin(), num.end());
    return stoi(num);
}

bool is_odd_digits(int n) {
    while (n > 0) {
        int digit = n % 10;
        if (digit % 2 == 0)
            return false;
        n = n / 10;
    }
    return true;
}

void countReversible(int n) {
    unsigned long long count = 0;

    // Calculate counts of
    // reversible number of 1 to n-digits
    for (int i = 1; i <= n; i++) {
        // All four possible cases and their formula
        switch (i % 4) {
            // for i of form 2k
            case 0:
            case 2:
                count += 20 * pow(30, (i / 2 - 1));
                break;

            // for i of form 4k + 3
            case 3:
                count += 100 * pow(500, i / 4);
                break;

            // for i of form 4k + 1 no solution
            case 1:
                break;
        }
    }
    cout << count;
}

// prob: 145 number of reversible numbers
int reversible(int n) {
    int res = 0;
    for (int i = 1; i <= n; i++) {
        if (i % 10 == 0)
            continue;

        int 
        /*
    High Level instructions for Wordle game:
    1. get user input
    2. pick random word from word list
    3. iterate through each char and tell user using colors whether each char in their input
        is in the random word and whether it is in the correct place
    4. keep getting guesses (until 6 guesses or the user gets the right word)
    5. save user data and print an exit message
*/
        rnum = rev_num(i);
        if (is_odd_digits(i + rnum))
            res++;
    }
    return res;
}

bool check_div(int n) {
    for (int i = 4; i <= 20; i++) {
        if (n % i != 0)
            return false;
    }
    return true;
}

// prob 5:
int smallest_multiple(int d) {
    for (int i = 1; i < INT_MAX; i++) {
        if (i % 2 == 0 && i % 5 == 0 && i % 3 == 0) {
            if (check_div(i))
                return i;
        }
    }
    return -1;
}

// prob 6:
int sumSquareDiff(int n) {
    int sumOfSquares = 0;
    int sum          = 0;
    for (int i = 0; i <= n; i++) {
        sumOfSquares += i * i;
        sum += i;
    }
    return sum * sum - sumOfSquares;
}

int nthPrime(int n) {
    int count = 0;
    for (int i = 1;; i += 2) {
        if (isPrime(i))
            count++;
        if (count == n)
            return i;
    }
    return -1;
}

// prob 8:
long long greatest_product(string s) {
    vector<int> digits;
    for (char c : s) digits.push_back(c - '0');

    long long greatest = 1;

    for (int i = 0; i + 12 < digits.size(); i++) {
        long long prod = 1;
        for (int k = i; k <= i + 12; k++) {
            prod *= digits[k];
        }
        if (prod > greatest)
            greatest = prod;
    }
    return greatest;
}

class thing {};

// prob 9:
int pythagorean_triplet_product(int limit) {
    for (int n = 1; n <= limit; n++) {
        for (int m = n; m <= limit; m++) {
            int a = m * m - n * n;
            int b = 2 * m * n;
            int c = m * m + n * n;

            if (a + b + c == limit) {
                return a * b * c;
            }
        }
    }
    return -1;
}

// prob 10:
unsigned long long sum_of_primes(int n) {
    unsigned long long sum = 0;
    for (int i = 1; i <= n; i++) {
        if (isPrime(i))
            sum += i;
    }
    return sum;
}

// prob 11:
long long greatest_product_in_grid(string s) {
    vector<vector<int>> digits;
    for (char c : s) {
        vector<int> temp;
        temp.push_back(c);
        if (c == '\n') {
            digits.push_back(temp);
            temp.clear();
        }
    }

    for (auto &i : digits) {
        for (auto n : i) {
            cout << n << " ";
        }
        cout << endl;
    }
    return 0;

    //     long long greatest = 1;

    //     for (int i = 0; i + 4 < digits[i].size(); i++) {
    //         long long prod = 1;
    //         for (int k = i; k <= i + 4; k++) {
    //             prod *= digits[k];
    //         }
    //         if (prod > greatest)
    //             greatest = prod;
    //     }

    //     for (int i = 0; i + 4 < digits)

    //     return greatest;
}

unsigned long long summation(int n) {
    int sum = 0;
    for(unsigned long long i = 1; i <= n; i++) {
        sum += i;
    }
    return sum;
}

// prob 12:
unsigned long long highly_divisible_triangle_number(int divisors) {
    for (unsigned long long i = 1;; i++) {
        int num = (i * (i + 1)) / 2; // summation from 1... num
        int div_count = 0;

        for (int j = 1; j <= sqrt(num); j++) 
            if (num % j == 0) div_count += 2;
        
        if (div_count >= divisors)
            return num;
    }
    return -1;
}

int main(){
    cout << highly_divisible_triangle_number(500) << endl;
}

