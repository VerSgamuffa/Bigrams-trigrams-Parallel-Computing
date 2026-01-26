#ifndef UTILS_H
#define UTILS_H

#include <unordered_map>
#include <string>
#include <vector>

void save_histogram_to_csv(const std::unordered_map<std::string, int>& histogram, const std::string& filename);

void save_character_histogram_to_csv(const std::vector<int>& histogram, int ngramLength, const std::string& filename);

#endif
