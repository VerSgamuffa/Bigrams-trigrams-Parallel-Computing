#include "Utils.h"
#include <fstream>
#include <iostream>
#include <vector>

void save_histogram_to_csv(const std::unordered_map<std::string,int>& histogram,
                           const std::string& filename)
{
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Errore apertura file CSV: " << filename << std::endl;
        return;
    }

    out << "ngram,count\n";
    for (auto& [ngram, count] : histogram) {
        out << ngram << "," << count << "\n";
    }
}

void save_character_histogram_to_csv(const std::vector<int>& histogram, int ngramLength, const std::string& filename){

    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Errore apertura file CSV: " << filename << std::endl;
        return;
    }

    out << "ngram,count\n";

    for (size_t idx = 0; idx < histogram.size(); ++idx) {
        int count = histogram[idx];
        if (count == 0) continue;

        // Ricostruzione n-gram da indice base-36
        std::string ngram(ngramLength, ' ');
        size_t value = idx;

        for (int i = ngramLength - 1; i >= 0; --i) {
            int digit = value % 36;
            value /= 36;

            if (digit < 10)
                ngram[i] = '0' + digit;
            else
                ngram[i] = 'a' + (digit - 10);
        }

        out << ngram << "," << count << "\n";
    }

    out.close();
}

