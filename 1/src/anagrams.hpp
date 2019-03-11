#ifndef ANAGRAMS
#define ANAGRAMS

#include <string>
#include <utility> /// std::pair
#include <vector>
#include <array>

typedef std::vector<std::pair<std::string, std::array<short, 27>>> Dictionary;

/**
 * Generate all anagrams of a string.
 *
 * @param input the string whose anagrams are searched.
 * @param dict the used dictionary.
 * @param max the maximum number of words that are allowed in anagrams. If max is null, there is no limit.
 * @return the container of generated anagrams.
 */
std::vector<std::vector<std::string>> anagrams(const std::string& input, const Dictionary& dict, unsigned max);

/**
 * Create a dictionary from a file.
 *
 * @param filename the path to the file.
 * @return the created dictionary. If non-existent or empty file, return an empty Dictionary.
 */
Dictionary create_dictionary(const std::string& filename);

#endif
