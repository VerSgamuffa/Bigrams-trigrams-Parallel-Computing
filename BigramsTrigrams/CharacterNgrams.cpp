#include "CharacterNgrams.h"
#include "Utils.h"
#include <chrono>
#include <omp.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <queue>
#include <sstream>

using namespace std;

void print_character_histogram(const int* histogram, int num_slots, int ngramLength) {
    priority_queue<pair<int, string>> q;

    for (int idx = 0; idx < num_slots; idx++) {
        if (histogram[idx] > 0) {
            string ngram = "";
            int val = idx;
            for (int i = 0; i < ngramLength; i++) {
                int c = val % 36;
                ngram = char('a' + c) + ngram;
                val /= 36;
            }
            q.push({histogram[idx], ngram});
        }
    }

    // stampa
    /*
    while (!q.empty()) {
        auto [count, ngram] = q.top();
        q.pop();
        cout << ngram << ": " << count << endl;
    }
    */
}

double CharacterNgrams::compute_character_ngrams(const std::string& filename) const {
    //auto start_total = chrono::high_resolution_clock::now();

    cout << "Computing sequential character ngrams (fair version)..." << endl;

    //auto start_file = chrono::high_resolution_clock::now();
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error opening file\n";
        return 1;
    }
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    //auto end_file = chrono::high_resolution_clock::now();


    //auto start_count = chrono::high_resolution_clock::now();
    stringstream ss(content);
    string word;
    int count = 0;

    while (ss >> word) {
        count++;
    }
    ss.clear();
    ss.seekg(0, ios::beg);
    //auto end_count = chrono::high_resolution_clock::now();

    //auto start_parse = chrono::high_resolution_clock::now();
    vector<string_view> words;
    words.reserve(count);

    while (ss >> word) {
        words.push_back(word);
    }
    //auto end_parse = chrono::high_resolution_clock::now();

    //global histogram
    //auto start_alloc = chrono::high_resolution_clock::now();
    int n = getNgramLength();
    auto num_slots = static_cast<size_t>(pow(36, n));
    vector<int> histogram(num_slots, 0);
    //auto end_alloc = chrono::high_resolution_clock::now();

    //main cycle
    auto start_compute = chrono::high_resolution_clock::now();
    for (auto w : words) {
        for (int p = 0; p <= static_cast<int>(w.size()) - n ; p++) {
            //bool ok = true;
            size_t index = 0;
            for (int i = 0; i < n; i++) {
                const char c = w.at(p + i);
                int offset;
                if (c >= '0' && c <= '9') {
                    offset = c - '0';
                } else if (c >= 'a' && c <= 'z') {
                    offset = c - 'a' + 10;
                } else {
                    goto skip_ngram;
                }
                index = index * 36 + offset;
            }
            histogram[index]++;
        }
        skip_ngram:
                ;
    }
    auto end_compute = chrono::high_resolution_clock::now();

    auto start_print = chrono::high_resolution_clock::now();
    print_character_histogram(histogram.data(), static_cast<int>(num_slots), n);
    auto end_print = chrono::high_resolution_clock::now();
    //auto end_total = chrono::high_resolution_clock::now();

    //to be removed/added as necessary so as not to alter fairness
    //save_character_histogram_to_csv(histogram, n, "character_ngrams.csv");

    double compute_time =
    chrono::duration<double>(end_compute - start_compute).count() +
    chrono::duration<double>(end_print - start_print).count();

    /* //print steps times
    cout << "=== Tempi sequenziali ===\n";
    cout << "Lettura file: " << chrono::duration<double>(end_file - start_file).count() << " s\n";
    cout << "Conteggio parole: " << chrono::duration<double>(end_count - start_count).count() << " s\n";
    cout << "Parsing parole: " << chrono::duration<double>(end_parse - start_parse).count() << " s\n";
    cout << "Allocazione memoria: " << chrono::duration<double>(end_alloc - start_alloc).count() << " s\n";
    cout << "Ciclo principale: " << chrono::duration<double>(end_compute - start_compute).count() << " s\n";
    cout << "Stampa output: " << chrono::duration<double>(end_print - start_print).count() << " s\n";
    cout << "Tempo totale funzione sequenziale: " << chrono::duration<double>(end_total - start_total).count() << " s\n";*/

    return compute_time;
}


double CharacterNgrams::parallel_compute_character_ngrams(const std::string& filename) {
    //auto start_total = chrono::high_resolution_clock::now();

    cout << "Computing OpenMP character ngrams (fair version)..." << endl;

    //auto start_file = chrono::high_resolution_clock::now();
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error opening file\n";
        return 1;
    }
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    //auto end_file = chrono::high_resolution_clock::now();

    //auto start_count = chrono::high_resolution_clock::now();
    stringstream ss(content);
    string word;
    int count = 0;
    while (ss >> word) {
        count++;
    }
    ss.clear();
    ss.seekg(0, ios::beg);
    //auto end_count = chrono::high_resolution_clock::now();

    //auto start_parse = chrono::high_resolution_clock::now();
    vector<string_view> words;
    words.reserve(count);
    while (ss >> word) {
        words.push_back(word);
    }
    //auto end_parse = chrono::high_resolution_clock::now();

    //histograms allocation
    //auto start_alloc = chrono::high_resolution_clock::now();
    int n = getNgramLength();
    auto num_slots = static_cast<size_t>(pow(36, n));

    vector<int> histogram(num_slots, 0);

    int threads = omp_get_max_threads();
    //cout << "Numero threads " << threads << endl;
    vector<vector<int>> local_histograms(
        threads, vector<int>(num_slots, 0)
    );
    //auto end_alloc = chrono::high_resolution_clock::now();

    //main cycle
    auto start_compute = chrono::high_resolution_clock::now();

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        auto& local_hist = local_histograms[tid];

        #pragma omp for schedule(static)
        for (size_t widx = 0; widx < words.size(); ++widx) {
            const auto& w = words[widx];

            for (int p = 0; p <= static_cast<int>(w.size()) - n; p++) {
                size_t index = 0;

                for (int i = 0; i < n; i++) {
                    const char c = w.at(p + i);
                    int offset;

                    if (c >= '0' && c <= '9') {
                        offset = c - '0';
                    } else if (c >= 'a' && c <= 'z') {
                        offset = c - 'a' + 10;
                    } else {
                        goto skip_ngram;
                    }

                    index = index * 36 + offset;
                }

                local_hist[index]++;

            skip_ngram:
                ;
            }
        }
    }

    for (int t = 0; t < threads; ++t) {
        for (size_t i = 0; i < num_slots; ++i) {
            histogram[i] += local_histograms[t][i];
        }
    }

    auto end_compute = chrono::high_resolution_clock::now();

    auto start_print = chrono::high_resolution_clock::now();
    print_character_histogram(histogram.data(), static_cast<int>(num_slots), n);
    auto end_print = chrono::high_resolution_clock::now();

    //auto end_total = chrono::high_resolution_clock::now();

    double compute_time =
    chrono::duration<double>(end_compute - start_compute).count() + chrono::duration<double>(end_print - start_print).count();

    /* //print steps times
    cout << "=== Tempi paralleli ===\n";
    cout << "Lettura file: " << chrono::duration<double>(end_file - start_file).count() << " s\n";
    cout << "Conteggio parole: " << chrono::duration<double>(end_count - start_count).count() << " s\n";
    cout << "Parsing parole: " << chrono::duration<double>(end_parse - start_parse).count() << " s\n";
    cout << "Allocazione memoria: " << chrono::duration<double>(end_alloc - start_alloc).count() << " s\n";
    cout << "Ciclo principale parallelo: " << chrono::duration<double>(end_compute - start_compute).count() << " s\n";
    cout << "Stampa output: " << chrono::duration<double>(end_print - start_print).count() << " s\n";
    cout << "Tempo totale funzione OpenMP: " << chrono::duration<double>(end_total - start_total).count() << " s\n";*/

    return compute_time;
}

int CharacterNgrams::getNgramLength() const {
    return NgramLength;
}
