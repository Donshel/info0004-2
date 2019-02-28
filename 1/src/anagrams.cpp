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
 * @note both strings are supposed to be alphabetically sorted.
 *
 * @param str1, the string where are removed characters from.
 * @param str2, the other string.
 * @return the residual string, or "1" if str2 has character(s) that str1 hasn't.
 */
string removeFrom(string str1, string str2) {
    auto pt = str1.begin();

    for (auto it = str2.begin(); it != str2.end(); it++) {
        pt = find(pt, str1.end(), *it);

        if (pt == str1.end())
            return "1";

        str1.erase(pt);
    }

    return str1;
}

/**
 * Create a dictionary from a file.
 *
 * @param filename, the path to the file.
 * @return the created dictionary. If no or empty file, return an empty Dictionary.
 */
Dictionary create_dictionary(const string& filename) {
    Dictionary dict;
    string word;
    ifstream file;

    file.open(filename);

    while (file >> word) {
        word = cleanUp(word);
        if (!word.empty()) {
            string temp = word;
            sort(temp.begin(), temp.end());
            dict.push_back(pair<string, string>(word, temp));
        }
    }

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

    vector<string> residuals;

    /// Search fitting words
    for (auto it = indexes.begin(); it != indexes.end();) {
        string residual = removeFrom(str, dict[*it].second);

        if (residual != "1") {
            residuals.push_back(residual);
            it++;
        } else
            it = indexes.erase(it);
    }

    /// Start again recursively for each fitting word found
    for (auto it = residuals.begin(); it != residuals.end(); it++) {
        current.push_back(dict[*indexes.begin()].first);

        if (!(*it).empty())
            build(*it, dict, indexes, current, anagrams, max - 1);
        else
            anagrams.push_back(current);

        current.pop_back();
        indexes.erase(indexes.begin());
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
