// g++ -std=c++11 -Wall -Wextra src/main.cpp src/anagrams.cpp -o bin/anagrams

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

    vector<vector<string>> vect = anagrams(input, dict, max);

    ofstream file;
    file.open("anagram.txt");

    file << input + "\n\n";
    cout << endl;

    for (auto it = vect.begin(); it != vect.end(); it++) {
        for (auto it2 = (*it).begin(); it2 != (*it).end(); *it2++) {
            file << *it2 + " ";
            cout << *it2 + " ";
        }
        
        file << "\n";
        cout << endl;
    }

    file.close();

    return 0;
}
