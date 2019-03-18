#include <fstream>
#include <algorithm>

#include "anagrams.hpp"

using namespace std;

/**
 * Remove from a string undesired characters such as spaces, figures, etc. and replace uppercases by corresponding lowercases.
 *
 * @param[in,out] str a string to clean up.
 */
static void cleanUp(string& str) {
    for (auto it = str.begin(); it != str.end();)
        if (islower(*it))
            ++it;
        else if (isupper(*it))
            *(it++) += 'a' - 'A';
        else
            it = str.erase(it);
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
static astring atransform(string str) {
    astring astr = {(short) str.length()};
    for (char c : str)
        ++astr[c - 'a' + 1];

    return astr;
}

/**
 * Search if an astring is a subset of another.
 *
 * @param[in] astr an astring.
 * @param[in] asub a supposed subset.
 * @return true if asub is a subset of astr, false otherwise.
 */
static bool isSub(const astring& astr, const astring& asub) {
    for (unsigned i = 0; i < 27; ++i)
        if (astr[i] < asub[i])
            return false;

    return true;
}

/**
 * Search if an astring is a subset of another and compute the residual if so.
 *
 * @param[in] astr an astring.
 * @param[in] asub a supposed subset.
 * @param[out] ares the residual.
 * @return true if asub is a subset of astr, false otherwise.
 */
static bool isRes(const astring& astr, const astring& asub, astring& ares) {
    if (astr[0] < asub[0])
        return false;

    ares = astr;
    for (unsigned i = 1; i < 27; ++i)
        if (asub[i] > 0 && (ares[i] -= asub[i]) < 0)
            return false;

    ares[0] -= asub[0];

    return true;
}

typedef Dictionary::const_iterator reference;

/**
 * Compute all anagrams of an astring, recursively.
 *
 * @param[in] astr the astring whose anagrams are searched.
 * @param[in] references a container of references to words in which it is still useful to search anagrams.
 * @param[in] wrds the container of previous words in the current anagram. In spite of manipulations, wrds returns to its initial state after execution.
 * @param[in,out] anagrams the container of already-computed anagrams.
 * @param[in] max the maximum number of words that are allowed to be added to current. If max is negative, there is no limit.
 */
static void build(const astring& astr, const vector<reference>& references, vector<string>& wrds, vector<vector<string>>& anagrams, int max) {
    if (max == 0)
        return;

    astring ares;
    vector<reference> newreferences;

    for (auto it = references.begin(); it != references.end(); ++it)
        if (isRes(astr, (**it).second, ares)) {
            newreferences.push_back(*it);

            wrds.push_back((**it).first);
            if (ares[0] > 0)
                build(ares, newreferences, wrds, anagrams, max - 1);
            else
                anagrams.push_back(wrds);
            wrds.pop_back();
        }
}

vector<vector<string>> anagrams(const string& input, const Dictionary& dict, unsigned max) {
    vector<reference> references;
    vector<vector<string>> anagrams;
    vector<string> wrds;

    string str = input;
    cleanUp(str);
    astring astr = atransform(str);

    for (auto it = dict.begin(); it != dict.end(); ++it)
        if (isSub(astr, (*it).second))
            references.push_back(it);
    reverse(references.begin(), references.end());

    build(astr, references, wrds, anagrams, max == 0 ? -1 : max);

    return anagrams;
}

Dictionary create_dictionary(const string& filename) {
    Dictionary dict;
    string wrd;
    ifstream file;

    file.open(filename);

    while (file >> wrd) {
        cleanUp(wrd);
        if (!wrd.empty())
            dict.push_back(pair<string, astring>(wrd, atransform(wrd)));
    }

    file.close();

    return dict;
}
