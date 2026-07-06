// *********************************************************
// Program: hash_table_search_step.cpp
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
#include <algorithm>
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
        return node;

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

    void insert(const Record& r) {
        int index = hashFunction(r.key);
        table[index] = insertAVL(table[index], r);
    }

    bool searchStep(unsigned long long target, ofstream& outfile) const {
        int index = hashFunction(target);
        outfile << "Target: " << target << "\n";
        outfile << "Hash index = " << target << " % " << tableSize << " = " << index << "\n";

        AVLNode* current = table[index];
        if (current == nullptr) {
            outfile << "Bucket is empty.\n";
            outfile << "-1 != " << target << "\n";
            return false;
        }

        if (current->data.key == target) {
            outfile << "Direct search at bucket root.\n";
            outfile << current->data.key << " = " << current->data.key << "/" << current->data.value << "\n";
            return true;
        }

        outfile << "Collision exists. Search through AVL tree in this bucket.\n";
        while (current != nullptr) {
            outfile << "Compare: " << current->data.key;

            if (target == current->data.key) {
                outfile << " = " << target << " -> found "
                        << current->data.key << "/" << current->data.value << "\n";
                return true;
            }
            else if (target < current->data.key) {
                outfile << " > " << target << ", go left\n";
                current = current->left;
            }
            else {
                outfile << " < " << target << ", go right\n";
                current = current->right;
            }
        }

        outfile << "-1 != " << target << "\n";
        return false;
    }
};

vector<Record> readDatasetCSV(const string& filename) {
    vector<Record> records;
    ifstream infile(filename);
    string line;

    if (!infile.is_open()) {
        cerr << "Cannot open dataset file: " << filename << endl;
        return records;
    }

    while (getline(infile, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string intPart, strPart;
        getline(ss, intPart, ',');
        getline(ss, strPart, ',');

        if (!strPart.empty() && strPart.back() == '\r') {
            strPart.pop_back();
        }

        if (!intPart.empty() && !strPart.empty()) {
            records.push_back({stoull(intPart), strPart});
        }
    }

    infile.close();
    return records;
}

vector<unsigned long long> readQueryCSV(const string& filename) {
    vector<unsigned long long> queries;
    ifstream infile(filename);
    string line;

    if (!infile.is_open()) {
        cerr << "Cannot open dataset_search file: " << filename << endl;
        return queries;
    }

    while (getline(infile, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string keyText;
        getline(ss, keyText, ',');

        if (!keyText.empty()) {
            queries.push_back(stoull(keyText));
        }
    }

    infile.close();
    return queries;
}

string extractSizeFromFilename(string filename) {
    size_t slashPos = filename.find_last_of("/\\");
    if (slashPos != string::npos) filename = filename.substr(slashPos + 1);

    size_t underPos = filename.find_last_of("_");
    size_t dotPos = filename.find_last_of(".");

    if (underPos == string::npos || dotPos == string::npos || dotPos <= underPos)
        return "unknown";

    return filename.substr(underPos + 1, dotPos - underPos - 1);
}

string deriveQueryFilename(const string& datasetFilename) {
    string n_str = extractSizeFromFilename(datasetFilename);
    return "dataset_search/dataset_search_from_" + n_str + ".csv";
}

int main(int argc, char* argv[]) {
    string datasetFilename;

    if (argc < 2) {
        // Fallback: Ask the user to type it into the VS Code terminal pane
        cout << "Enter the path to the dataset CSV file: ";
        cin >> datasetFilename;
    } else {
        datasetFilename = argv[1];
    }

    string queryFilename = deriveQueryFilename(datasetFilename);

    vector<Record> records = readDatasetCSV(datasetFilename);
    vector<unsigned long long> queries = readQueryCSV(queryFilename);

    if (records.empty()) {
        cerr << "Dataset is empty or cannot be read." << endl;
        return 1;
    }

    if (queries.empty()) {
        cerr << "dataset_search file is empty or cannot be read." << endl;
        return 1;
    }

    int tableSize = max(1, (int)records.size() / 2);
    HashTable ht(tableSize);

    for (const Record& r : records) {
        ht.insert(r);
    }

    string n_str = extractSizeFromFilename(datasetFilename);
    string outputFilename = "dataset_" + n_str + "_hash_table_search_step_all.txt";

    ofstream outfile(outputFilename);

    if (!outfile.is_open()) {
        cerr << "Cannot create output file: " << outputFilename << endl;
        return 1;
    }

    outfile << "Dataset file: " << datasetFilename << "\n";
    outfile << "dataset_search file: " << queryFilename << "\n";
    outfile << "Dataset records inserted into hash table: " << records.size() << "\n";
    outfile << "dataset_search targets compared: " << queries.size() << "\n";
    outfile << "Hash table size: " << tableSize << "\n";
    outfile << "Output type: Search path for every target in dataset_search\n\n";

    int foundCount = 0;
    int notFoundCount = 0;

    for (int i = 0; i < (int)queries.size(); i++) {
        outfile << "==================================================\n";
        outfile << "Query row: " << (i + 1) << " of " << queries.size() << "\n";

        bool found = ht.searchStep(queries[i], outfile);

        if (found) {
            foundCount++;
            outfile << "Result: FOUND\n";
        }
        else {
            notFoundCount++;
            outfile << "Result: NOT_FOUND\n";
        }

        outfile << "\n";
    }

    outfile << "==================================================\n";
    outfile << "SUMMARY\n";
    outfile << "Total targets compared: " << queries.size() << "\n";
    outfile << "Found count: " << foundCount << "\n";
    outfile << "Not-found count: " << notFoundCount << "\n";

    outfile.close();

    cout << "Dataset file: " << datasetFilename << endl;
    cout << "dataset_search file: " << queryFilename << endl;
    cout << "Dataset records inserted into hash table: " << records.size() << endl;
    cout << "dataset_search targets compared: " << queries.size() << endl;
    cout << "Found count: " << foundCount << endl;
    cout << "Not-found count: " << notFoundCount << endl;
    cout << "Step output for all targets saved to: " << outputFilename << endl;

    return 0;
}
