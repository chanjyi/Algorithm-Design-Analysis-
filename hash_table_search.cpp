// *********************************************************
// Program: hash_table_search.cpp
// Course: CCP6214 Algorithm Design and Analysis
// Lecture Class: TC6L
// Tutorial Class: T13L
// Trimester: 2610
// Member_1: 242UC244SR | Chan Jia Yi | chan.jia.yi1@student.mmu.edu.my | +60 12-253 9359
// Member_2: 242UC244QN | Chew Jia Yi | chew.jia.yi@student.mmu.edu.my | +60 13-282 3398
// Member_3: 242UC244FZ | Koay Yee Shuen | koay.yee.shuen@student.mmu.edu.my | +60 11-5680 0192
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
#include <iomanip>
using namespace std;

struct Record {
    unsigned long long key;
    string value;
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

int maxInt(int a, int b) {
    return (a > b) ? a : b;
}

int height(AVLNode* node) {
    return (node == nullptr) ? 0 : node->height;
}

int balanceFactor(AVLNode* node) {
    return (node == nullptr) ? 0 : height(node->left) - height(node->right);
}

AVLNode* rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* temp = x->right;

    x->right = y;
    y->left = temp;

    y->height = maxInt(height(y->left), height(y->right)) + 1;
    x->height = maxInt(height(x->left), height(x->right)) + 1;

    return x;
}

AVLNode* rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* temp = y->left;

    y->left = x;
    x->right = temp;

    x->height = maxInt(height(x->left), height(x->right)) + 1;
    y->height = maxInt(height(y->left), height(y->right)) + 1;

    return y;
}

AVLNode* insertAVL(AVLNode* node, const Record& record) {
    if (node == nullptr) {
        return new AVLNode(record);
    }

    if (record.key < node->data.key) {
        node->left = insertAVL(node->left, record);
    }
    else if (record.key > node->data.key) {
        node->right = insertAVL(node->right, record);
    }
    else {
        return node; // Duplicate keys are ignored. Dataset should be unique.
    }

    node->height = maxInt(height(node->left), height(node->right)) + 1;
    int balance = balanceFactor(node);

    // Left Left case
    if (balance > 1 && record.key < node->left->data.key) {
        return rotateRight(node);
    }

    // Right Right case
    if (balance < -1 && record.key > node->right->data.key) {
        return rotateLeft(node);
    }

    // Left Right case
    if (balance > 1 && record.key > node->left->data.key) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    // Right Left case
    if (balance < -1 && record.key < node->right->data.key) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

void deleteAVL(AVLNode* node) {
    if (node == nullptr) {
        return;
    }

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
        for (AVLNode* root : table) {
            deleteAVL(root);
        }
    }

    int hashFunction(unsigned long long key) const {
        return key % tableSize;
    }

    void insert(const Record& record) {
        int index = hashFunction(record.key);
        table[index] = insertAVL(table[index], record);
    }

    bool search(unsigned long long target) const {
        int index = hashFunction(target);
        AVLNode* current = table[index];

        while (current != nullptr) {
            if (target == current->data.key) {
                return true;
            }
            else if (target < current->data.key) {
                current = current->left;
            }
            else {
                current = current->right;
            }
        }

        return false;
    }

    int searchComparisons(unsigned long long target) const {
        int index = hashFunction(target);
        AVLNode* current = table[index];
        int comparisons = 0;

        while (current != nullptr) {
            comparisons++;

            if (target == current->data.key) {
                return comparisons;
            }
            else if (target < current->data.key) {
                current = current->left;
            }
            else {
                current = current->right;
            }
        }

        return comparisons;
    }

    unsigned long long getRootKeyForBestCase() const {
        for (AVLNode* root : table) {
            if (root != nullptr) {
                return root->data.key;
            }
        }
        return 0;
    }
};

vector<Record> readCSV(const string& filename) {
    vector<Record> records;
    ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        cerr << "Cannot open file: " << filename << endl;
        return records;
    }

    string line;

    while (getline(inputFile, line)) {
        if (line.empty()) {
            continue;
        }

        stringstream ss(line);
        string keyText;
        string valueText;

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

string extractDatasetSize(string filename) {
    size_t slashPosition = filename.find_last_of("/\\");

    if (slashPosition != string::npos) {
        filename = filename.substr(slashPosition + 1);
    }

    size_t underscorePosition = filename.find_last_of('_');
    size_t dotPosition = filename.find_last_of('.');

    if (underscorePosition == string::npos ||
        dotPosition == string::npos ||
        dotPosition <= underscorePosition) {
        return "unknown";
    }

    return filename.substr(underscorePosition + 1,
                           dotPosition - underscorePosition - 1);
}

void runSearchExperiment(const string& filename) {
    cout << "Processing: " << filename << endl;

    // Reading input is not timed.
    vector<Record> records = readCSV(filename);
    int n = records.size();

    if (n == 0) {
        cerr << "Dataset is empty or cannot be read. Skipping file.\n" << endl;
        return;
    }

    // Use n/2 buckets to create collisions.
    // This allows the AVL tree collision handling to be demonstrated.
    int tableSize = (n / 2 > 0) ? n / 2 : 1;
    HashTable hashTable(tableSize);

    // Building the hash table is setup work.
    // It is not included in the search timing.
    for (const Record& record : records) {
        hashTable.insert(record);
    }

    // ---------------- TARGET SELECTION ----------------

    // Best case: target is at the root of one bucket.
    unsigned long long bestTarget = hashTable.getRootKeyForBestCase();
    int bestComparisons = hashTable.searchComparisons(bestTarget);

    // Average case: search every existing key once.
    // Worst case: search keys that need maximum comparisons.
    unsigned long long worstTarget = records[0].key;
    vector<unsigned long long> worstTargets;

    int maxComparisons = -1;
    long long totalComparisons = 0;

    for (const Record& record : records) {
        int comparisons = hashTable.searchComparisons(record.key);
        totalComparisons += comparisons;

        if (comparisons > maxComparisons) {
            maxComparisons = comparisons;
            worstTarget = record.key;

            worstTargets.clear();
            worstTargets.push_back(record.key);
        }
        else if (comparisons == maxComparisons) {
            worstTargets.push_back(record.key);
        }
    }

    double averageComparisons = static_cast<double>(totalComparisons) / n;

    // Prevent compiler optimization from removing the search loops.
    volatile long long searchGuard = 0;

    // ---------------- BEST CASE TIMING ----------------
    // Search a direct/root key n times.
    auto startBest = chrono::high_resolution_clock::now();

    for (int i = 0; i < n; i++) {
        searchGuard += hashTable.search(bestTarget);
    }

    auto endBest = chrono::high_resolution_clock::now();

    double bestTime =
        chrono::duration<double>(endBest - startBest).count();

    // ---------------- AVERAGE CASE TIMING ----------------
    // Search every existing dataset key once.
    auto startAverage = chrono::high_resolution_clock::now();

    for (int i = 0; i < n; i++) {
        searchGuard += hashTable.search(records[i].key);
    }

    auto endAverage = chrono::high_resolution_clock::now();

    double averageTime =
        chrono::duration<double>(endAverage - startAverage).count();

    // ---------------- WORST CASE TIMING ----------------
    // Search keys that require the maximum number of comparisons.
    auto startWorst = chrono::high_resolution_clock::now();

    for (int i = 0; i < n; i++) {
        searchGuard += hashTable.search(worstTargets[i % worstTargets.size()]);
    }

    auto endWorst = chrono::high_resolution_clock::now();

    double worstTime =
        chrono::duration<double>(endWorst - startWorst).count();

    if (searchGuard == -1) {
        cout << "";
    }

    // ---------------- OUTPUT FILE ----------------

    string datasetSize = extractDatasetSize(filename);
    string outputFilename =
        "hash_table_search_dataset_" + datasetSize + ".txt";

    ofstream outputFile(outputFilename);

    outputFile << fixed << setprecision(9);

    outputFile << "Dataset file: " << filename << "\n";
    outputFile << "Dataset size: " << n << "\n";
    outputFile << "Hash table size: " << tableSize << "\n";
    outputFile << "Collision handling: AVL tree in each hash bucket\n\n";

    outputFile << "Best case target: " << bestTarget << "\n";
    outputFile << "Average case: search every existing dataset key once\n";
    outputFile << "Worst case target example: " << worstTarget << "\n\n";

    outputFile << "Best case comparisons per search: "
               << bestComparisons << "\n";
    outputFile << "Average case comparisons per search: "
               << averageComparisons << "\n";
    outputFile << "Worst case comparisons per search: "
               << maxComparisons << "\n\n";

    outputFile << "Best case time: "
               << bestTime << " seconds\n";
    outputFile << "Average case time: "
               << averageTime << " seconds\n";
    outputFile << "Worst case time: "
               << worstTime << " seconds\n";

    outputFile.close();

    // ---------------- COMMAND PROMPT OUTPUT ----------------

    cout << fixed << setprecision(9);

    cout << "n = " << n << endl;
    cout << "Best case comparisons per search: "
         << bestComparisons << endl;
    cout << "Average case comparisons per search: "
         << averageComparisons << endl;
    cout << "Worst case comparisons per search: "
         << maxComparisons << endl;

    cout << "Best case time: "
         << bestTime << " seconds" << endl;
    cout << "Average case time: "
         << averageTime << " seconds" << endl;
    cout << "Worst case time: "
         << worstTime << " seconds" << endl;

    cout << "Search timing output saved to: "
         << outputFilename << "\n" << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: hash_table_search dataset_1000.csv dataset_5000.csv ..."
             << endl;
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        runSearchExperiment(argv[i]);
    }

    return 0;
}