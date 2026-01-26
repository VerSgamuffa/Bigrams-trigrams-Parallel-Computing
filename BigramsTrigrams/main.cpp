#include <iostream>
#include <string>
#include <cctype>
#include "WordNgrams.h"
#include "CharacterNgrams.h"
#include <fstream>
#include <omp.h>


int clean_all() {
    ifstream input("../input.txt", ios::binary);
    if (!input.is_open()) {
        cout << "Error opening input.txt\n";
        return 1;
    }

    ofstream output("../clean_ascii.txt", ios::binary);
    if (!output.is_open()) {
        cout << "Error opening clean_ascii.txt\n";
        return 1;
    }

    bool last_was_space = true;  //avoid initial space
    char c;

    while (input.get(c)) {
        auto uc = static_cast<unsigned char>(c);

        //only ASCII characters
        if (uc > 127) continue;

        //converts punctuation to space
        if (ispunct(uc)) {
            if (!last_was_space) {
                output << ' ';
                last_was_space = true;
            }
            continue;
        }

        //remove double spaces
        if (isspace(uc)) {
            if (!last_was_space) {
                output << ' ';
                last_was_space = true;
            }
            continue;
        }

        //all lowercase letters
        if (isalpha(uc)) {
            output << static_cast<char>(tolower(uc));
            last_was_space = false;
        }
        //number
        else if (isdigit(uc)) {
            output << c;
            last_was_space = false;
        }
    }

    input.close();
    output.close();
    return 0;
}


void benchmark_scaling(CharacterNgrams& c,
                       WordNgrams& w,
                       const string& filename,
                       int iterations) {

    ofstream out("scaling_results.csv");
    out << "threads,char_seq,char_par,word_seq,word_par\n";

    for (int threads = 1; threads <= 32; threads++) {
        omp_set_num_threads(threads);

        double char_seq = 0, char_par = 0;
        double word_seq = 0, word_par = 0;

        //sequential character
        for (int i = 0; i < iterations; i++) {
            char_seq += c.compute_character_ngrams(filename);
        }

        //parallel character
        for (int i = 0; i < iterations; i++) {
            char_par += c.parallel_compute_character_ngrams(filename);
        }

        //sequential word
        for (int i = 0; i < iterations; i++) {
            word_seq += w.compute_word_ngrams(filename);
        }

        //parallel word
        for (int i = 0; i < iterations; i++) {
            word_par += w.parallel_compute_word_ngrams(filename);
        }

        //averages
        char_seq /= iterations;
        char_par /= iterations;
        word_seq /= iterations;
        word_par /= iterations;

        out << threads << "," << char_seq << "," << char_par << "," << word_seq << "," << word_par << "\n";
        cout << "Threads " << threads << " done\n";
    }

    out.close();
    cout << "Benchmark CSV saved: scaling_results.csv\n";
}


int main() {
    clean_all();
    CharacterNgrams c(2);
    WordNgrams w(2);

    benchmark_scaling(c, w, "../clean_ascii.txt", 10);

    return 0;

}
