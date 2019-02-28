#include <iostream>
#include <fstream>

#include <algorithm> /// std::sort

#include <set>

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
 * Search if a string is a subset of another.
 *
 * @note both strings are supposed to be alphabetically sorted.
 *
 * @param[in] str, a string.
 * @param[in] sub, a supposed subset.
 * @param[out] res, the residual.
 * @return true if sub is a subset of str, false otherwise.
 */
bool isSub(const string& str, const string& sub, string& res) {
    if (str.length() < sub.length())
        return false;

    res = str;
    auto it1 = sub.begin();
    auto it2 = res.begin();

    while (it1 != sub.end())
        if (it2 == res.end() || *it1 < *it2)
            return false;
        else if (*it1 > *it2)
            it2++;
        else {
            it2 = res.erase(it2);
            it1++;
        }

    return true;
}

/**
 * Create a dictionary from a file.
 *
 * @param filename, the path to the file.
 * @return the created dictionary. If no or empty file, return an empty Dictionary.
 */
Dictionary create_dictionary(const string& filename) {
    Dictionary dict;
    string wrd, tmp;
    ifstream file;

    file.open(filename);

    while (file >> wrd) {
        wrd = cleanUp(wrd);
        if (!wrd.empty()) {
            tmp = wrd;
            sort(tmp.begin(), tmp.end());
            dict.push_back(pair<string, string>(wrd, tmp));
        }
    }

    file.close();

    return dict;
}

/**
 * Compute all anagrams of a string, recursively.
 *
 * @param[in] str, the string whose anagrams are searched.
 * @param[in] dict, the dictionary.
 * @param[in] indexes, a container of indexes of the dictionary in which it is still useful to search anagrams.
 * @param[in] wrds, the container of previous words in the current anagram.
 * @param[in, out] anagrams, the container of already-computed anagrams.
 * @param[in] max, the maximum number of words that are allowed to be added to current. If max is negative, there is no limit.
 */
void build(string str, const Dictionary& dict, const set<unsigned long>& indexes, vector<string>& wrds, vector<vector<string>>& anagrams, int max) {
    if (max == 0)
        return;

    string res;
    set<unsigned long> findexes;

    for (auto it = indexes.rbegin(); it != indexes.rend(); it++)
        if (isSub(str, dict[*it].second, res)) {
            findexes.insert(findexes.begin(), *it);
            wrds.push_back(dict[*it].first);

            if (!res.empty())
                build(res, dict, findexes, wrds, anagrams, max - 1);
            else
                anagrams.push_back(wrds);

            wrds.pop_back();
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
    vector<string> wrds;

    string str = cleanUp(input);
    sort(str.begin(), str.end());

    int maxi = (int) max;
    if (maxi == 0)
        maxi = -1;

    for (unsigned long i = 0; i < dict.size(); i++)
        indexes.insert(indexes.end(), i);

    build(str, dict, indexes, wrds, anagrams, maxi);

    return anagrams;
}
