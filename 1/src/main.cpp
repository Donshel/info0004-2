// g++ -std=c++11 -Wall -Wextra src/main.cpp src/anagrams.cpp -o bin/anagrams

#include <ctime>
#include <iostream>
#include <fstream>
#include "anagrams.hpp"

using namespace std;

int main() {
    Dictionary dict = create_dictionary("../resources/txt/sowpods.txt");

    string input;
    unsigned int max;

    cout << "Enter a string :" << endl;
    getline(cin, input);
    cout << "Enter a number :" << endl;
    cin >> max;

    time_t t = time(NULL);
    vector<vector<string>> vect = anagrams(input, dict, max);
    t -= time(NULL);
    t *= -1;

    ofstream file;
    file.open("anagram.txt");

    file << input + " in " << t << " secs for " << vect.size() << " anagrams\n\n";

    for (auto it = vect.begin(); it != vect.end(); it++) {
        for (auto it2 = (*it).begin(); it2 != (*it).end(); *it2++) {
            file << *it2 + " ";
        }

        file << "\n";
    }

    file.close();

    cout << input + " in " << t << " secs for " << vect.size() << " anagrams" << endl;

    return 0;
}
