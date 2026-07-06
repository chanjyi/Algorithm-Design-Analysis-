// *********************************************************
// Program: heap_sort_step.cpp
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
using namespace std;

typedef pair<unsigned long long, string> Record;

// Helper function to print the current state of the array
void printStep(ofstream& outfile, const vector<Record>& arr, string label) {
    outfile << "[";
    for (int i = 0; i < (int)arr.size(); i++) {
        if (i > 0) outfile << ", ";
        outfile << arr[i].first << "/" << arr[i].second;
    }
    outfile << "] " << label << "\n";
}

// Max-Heapify function customized for the step-by-step process
void heapifyStep(vector<Record>& arr, int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[left].first > arr[largest].first) largest = left;
    if (right < n && arr[right].first > arr[largest].first) largest = right;

    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapifyStep(arr, n, largest);
    }
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

    // Tutor will uncomment one of these pairs during the demo
    int startRow = 1;   int endRow = 7;
    // int startRow = 1; int endRow = 20;
    // int startRow = 5; int endRow = 15;

    ifstream infile(filename);
    vector<Record> arr;
    string line;
    int currentRow = 0;

    // Read only the specified rows
    while (getline(infile, line)) {
        currentRow++;
        if (currentRow < startRow) continue;
        if (currentRow > endRow) break;

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

    // Extract 'n' from the filename to name the output file correctly
    string baseName = filename;
    size_t slashPos = baseName.find_last_of("/\\");
    if (slashPos != string::npos) baseName = baseName.substr(slashPos + 1);

    size_t underPos = baseName.find_last_of("_");
    size_t dotPos = baseName.find_last_of(".");
    string n_str = baseName.substr(underPos + 1, dotPos - underPos - 1);

    string outFilename = "dataset_" + n_str + "_heap_sorted_step_"
                       + to_string(startRow) + "_" + to_string(endRow) + ".txt";
    ofstream outfile(outFilename);

    outfile << "(using maxheap)\n";

    int n = arr.size();

    // 1. Build max heap and print the 'initial' state
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapifyStep(arr, n, i);
    }
    printStep(outfile, arr, "initial");

    // 2. Extract elements one by one and print the 'i = X' state
    for (int i = n - 1; i > 0; i--) {
        swap(arr[0], arr[i]);
        heapifyStep(arr, i, 0);

        printStep(outfile, arr, "i = " + to_string(i));
    }

    outfile.close();
    cout << "Step output saved to: " << outFilename << endl;

    return 0;
}
