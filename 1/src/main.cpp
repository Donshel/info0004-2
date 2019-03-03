// g++ -std=c++11 -Wall -Wextra src/main.cpp src/anagrams.cpp -o bin/anagrams

#include <chrono>
#include <iostream>
#include <fstream>
#include "anagrams.hpp"

using namespace std;

int main() {
    auto start = chrono::steady_clock::now();

    Dictionary dict = create_dictionary("../resources/txt/sowpods.txt");

    auto end = chrono::steady_clock::now();
    auto diff = end - start;
    auto time1 = chrono::duration <double, milli> (diff).count();

    string input;
    unsigned int max;

    cout << "Enter a string :" << endl;
    getline(cin, input);
    cout << "Enter a number :" << endl;
    cin >> max;

    start = chrono::steady_clock::now();

    vector<vector<string>> vect = anagrams(input, dict, max);

    end = chrono::steady_clock::now();
    diff = end - start;
    auto time2 = chrono::duration <double, milli> (diff).count();

    ofstream file;
    file.open("anagram.txt");

    file << input + "\n\n";
    file << "create_dictionary time : " << time1 << " ms" << "\n";
    file << "anagrams time : " << time2 << " ms" << "\n";
    file << "anagrams number : " << vect.size() << "\n\n";

    for (auto it = vect.begin(); it != vect.end(); it++) {
        for (auto it2 = (*it).begin(); it2 != (*it).end(); *it2++) {
            file << *it2 + " ";
        }

        file << "\n";
    }

    file.close();

    cout << "create_dictionary time : " << time1 << " ms" << endl;
    cout << "anagrams time : " << time2 << " ms" << endl;
    cout << "anagrams number : " << vect.size() << endl;

    return 0;
}
