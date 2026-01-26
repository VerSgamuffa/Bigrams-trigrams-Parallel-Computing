#include "WordNgrams.h"
#include "Utils.h"
#include <omp.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <queue>

void print_histogram(unordered_map<string, int> histogram) {
    string ngram;
    //int count;

    //sort the histogram
    priority_queue<pair<int, string>> q;
    for (auto& [ngram, count]: histogram) {
        q.push({count, ngram});
    }

    /*while (!q.empty()) {
        auto [count, ngram] = q.top();
        q.pop();
        cout << ngram << ": " << count << endl;
    }*/

}


double WordNgrams::compute_word_ngrams(const string& filename) {
    //auto start_total = chrono::high_resolution_clock::now();
    cout << "Computing sequential word ngrams..." << endl;

    //auto start_file = chrono::high_resolution_clock::now();
    ifstream file(filename);
    if (!file) {
        cout << "Error opening input txt file" << endl;
        return 1;
    }

    vector<string> words;
    string word;
    while (file >> word) {
        words.push_back(word);
    }
    file.close();


    //auto end_file = chrono::high_resolution_clock::now();

    int n = this->NgramLength;
    unordered_map<string, int> histogram;

    auto start_compute = omp_get_wtime();
    for (int i = 0; i <= static_cast<int>(words.size()) - n; i++) {
        string ngram = words[i];
        for (int j = 1; j < n; j++) {
            ngram.push_back(' ');
            ngram += words[i + j];
        }
        histogram[ngram]++;
    }
    auto end_compute = omp_get_wtime();

    auto start_print = chrono::high_resolution_clock::now();
    print_histogram(histogram);
    //save_histogram_to_csv(histogram, "word_ngrams.csv");
    auto end_print = chrono::high_resolution_clock::now();

    //auto end_total = chrono::high_resolution_clock::now();

    double compute_time = end_compute - start_compute;

    double print_time = chrono::duration<double>(end_print - start_print).count();


    /*cout << "=== Sequential times ===\n";
    cout << "File read:        "
         << chrono::duration<double>(end_file - start_file).count() << " s\n";
    cout << "Histogram build:  "
         << chrono::duration<double>(end_compute - start_compute).count() << " s\n";
    cout << "Print + CSV:      "
         << chrono::duration<double>(end_print - start_print).count() << " s\n";
    cout << "TOTAL:            "
         << chrono::duration<double>(end_total - start_total).count() << " s\n";*/

    return compute_time + print_time;

}


double WordNgrams::parallel_compute_word_ngrams(const string& filename) {
    //auto start_total = chrono::high_resolution_clock::now();
    cout << "Computing parallel word ngrams..." << endl;

    //auto start_file = chrono::high_resolution_clock::now();
    ifstream file(filename);
    if (!file) {
        cout << "Error opening input txt file" << endl;
        return 1;
    }

    vector<string> words;
    string word;
    while (file >> word) {
        words.push_back(word);
    }
    file.close();
    //auto end_file = chrono::high_resolution_clock::now();

    int n = this->getNgramLength();
    int threads = omp_get_max_threads();
    //cout << "Numero threads " << threads << endl;

    vector<unordered_map<string, int>> local_hist(threads);
    for (auto &h : local_hist)
        h.reserve(words.size() / threads);

    auto start_compute = omp_get_wtime();
#pragma omp parallel
    {
        int tid = omp_get_thread_num();
        unordered_map<string, int>& hist = local_hist[tid];

#pragma omp for schedule(static)
        for (int i = 0; i <= (int)words.size() - n; i++) {
            string ngram = words[i];
            for (int j = 1; j < n; j++) {
                ngram.push_back(' ');
                ngram += words[i + j];
            }
            hist[ngram]++;
        }
    }
    auto end_compute = omp_get_wtime();

    auto start_reduce = chrono::high_resolution_clock::now();
    unordered_map<string, int> global_hist;
    global_hist.reserve(words.size());

    for (auto &hist : local_hist) {
        for (auto &[k, v] : hist) {
            global_hist[k] += v;
        }
    }
    auto end_reduce = chrono::high_resolution_clock::now();

    auto start_print = chrono::high_resolution_clock::now();
    print_histogram(global_hist);
    auto end_print = chrono::high_resolution_clock::now();

    //auto end_total = chrono::high_resolution_clock::now();

    double compute_time = end_compute - start_compute;
    double print_time = chrono::duration<double>(end_print - start_print).count();
    double reduce_time = chrono::duration<double>(end_reduce - start_reduce).count();


    /*cout << "=== Parallel times ===\n";
    cout << "File read:        "
         << chrono::duration<double>(end_file - start_file).count() << " s\n";
    cout << "Parallel compute: "
         << (end_compute - start_compute) << " s\n";
    cout << "Reduction:        "
         << chrono::duration<double>(end_reduce - start_reduce).count() << " s\n";
    cout << "Print + CSV:      "
         << chrono::duration<double>(end_print - start_print).count() << " s\n";
    cout << "TOTAL:            "
         << chrono::duration<double>(end_total - start_total).count() << " s\n";*/

    return compute_time + print_time + reduce_time;

}


int WordNgrams::getNgramLength() const {
    return NgramLength;
}
