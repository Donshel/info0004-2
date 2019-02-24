#ifndef ANAGRAMS
#define ANAGRAMS

#include <string>
#include <vector>

typedef std::vector<std::string> Dictionary;

/**
 * Create a dictionary from a file.
 *
 * @note The words in the file have to be lowercase.
 *
 * @param filename, the path to the file.
 * @return the created dictionary.
 */
Dictionary create_dictionary(const std::string&);

/**
 * Generate all anagrams of a string.
 *
 * @param input, the string whose anagrams are searched.
 * @param dict, the used dictionary.
 * @param max, the maximum number of words that are allowed in anagrams. If max is null, there is no limit.
 * @return the container of generated anagrams.
 */
std::vector<std::vector<std::string>> anagrams(const std::string&, const Dictionary&, unsigned);

#endif
