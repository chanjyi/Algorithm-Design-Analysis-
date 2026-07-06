// *********************************************************
// Program: radix_sort_step.cpp
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

void printStep(ofstream& outfile, vector<Record>& arr, string label) {
    outfile << "[";
    for (int i = 0; i < (int)arr.size(); i++) {
        if (i > 0) outfile << ", ";
        outfile << arr[i].first << "/" << arr[i].second;
    }
    outfile << "] " << label << "\n";
}

int main(int argc, char* argv[]) {
    string filename; // This matches your original variable name

    if (argc < 2) {
        // Fallback: Ask the user to type it into the VS Code terminal pane
        cout << "Enter the path to the dataset CSV file: ";
        cin >> filename;
    } else {
        filename = argv[1];
    }

    // Tutor: uncomment one pair below
    int startRow = 1;   int endRow = 7;
    // int startRow = 1; int endRow = 20;
    // int startRow = 5; int endRow = 15;

    ifstream infile(filename);
    if (!infile.is_open()) {
        cerr << "Cannot open file: " << filename << endl;
        return 1;
    }

    vector<Record> arr;
    string line;
    int currentRow = 0;

    while (getline(infile, line)) {
        currentRow++;
        if (currentRow < startRow) continue;
        if (currentRow > endRow) break;

        stringstream ss(line);
        string intPart, strPart;
        getline(ss, intPart, ',');
        getline(ss, strPart, ',');

        unsigned long long num = stoull(intPart);
        arr.push_back({num, strPart});
    }
    infile.close();

    // Extract n from filename like "dataset/dataset_1000.csv"
    string baseName = filename;
    size_t slashPos = baseName.find_last_of("/\\");
    if (slashPos != string::npos) baseName = baseName.substr(slashPos + 1);
    // baseName is now "dataset_1000.csv"

    size_t underPos = baseName.find_last_of("_");
    size_t dotPos = baseName.find_last_of(".");
    string n_str = baseName.substr(underPos + 1, dotPos - underPos - 1);

    string outFilename = "dataset_" + n_str + "_radix_sorted_step_"
                       + to_string(startRow) + "_" + to_string(endRow) + ".txt";
    ofstream outfile(outFilename);

    // Print original state
    printStep(outfile, arr, "original");

    unsigned long long exp = 1;
    for (int d = 10; d >= 1; d--) {
        countingSort(arr, exp);
        printStep(outfile, arr, "d=" + to_string(d));
        exp *= 10;
    }

    outfile.close();
    cout << "Step output saved to: " << outFilename << endl;
    return 0;
}
