#include <fstream>
#include <set>

#include "anagrams.hpp"

using namespace std;

/**
 * Remove, from a string, undesired characters such as spaces, figures, etc. and replace uppercases by corresponding lowercases.
 *
 * @param str a string to clean up.
 * @return the cleaned up string.
 */
string cleanUp(string str) {
    for (auto it = str.begin(); it != str.end();)
        if (*it >= 'a' && *it <= 'z')
            it++;
        else if (*it >= 'A' && *it <= 'Z')
            *(it++) += 32;
        else
            it = str.erase(it);

    return str;
}

typedef std::array<short, 27> astring;

/**
 * Transform a string into an astring.
 *
 * @note the string should have a to z characters only.
 *
 * @param str a string.
 * @return the transformed string.
 */
astring atransform(string str) {
    astring astr = {(short) str.length()};
    for (char c : str)
        astr[c - 'a' + 1]++;

    return astr;
}

/**
 * Search if an astring is a subset of another.
 *
 * @param[in] astr an astring.
 * @param[in] asub a supposed subset.
 * @param[out] ares the residual.
 * @return true if asub is a subset of astr, false otherwise.
 */
bool isSub(const astring& astr, const astring& asub, astring& ares) {
    ares = astr;
    for (unsigned i = 0; i < 27; i++)
        if (asub[i] > 0 && (ares[i] -= asub[i]) < 0)
            return false;

    return true;
}

/**
 * Create a dictionary from a file.
 *
 * @param filename the path to the file.
 * @return the created dictionary. If non-existent or empty file, return an empty Dictionary.
 */
Dictionary create_dictionary(const string& filename) {
    Dictionary dict;
    string wrd;
    ifstream file;

    file.open(filename);

    while (file >> wrd) {
        wrd = cleanUp(wrd);
        if (!wrd.empty())
            dict.push_back(pair<string, astring>(wrd, atransform(wrd)));
    }

    file.close();

    return dict;
}

/**
 * Compute all anagrams of an astring, recursively.
 *
 * @param[in] astr the astring whose anagrams are searched.
 * @param[in] dict the dictionary.
 * @param[in] indexes a container of indexes of the dictionary in which it is still useful to search anagrams.
 * @param[in] wrds the container of previous words in the current anagram. In spite of manipulations, wrds returns to its initial state after execution.
 * @param[in,out] anagrams the container of already-computed anagrams.
 * @param[in] max the maximum number of words that are allowed to be added to current. If max is negative, there is no limit.
 */
void build(const astring& astr, const Dictionary& dict, const set<unsigned long>& indexes, vector<string>& wrds, vector<vector<string>>& anagrams, int max) {
    if (max == 0)
        return;

    astring ares;
    set<unsigned long> findexes;

    for (auto it = indexes.rbegin(); it != indexes.rend(); it++)
        if (isSub(astr, dict[*it].second, ares)) {
            findexes.insert(findexes.begin(), *it);
            wrds.push_back(dict[*it].first);

            if (ares[0] > 0)
                build(ares, dict, findexes, wrds, anagrams, max - 1);
            else
                anagrams.push_back(wrds);

            wrds.pop_back();
        }
}

/**
 * Generate all anagrams of a string.
 *
 * @param input the string whose anagrams are searched.
 * @param dict the used dictionary.
 * @param max the maximum number of words that are allowed in anagrams. If max is null, there is no limit.
 * @return the container of generated anagrams.
 */
vector<vector<string>> anagrams(const string& input, const Dictionary& dict, unsigned max) {
    set<unsigned long> indexes;
    vector<vector<string>> anagrams;
    vector<string> wrds;

    astring astr = atransform(cleanUp(input));

    int maxi = max;
    if (maxi == 0)
        maxi = -1;

    for (unsigned long i = 0; i < dict.size(); i++)
        indexes.insert(indexes.end(), i);

    build(astr, dict, indexes, wrds, anagrams, maxi);

    return anagrams;
}
