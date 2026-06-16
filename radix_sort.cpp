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
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
using namespace std;

typedef pair<unsigned long long, string> Record;

void countingSort(vector<Record>& arr, unsigned long long exp) {
    int n = arr.size();
    vector<Record> output(n);
    int count[10] = {0};

    // Count how many numbers have each digit at this position
    for (int i = 0; i < n; i++)
        count[(arr[i].first / exp) % 10]++;

    // Convert counts to positions
    for (int i = 1; i < 10; i++)
        count[i] += count[i - 1];

    // Fill output right to left (this keeps it stable)
    for (int i = n - 1; i >= 0; i--) {
        int digit = (arr[i].first / exp) % 10;
        output[count[digit] - 1] = arr[i];
        count[digit]--;
    }

    arr = output;
}

void radixSort(vector<Record>& arr) {
    unsigned long long exp = 1;
    for (int d = 10; d >= 1; d--) {
        countingSort(arr, exp);
        exp *= 10;
    }
}

vector<Record> readCSV(const string& filename) {
    vector<Record> arr;
    ifstream infile(filename);
    string line;
    while (getline(infile, line)) {
        stringstream ss(line);
        string intPart, strPart;
        getline(ss, intPart, ',');
        getline(ss, strPart, ',');
        arr.push_back({stoull(intPart), strPart});
    }
    infile.close();
    return arr;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: radix_sort dataset_1000.csv dataset_5000.csv ..." << endl;
        return 1;
    }

    for (int f = 1; f < argc; f++) {
        string filename = argv[f];
        cout << "Processing: " << filename << endl;

        // --- I/O NOT timed ---
        vector<Record> arr = readCSV(filename);
        int n = arr.size();

        // --- START TIMER ---
        auto start = chrono::high_resolution_clock::now();
        radixSort(arr);
        auto end = chrono::high_resolution_clock::now();
        // --- STOP TIMER ---

        double elapsed = chrono::duration<double>(end - start).count();
        cout << "n = " << n << " | Running time: " << elapsed << " seconds" << endl;

        // Build output filename
        string outFilename = "radix_sorted_dataset_" + to_string(n) + ".csv";
        ofstream outfile(outFilename);
        for (auto& rec : arr)
            outfile << rec.first << "/" << rec.second << "\n";
        outfile.close();

        cout << "Sorted output saved to: " << outFilename << "\n" << endl;
    }

    return 0;
}
