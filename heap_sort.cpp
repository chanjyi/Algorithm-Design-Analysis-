// *********************************************************
// Program: heap_sort.cpp
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

// Max-Heapify function
void heapify(vector<Record>& arr, int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[left].first > arr[largest].first)
        largest = left;

    if (right < n && arr[right].first > arr[largest].first)
        largest = right;

    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

// Main Heap Sort logic
void heapSort(vector<Record>& arr) {
    int n = arr.size();

    // Build max heap
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // Extract elements one by one
    for (int i = n - 1; i > 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

// Read CSV without timing it
vector<Record> readCSV(const string& filename) {
    vector<Record> arr;
    ifstream infile(filename);
    string line;
    while (getline(infile, line)) {
        stringstream ss(line);
        string intPart, strPart;
        getline(ss, intPart, ',');
        getline(ss, strPart, ',');
        // Guard against CRLF-terminated input files (e.g. CSV generated on Windows)
        while (!strPart.empty() && (strPart.back() == '\r' || strPart.back() == '\n'))
            strPart.pop_back();
        arr.push_back({stoull(intPart), strPart});
    }
    infile.close();
    return arr;
}

int main(int argc, char* argv[]) {
    vector<string> filenames;

    if (argc < 2) {
        // Fallback: Ask the user to type it into the VS Code terminal pane
        // (lets this run with the VS Code "Run" button, no command-line args needed)
        string datasetFilename;
        cout << "Enter the path to the dataset CSV file: ";
        cin >> datasetFilename;
        filenames.push_back(datasetFilename);
    } else {
        // Still works the old way too: heap_sort dataset_1000.csv dataset_5000.csv ...
        for (int f = 1; f < argc; f++)
            filenames.push_back(argv[f]);
    }

    for (const string& filename : filenames) {
        cout << "Processing: " << filename << endl;

        vector<Record> arr = readCSV(filename);
        int n = arr.size();

        // start timer
        auto start = chrono::high_resolution_clock::now();
        heapSort(arr);
        auto end = chrono::high_resolution_clock::now();
        // stop timer

        double elapsed = chrono::duration<double>(end - start).count();
        cout << "n = " << n << " | Running time: " << elapsed << " seconds" << endl;

        // Build output filename according to rubric
        string outFilename = "heap_sort_dataset_" + to_string(n) + ".csv";
        ofstream outfile(outFilename);
        for (auto& rec : arr)
            outfile << rec.first << "/" << rec.second << "\n";

        outfile << "\nRunning time: " << elapsed << " seconds\n";
        outfile.close();

        cout << "Sorted output saved to: " << outFilename << "\n" << endl;
    }
    return 0;
}