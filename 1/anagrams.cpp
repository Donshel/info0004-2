// g++ -std=c++11 -Wall -Wextra anagrams.cpp -o anagrams
// g++ -std=c++11 -Wall anagrams.cpp -o anagrams

#include <iostream>
#include <fstream>

#include <algorithm>

#include <utility>
#include <string>
#include <map>
#include <set>
#include <vector>

using namespace std;

// File

typedef vector<string> Dictionary;

Dictionary create_dictionary(const string& filename) {
    Dictionary dictionary;
    string word;
    ifstream file;

    file.open(filename);

    if (!file) {
        cout << "Unable to open file." << endl;
        exit(1);
    }

    while (file >> word)
        dictionary.push_back(word);

    file.close();

    return dictionary;
}

// Strings management

string cleanUp(string str) {
    str.erase(remove(str.begin(), str.end(), ' '), str.end());

    for (auto it = str.begin(); it != str.end(); it++)
        if (*it >= 65 && *it <= 92)
            *it += 32;

    return str;
}

string alphaSort(string str) {
    sort(str.begin(), str.end());

    return str;
}

string removeOnce(string str1, string str2) {
    str2 = alphaSort(str2);

    auto pt = str1.begin();

    for (auto it = str2.begin(); it != str2.end(); it++) {
        pt = find(pt, str1.end(), *it);

        if (pt == str1.end())
            return "1";

        str1.erase(pt);
    }

    return str1;
}

//

typedef vector<string> svector;
typedef vector<pair<unsigned long, string>> pvector;
typedef map<string, pvector> hashMap;

void buildHash(string str, const Dictionary& dict, set<unsigned long> indexes, hashMap& hash, int n) {
    if (hash.find(str) != hash.end())
        return;

    hash[str] = pvector();

    if (n == 0)
        return;

    for (auto it = indexes.begin(); it != indexes.end();) {
        string residual = removeOnce(str, dict[*it]);

        if (residual != "1") {
            hash.at(str).push_back(pair<unsigned long, string> (*it, residual));
            it++;
        } else {
            it = indexes.erase(it);
        }
    }

    for (auto it = hash.at(str).begin(); it != hash.at(str).end(); it++)
        if ((*it).second != "") {
            buildHash((*it).second, dict, indexes, hash, n - 1);
        }

    return;
}

void buildAnagrams(const string& str, const Dictionary& dict, const hashMap& hash, svector curr, vector<svector>& anagrams, int n) {
    if (n == 0)
        return;

    for (auto it = hash.at(str).begin(); it != hash.at(str).end(); it++) {
        curr.push_back(dict[(*it).first]);
        if ((*it).second != "")
            buildAnagrams((*it).second, dict, hash, curr, anagrams, n - 1);
        else
            anagrams.push_back(curr);
        curr.pop_back();
    }

    return;
}

vector<svector> anagrams(const string& input, const Dictionary& dict, unsigned max) {
    hashMap hash;
    set<unsigned long> indexes;
    vector<svector> anagrams;

    for (unsigned long i = 0; i < dict.size(); i++)
        indexes.insert(indexes.end(), i);

    string str = alphaSort(cleanUp(input));

    int n = max;
    if (n == 0)
        n = -1;

    buildHash(str, dict, indexes, hash, n);
    buildAnagrams(str, dict, hash, svector(), anagrams, n);

    return anagrams;
}

int main() {
    Dictionary dict = create_dictionary("sowpods.txt");

    string input;
    unsigned int max;

    cout << "Enter a string :" << endl;
    getline(cin, input);
    cout << "Enter a number :" << endl;
    cin >> max;

    vector<svector> list = anagrams(input, dict, max);

    cout << endl;
    for (auto it = list.begin(); it != list.end(); it++) {
        for (auto it2 = (*it).begin(); it2 != (*it).end(); *it2++)
            cout << *it2 + " ";
        cout << endl;
    }

    return 0;
}
