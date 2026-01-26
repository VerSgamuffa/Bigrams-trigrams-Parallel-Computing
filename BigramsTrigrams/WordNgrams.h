#ifndef BIGRAMSTRIGRAMS_WORDNGRAMS_H
#define BIGRAMSTRIGRAMS_WORDNGRAMS_H

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

class WordNgrams {
public:
    WordNgrams(int n) {
        NgramLength = n;
    };

    double compute_word_ngrams(const string &filename);

    double parallel_compute_word_ngrams(const string &filename);

    int getNgramLength() const;

private:
    int NgramLength;



};


#endif //BIGRAMSTRIGRAMS_WORDNGRAMS_H
