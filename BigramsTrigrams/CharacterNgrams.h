#ifndef BIGRAMSTRIGRAMS_CHARACTERNGRAMS_H
#define BIGRAMSTRIGRAMS_CHARACTERNGRAMS_H

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

class CharacterNgrams {
public:
    CharacterNgrams(int n) {
        NgramLength = n;
    };

    double compute_character_ngrams(const string &filename) const;

    double parallel_compute_character_ngrams(const string &filename);

    int getNgramLength() const;

private:
    int NgramLength;

};


#endif //BIGRAMSTRIGRAMS_CHARACTERNGRAMS_H
