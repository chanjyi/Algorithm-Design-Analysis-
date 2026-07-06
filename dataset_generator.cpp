// *********************************************************
// Program: dataset_generator.cpp
// Course: CCP6214 Algorithm Design and Analysis
// Lecture Class: TC6L
// Tutorial Class: T21L
// Trimester: 2610
// Member_1: 242UC244SR | Chan Jia Yi | chan.jia.yi1@student.mmu.edu.my| +60 12-253 9359
// Member_2: 242UC244QN | Chew Jia Yi | chew.jia.yi@student.mmu.edu.my | +60 13-282 3398
// Member_3: 242UC244FZ | Koay Yee Shuen | koay.yee.shuen@student.mmu.edu.my| +60 11-5680 0192
// Member_4: 242UC244GL | Ong Wan Ning | ong.wan.ning@student.mmu.edu.my | +60 16-607 0825
// *********************************************************
// Task Distribution
// Member_1: Hash Table Search
// Member_2: Radix Sort
// Member_3: Dataset Generators
// Member_4: Heap Sort
// *********************************************************
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <unordered_set>
#include <direct.h>

using namespace std;

// Function to generate a random 5-letter lowercase string
string generate_random_string(mt19937_64& rng) {
    uniform_int_distribution<int> dist(0, 25);
    string result = "";
    for (int i = 0; i < 5; ++i) {
        result += (char)('a' + dist(rng));
    }
    return result;
}

// A simple structure to store the full pair for the search pool
struct DataRecord {
    unsigned long long id;
    string text;
};

int main() {
    const string output_dir = "dataset";
    _mkdir(output_dir.c_str());
    const string search_output_dir = "dataset_search";
    _mkdir(search_output_dir.c_str());

    long long input_sizes[] = {
        1000, 5000, 10000, 50000, 100000,
        500000, 1000000, 5000000, 10000000, 50000000
    };
    int num_sizes = sizeof(input_sizes) / sizeof(input_sizes[0]);

    unsigned long long seed = 2421324498ULL;
    mt19937_64 rng(seed);

    uniform_int_distribution<unsigned long long> dist_int(1000000000ULL, 9999999999ULL);

    for (int i = 0; i < num_sizes; ++i) {
        long long n = input_sizes[i];
        long long search_n = n / 5;
        long long found_target = search_n / 2;

        unordered_set<unsigned long long> unique_integers;
        
        // Memory-efficient pool to store complete pairs for the "found" search queries
        vector<DataRecord> search_found_pool;
        search_found_pool.reserve(found_target);

        string filename = output_dir + "/dataset_" + to_string(n) + ".csv";
        ofstream outfile(filename);

        string search_filename = search_output_dir + "/dataset_search_from_" + to_string(n) + ".csv";
        ofstream search_outfile(search_filename);

        if (!outfile.is_open() || !search_outfile.is_open()) {
            cerr << "Error: Could not open files for size " << n << endl;
            continue;
        }

        cout << "---------------------------------------------" << endl;
        cout << "Generating dataset for n = " << n << "..." << endl;

        long long count = 0;
        while (count < n) {
            unsigned long long random_val = dist_int(rng);

            if (unique_integers.find(random_val) == unique_integers.end()) {
                unique_integers.insert(random_val);
                
                string random_str = generate_random_string(rng);
                
                // Write directly to main dataset
                outfile << random_val << "," << random_str << "\n";

                // Save the exact pair into our search pool if we still need "found" targets
                if (count < found_target) {
                    search_found_pool.push_back({random_val, random_str});
                }
                
                count++;

                if (count % 5000000 == 0) {
                    cout << "   -> Generated " << count << " elements..." << endl;
                }
            }
        }
        outfile.close();

        cout << "Generating mixed search dataset with full pairs..." << endl;
        
        // 1. Write the existing valid pairs (Guaranteed to be found)
        for (long long j = 0; j < found_target; ++j) {
            search_outfile << search_found_pool[j].id << "," << search_found_pool[j].text << "\n";
        }

        // 2. Write completely new pairs (Guaranteed NOT to be found)
        long long search_count = found_target;
        while (search_count < search_n) {
            unsigned long long random_val = dist_int(rng);

            if (unique_integers.find(random_val) == unique_integers.end()) {
                string random_str = generate_random_string(rng);
                
                // Write the full pair so it matches format but won't match any key in the main dataset
                search_outfile << random_val << "," << random_str << "\n"; 
                search_count++;
            }
        }

        search_outfile.close();
        cout << "Successfully saved: " << filename << endl;
        cout << "Successfully saved: " << search_filename << endl;
    }

    cout << "---------------------------------------------" << endl;
    cout << "All datasets generated successfully." << endl;
    return 0;
}