#ifndef ANAGRAMS
#define ANAGRAMS

#include <string>
#include <utility> /// std::pair
#include <vector>
#include <array>

typedef std::vector<std::pair<std::string, std::array<short, 27>>> Dictionary;

Dictionary create_dictionary(const std::string&);
std::vector<std::vector<std::string>> anagrams(const std::string&, const Dictionary&, unsigned);

#endif
