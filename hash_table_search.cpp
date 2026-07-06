// *********************************************************
// Program: hash_table_search.cpp
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
#include <algorithm>
using namespace std;

struct Record {
    unsigned long long key;
    string value;
};

struct SearchResult {
    bool found;
    string foundValue;
    int comparisons;
    int hashIndex;
};

struct AVLNode {
    Record data;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(const Record& r) {
        data = r;
        left = nullptr;
        right = nullptr;
        height = 1;
    }
};

int getHeight(AVLNode* node) {
    return node == nullptr ? 0 : node->height;
}

int getBalance(AVLNode* node) {
    return node == nullptr ? 0 : getHeight(node->left) - getHeight(node->right);
}

AVLNode* rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

    return x;
}

AVLNode* rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

    return y;
}

AVLNode* insertAVL(AVLNode* node, const Record& r) {
    if (node == nullptr) return new AVLNode(r);

    if (r.key < node->data.key)
        node->left = insertAVL(node->left, r);
    else if (r.key > node->data.key)
        node->right = insertAVL(node->right, r);
    else
        return node; // Ignore duplicate keys. Dataset should already be unique.

    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    int balance = getBalance(node);

    if (balance > 1 && r.key < node->left->data.key)
        return rotateRight(node);

    if (balance < -1 && r.key > node->right->data.key)
        return rotateLeft(node);

    if (balance > 1 && r.key > node->left->data.key) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    if (balance < -1 && r.key < node->right->data.key) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

void deleteAVL(AVLNode* node) {
    if (node == nullptr) return;
    deleteAVL(node->left);
    deleteAVL(node->right);
    delete node;
}

class HashTable {
private:
    vector<AVLNode*> table;
    int tableSize;

public:
    HashTable(int size) {
        tableSize = size;
        table.assign(tableSize, nullptr);
    }

    ~HashTable() {
        for (AVLNode* root : table)
            deleteAVL(root);
    }

    int hashFunction(unsigned long long key) const {
        return key % tableSize;
    }

    int getTableSize() const {
        return tableSize;
    }

    void insert(const Record& r) {
        int index = hashFunction(r.key);
        table[index] = insertAVL(table[index], r);
    }

    SearchResult searchDetailed(unsigned long long target) const {
        SearchResult result;
        result.found = false;
        result.foundValue = "-";
        result.comparisons = 0;
        result.hashIndex = hashFunction(target);

        AVLNode* current = table[result.hashIndex];

        while (current != nullptr) {
            result.comparisons++;

            if (target == current->data.key) {
                result.found = true;
                result.foundValue = current->data.value;
                return result;
            }
            else if (target < current->data.key) {
                current = current->left;
            }
            else {
                current = current->right;
            }
        }

        return result;
    }

    bool search(unsigned long long target) const {
        return searchDetailed(target).found;
    }
};

vector<Record> readDatasetCSV(const string& filename) {
    vector<Record> records;
    ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        cerr << "Cannot open dataset file: " << filename << endl;
        return records;
    }

    string line;
    while (getline(inputFile, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string keyText, valueText;

        getline(ss, keyText, ',');
        getline(ss, valueText, ',');

        if (!valueText.empty() && valueText.back() == '\r') {
            valueText.pop_back();
        }

        if (!keyText.empty() && !valueText.empty()) {
            records.push_back({stoull(keyText), valueText});
        }
    }

    inputFile.close();
    return records;
}

vector<unsigned long long> readQueryCSV(const string& filename) {
    vector<unsigned long long> queries;
    ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        cerr << "Cannot open query/search file: " << filename << endl;
        return queries;
    }

    string line;
    while (getline(inputFile, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string keyText;

        getline(ss, keyText, ',');

        if (!keyText.empty()) {
            queries.push_back(stoull(keyText));
        }
    }

    inputFile.close();
    return queries;
}

string extractDatasetSize(string filename) {
    size_t slashPosition = filename.find_last_of("/\\");
    if (slashPosition != string::npos) {
        filename = filename.substr(slashPosition + 1);
    }

    size_t underscorePosition = filename.find_last_of('_');
    size_t dotPosition = filename.find_last_of('.');

    if (underscorePosition == string::npos || dotPosition == string::npos || dotPosition <= underscorePosition) {
        return "unknown";
    }

    return filename.substr(underscorePosition + 1, dotPosition - underscorePosition - 1);
}

string deriveQueryPath(const string& datasetPath) {
    string n_str = extractDatasetSize(datasetPath);
    return "dataset_search/dataset_search_from_" + n_str + ".csv";
}

string deriveSearchResultPath(const string& datasetPath) {
    string n_str = extractDatasetSize(datasetPath);
    return "search_results_" + n_str + ".csv";
}

string deriveTimingOutputPath(const string& datasetPath) {
    string n_str = extractDatasetSize(datasetPath);
    return "hash_table_search_dataset_" + n_str + ".txt";
}

void writeSearchResultsCSV(const string& outputPath,
                           const string& datasetPath,
                           const string& queryPath,
                           const HashTable& hashTable,
                           const vector<unsigned long long>& queries) {
    ofstream outputFile(outputPath);

    if (!outputFile.is_open()) {
        cerr << "Cannot create search result file: " << outputPath << endl;
        return;
    }

    outputFile << "dataset_file," << datasetPath << "\n";
    outputFile << "query_file," << queryPath << "\n";
    outputFile << "\n";
    outputFile << "row_number,query_key,hash_index,status,matched_record,comparisons\n";

    for (int i = 0; i < (int)queries.size(); i++) {
        unsigned long long target = queries[i];
        SearchResult result = hashTable.searchDetailed(target);

        outputFile << (i + 1) << ","
                   << target << ","
                   << result.hashIndex << ","
                   << (result.found ? "FOUND" : "NOT_FOUND") << ",";

        if (result.found) {
            outputFile << target << "/" << result.foundValue;
        }
        else {
            outputFile << "-";
        }

        outputFile << "," << result.comparisons << "\n";
    }

    outputFile.close();
}

void warmUpSearchCase(const HashTable& hashTable,
                      const vector<unsigned long long>& targets,
                      int warmUpCount) {
    volatile bool searchGuard = false;

    for (int i = 0; i < warmUpCount; i++) {
        unsigned long long target = targets[i % targets.size()];
        searchGuard = hashTable.search(target);
    }

    if (searchGuard) {
        cout << "";
    }
}

double timeSearchCase(const HashTable& hashTable,
                      const vector<unsigned long long>& targets,
                      int searchesToPerform) {
    volatile bool searchGuard = false;

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < searchesToPerform; i++) {
        unsigned long long target = targets[i % targets.size()];
        searchGuard = hashTable.search(target);
    }

    auto end = chrono::high_resolution_clock::now();

    if (searchGuard) {
        cout << "";
    }

    return chrono::duration<double>(end - start).count();
}

int countFoundTargets(const HashTable& hashTable, const vector<unsigned long long>& targets) {
    int count = 0;

    for (unsigned long long target : targets) {
        if (hashTable.search(target)) {
            count++;
        }
    }

    return count;
}

void runHashTableSearch(const string& datasetPath,
                        const string& queryPath,
                        const string& searchResultPath) {
    cout << "Processing dataset: " << datasetPath << endl;
    cout << "Using query file: " << queryPath << endl;
    cout << "Search result output: " << searchResultPath << endl;

    // Reading input is not timed.
    vector<Record> records = readDatasetCSV(datasetPath);
    vector<unsigned long long> queries = readQueryCSV(queryPath);

    int n = records.size();
    int q = queries.size();

    if (n == 0) {
        cerr << "Dataset is empty or cannot be read.\n" << endl;
        return;
    }

    if (q == 0) {
        cerr << "Query file is empty or cannot be read.\n" << endl;
        return;
    }

    // Build hash table from main dataset.
    int tableSize = (n / 2 > 0) ? n / 2 : 1;
    HashTable hashTable(tableSize);

    for (const Record& record : records) {
        hashTable.insert(record);
    }

    // Compare ALL targets in dataset_search against the hash table.
    writeSearchResultsCSV(searchResultPath, datasetPath, queryPath, hashTable, queries);

    // Tutor-required case setup:
    // Best case    = search through first half of dataset_search, which should be FOUND targets.
    // Worst case   = search through last half of dataset_search, which should be NOT FOUND targets.
    // Average case = search through odd-numbered rows of dataset_search, which gives a mixed case.
    int half = q / 2;

    if (half == 0) {
        cerr << "Query file must contain at least 2 targets.\n" << endl;
        return;
    }

    vector<unsigned long long> bestCaseTargets;
    vector<unsigned long long> averageCaseTargets;
    vector<unsigned long long> worstCaseTargets;

    for (int i = 0; i < half; i++) {
        bestCaseTargets.push_back(queries[i]);
    }

    // Odd-numbered rows: 1st, 3rd, 5th, ...
    // Since C++ uses 0-based index, use 0, 2, 4, ...
    for (int i = 0; i < q; i += 2) {
        averageCaseTargets.push_back(queries[i]);
    }

    for (int i = half; i < q; i++) {
        worstCaseTargets.push_back(queries[i]);
    }

    int bestFoundCount = countFoundTargets(hashTable, bestCaseTargets);
    int averageFoundCount = countFoundTargets(hashTable, averageCaseTargets);
    int worstFoundCount = countFoundTargets(hashTable, worstCaseTargets);

    if (q != n) {
        cout << "Note: query file contains " << q << " targets, but dataset size is " << n << "." << endl;
        cout << "For timing, each case target list will be cycled until exactly n searches are performed." << endl;
    }

    // Perform exactly n searches for each case.
    // If a target group has fewer than n rows, the targets are repeated using modulo.
    int warmUpCount = min(n, 10000);
    warmUpSearchCase(hashTable, bestCaseTargets, warmUpCount);
    warmUpSearchCase(hashTable, averageCaseTargets, warmUpCount);
    warmUpSearchCase(hashTable, worstCaseTargets, warmUpCount);

    double bestTime = timeSearchCase(hashTable, bestCaseTargets, n);
    double averageTime = timeSearchCase(hashTable, averageCaseTargets, n);
    double worstTime = timeSearchCase(hashTable, worstCaseTargets, n);

    string timingOutputPath = deriveTimingOutputPath(datasetPath);
    ofstream timingFile(timingOutputPath);

    if (!timingFile.is_open()) {
        cerr << "Cannot create timing output file: " << timingOutputPath << endl;
        return;
    }

    timingFile << "Dataset file: " << datasetPath << "\n";
    timingFile << "Query/Search file: " << queryPath << "\n";
    timingFile << "Search result CSV: " << searchResultPath << "\n";
    timingFile << "Dataset size: " << n << "\n";
    timingFile << "Query target rows: " << q << "\n";
    timingFile << "Searches performed per case: " << n << "\n";
    timingFile << "Hash table size: " << tableSize << "\n";
    timingFile << "Collision handling: AVL tree in each hash bucket\n\n";

    timingFile << "Best case target group: first half of dataset_search (FOUND targets)\n";
    timingFile << "Best case target count: " << bestCaseTargets.size() << "\n";
    timingFile << "Best case FOUND count in target group: " << bestFoundCount << "\n\n";

    timingFile << "Average case target group: odd-numbered rows from dataset_search (1st, 3rd, 5th, ...)\n";
    timingFile << "Average case target count: " << averageCaseTargets.size() << "\n";
    timingFile << "Average case FOUND count in target group: " << averageFoundCount << "\n\n";

    timingFile << "Worst case target group: last half of dataset_search (NOT FOUND targets)\n";
    timingFile << "Worst case target count: " << worstCaseTargets.size() << "\n";
    timingFile << "Worst case FOUND count in target group: " << worstFoundCount << "\n\n";

    timingFile << "Best case time: " << bestTime << " seconds\n";
    timingFile << "Average case time: " << averageTime << " seconds\n";
    timingFile << "Worst case time: " << worstTime << " seconds\n";
    timingFile.close();

    cout << "Dataset size: " << n << endl;
    cout << "Query target rows: " << q << endl;
    cout << "Searches performed per case: " << n << endl;
    cout << "Search results saved to: " << searchResultPath << endl;

    cout << "Best case target group: first half of dataset_search (FOUND targets)" << endl;
    cout << "Best case target count: " << bestCaseTargets.size() << endl;
    cout << "Best case FOUND count in target group: " << bestFoundCount << endl;

    cout << "Average case target group: odd-numbered rows from dataset_search" << endl;
    cout << "Average case target count: " << averageCaseTargets.size() << endl;
    cout << "Average case FOUND count in target group: " << averageFoundCount << endl;

    cout << "Worst case target group: last half of dataset_search (NOT FOUND targets)" << endl;
    cout << "Worst case target count: " << worstCaseTargets.size() << endl;
    cout << "Worst case FOUND count in target group: " << worstFoundCount << endl;

    cout << "Best case time: " << bestTime << " seconds" << endl;
    cout << "Average case time: " << averageTime << " seconds" << endl;
    cout << "Worst case time: " << worstTime << " seconds" << endl;
    cout << "Timing output saved to: " << timingOutputPath << "\n" << endl;
}

int main(int argc, char* argv[]) {
    // Default paths for quick demo. These are used only when no command-line argument is given.
    const string datasetPath = "dataset/dataset_5000.csv";
    const string queryPath = "dataset_search/dataset_search_from_5000.csv";
    const string outputPath = "search_results_5000.csv";

    // Mode 1: no argument, use default constants above.
    if (argc == 1) {
        runHashTableSearch(datasetPath, queryPath, outputPath);
        return 0;
    }

    // Mode 2: explicit three paths.
    // Example:
    // hash_table_search.exe dataset/dataset_1000.csv dataset_search/dataset_search_from_1000.csv search_results_1000.csv
    if (argc == 4) {
        runHashTableSearch(argv[1], argv[2], argv[3]);
        return 0;
    }

    // Mode 3: one or more dataset files. Query and output paths are auto-generated.
    // Example:
    // hash_table_search.exe dataset/dataset_1000.csv dataset/dataset_5000.csv
    for (int i = 1; i < argc; i++) {
        string currentDatasetPath = argv[i];
        string currentQueryPath = deriveQueryPath(currentDatasetPath);
        string currentOutputPath = deriveSearchResultPath(currentDatasetPath);

        runHashTableSearch(currentDatasetPath, currentQueryPath, currentOutputPath);
    }

    return 0;
}
