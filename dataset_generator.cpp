// *********************************************************
// Program: dataset_generator.cpp
// Course: CCP6214 Algorithm Design and Analysis
// Lecture Class: TC6L
// Tutorial Class: T13L
// Trimester: 2610
// Member_1: 242UC244SR | Chan Jia Yi | chan.jia.yi1@student.mmu.edu.my| +60 12-253 9359
// Member_2: 242UC244QN | Chew Jia Yi | chew.jia.yi@student.mmu.edu.my | +60 13-282 3398
// Member_3: 242UC244FZ | Koay Yee Shuen | koay.yee.shuen@student.mmu.edu.my| +60 11-5680 0192
// Member_4: 242UC244GL | Ong Wan Ning | ong.wan.ning@student.mmu.edu.my | +60 16-607 0825
// *********************************************************
// Task Distribution
// Member_1: Hash Table Search
// Member_2: Radix Sort
// Member_3: Dataset Generator
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

int main() {
    const string output_dir = "dataset";
    _mkdir(output_dir.c_str());

    // Array initialized with the 10 specific dataset sizes required for the experiment
	// update here for different n
    long long input_sizes[] = {
        1000, 5000, 10000, 50000, 100000,
        500000, 1000000, 5000000, 10000000, 50000000
    };
    int num_sizes = sizeof(input_sizes) / sizeof(input_sizes[0]);

    // Initialize the 64-bit Mersenne Twister engine with your specified seed number
	// seed set to chan's ID, 242UC244SR, U->1, C->3, S->9, R->8.
    unsigned long long seed = 2421324498ULL;
    mt19937_64 rng(seed);

    // Uniform distribution for 10-digit integers ranging from 1,000,000,000 to 9,999,999,999
    uniform_int_distribution<unsigned long long> dist_int(1000000000ULL, 9999999999ULL);

    // Loop through each dataset size defined in the array
    for (int i = 0; i < num_sizes; ++i) {
        long long n = input_sizes[i];

        // Track uniqueness per dataset size using an unordered_set to completely avoid duplicates
        unordered_set<unsigned long long> unique_integers;
        vector<pair<unsigned long long, string>> dataset_values;

        // Construct file path matching the target template (e.g., dataset/dataset_1000.csv)
        string filename = output_dir + "/dataset_" + to_string(n) + ".csv";
        ofstream outfile(filename);

        if (!outfile.is_open()) {
            cerr << "Error: Could not create or open file " << filename << endl;
            continue; // Move to the next dataset size if file generation fails
        }

        cout << "---------------------------------------------" << endl;
        cout << "Generating dataset for n = " << n << "..." << endl;

        long long count = 0;
        while (count < n) {
            unsigned long long random_val = dist_int(rng);

            // Verify if the 10-digit integer has already been picked
            if (unique_integers.find(random_val) == unique_integers.end()) {
                unique_integers.insert(random_val);
                
                string random_str = generate_random_string(rng);
                
                // Push both the integer and the string as a pair
                dataset_values.push_back({random_val, random_str});

                // Write formatted row directly into the CSV: integer,string
                outfile << random_val << "," << random_str << "\n";
                count++;

                // Print basic progress updates for tracking massive datasets smoothly
                if (count > 0 && count % 2000000 == 0) {
                    cout << "   -> Generated " << count << " elements..." << endl;
                }
            }
        }

        outfile.close();

        cout << "Successfully saved: " << filename << endl;
    }

    cout << "---------------------------------------------" << endl;
    cout << "All 10 datasets generated successfully." << endl;

    return 0;
}