#include <iostream>
#include <fstream>

#include <algorithm> /// std::sort

#include <utility> /// std::pair
#include <string>
#include <set>
#include <vector>

#include "anagrams.hpp"

using namespace std;

/**
 * Remove, from a string, undesired characters such as spaces, figures, etc. and replace uppercases by corresponding lowercases.
 *
 * @param str, the string to clean up.
 * @return the cleaned up string.
 */
string cleanUp(string str) {
    for (auto it = str.begin(); it != str.end();)
        if (*it >= 65 && *it <= 90)
            *(it++) += 32;
        else if (*it >= 97 && *it <= 122)
            it++;
        else
            it = str.erase(it);

    return str;
}

/**
 * Remove from one string all characters of another. One character is removed for each its occurences in the other string.
 *
 * @param str1, the string where are removed characters from. str1 is supposed to be alphabetically sorted.
 * @param str2, the other string.
 * @return the residual string, or "1" if str2 has character(s) that str1 hasn't.
 */
string removeFrom(string str1, string str2) {
    sort(str2.begin(), str2.end());

    auto pt = str1.begin();

    for (auto it = str2.begin(); it != str2.end(); it++) {
        pt = find(pt, str1.end(), *it);

        if (pt == str1.end())
            return "1";

        str1.erase(pt);
    }

    return str1;
}

typedef vector<string> Dictionary;

/**
 * Create a dictionary from a file.
 *
 * @note The words in the file have to be lowercase.
 *
 * @param filename, the path to the file.
 * @return the created dictionary.
 */
Dictionary create_dictionary(const string& filename) {
    Dictionary dict;
    string word;
    ifstream file;

    file.open(filename);

    if (!file) {
        cout << "Unable to open file." << endl;
        exit(1);
    }

    while (file >> word)
        dict.push_back(word);

    file.close();

    return dict;
}

/**
 * Compute all anagrams of a string, recursively.
 *
 * @param[in] str, the string whose anagrams are searched.
 * @param[in] dict, the used dictionary.
 * @param[in] indexes, a container of indexes of words of the dictionary in which it is still useful to search anagrams.
 * @param[in] current, the container of previous words in the current anagram.
 * @param[in, out] anagrams, the container of already-computed anagrams.
 * @param[in] max, the maximum number of words that are allowed to be added to current. If max is negative, there is no limit.
 */
void build(string str, const Dictionary& dict, set<unsigned long> indexes, vector<string> current, vector<vector<string>>& anagrams, int max) {
    if (max == 0)
        return;

    vector<pair<unsigned long, string>> tree;

    /// Search fitting words
    for (auto it = indexes.begin(); it != indexes.end();) {
        string residual = removeFrom(str, dict[*it]);

        if (residual != "1")
            tree.push_back(pair<unsigned long, string>(*(it++), residual));
        else
            it = indexes.erase(it);
    }

    /// If no fitting word found
    if (tree.empty())
        return;

    /// Start again recursively for each fitting word found
    for (auto it = tree.begin(); it != tree.end(); it++) {
        indexes.erase(indexes.begin(), indexes.find((*it).first));

        current.push_back(dict[(*it).first]);

        if ((*it).second != "")
            build((*it).second, dict, indexes, current, anagrams, max - 1);
        else
            anagrams.push_back(current);

        current.pop_back();
    }
}

/**
 * Generate all anagrams of a string.
 *
 * @param input, the string whose anagrams are searched.
 * @param dict, the used dictionary.
 * @param max, the maximum number of words that are allowed in anagrams. If max is null, there is no limit.
 * @return the container of generated anagrams.
 */
vector<vector<string>> anagrams(const string& input, const Dictionary& dict, unsigned max) {
    set<unsigned long> indexes;
    vector<vector<string>> anagrams;

    string str = cleanUp(input);
    sort(str.begin(), str.end());

    int maxi = (int) max;
    if (maxi == 0)
        maxi = -1;

    for (unsigned long i = 0; i < dict.size(); i++)
        indexes.insert(indexes.end(), i);

    build(str, dict, indexes, vector<string>(), anagrams, maxi);

    return anagrams;
}
