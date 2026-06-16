// *********************************************************
// Program: hash_table_search_step.cpp
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
        left = right = nullptr;
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
        return node; // Ignore duplicate key. Dataset should already be unique.

    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    int balance = getBalance(node);

    // Left Left
    if (balance > 1 && r.key < node->left->data.key)
        return rotateRight(node);

    // Right Right
    if (balance < -1 && r.key > node->right->data.key)
        return rotateLeft(node);

    // Left Right
    if (balance > 1 && r.key > node->left->data.key) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    // Right Left
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

vector<Record> readCSV(const string& filename) {
    vector<Record> records;
    ifstream infile(filename);
    string line;

    if (!infile.is_open()) {
        cerr << "Cannot open file: " << filename << endl;
        return records;
    }

    while (getline(infile, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string intPart, strPart;
        getline(ss, intPart, ',');
        getline(ss, strPart, ',');

        if (!intPart.empty()) {
            records.push_back({stoull(intPart), strPart});
        }
    }

    infile.close();
    return records;
}

bool containsKey(const vector<Record>& records, unsigned long long key) {
    for (const Record& r : records) {
        if (r.key == key) return true;
    }
    return false;
}

unsigned long long chooseNotFoundTarget(const vector<Record>& records) {
    unsigned long long target = 1234567890ULL;
    while (containsKey(records, target)) {
        target++;
    }
    return target;
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

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: hash_table_search_step <dataset_file.csv>" << endl;
        return 1;
    }

    string filename = argv[1];
    vector<Record> records = readCSV(filename);

    if (records.empty()) {
        cerr << "Dataset is empty or cannot be read." << endl;
        return 1;
    }

    // Tutor can change these targets during demo.
    // Found target: use one value from the dataset.
    unsigned long long foundTarget = records[0].key;

    // Not-found target: automatically choose a value that is not inside the dataset.
    unsigned long long notFoundTarget = chooseNotFoundTarget(records);

    // Table size. Using about n/2 creates some collisions, so the AVL tree path can be shown.
    int tableSize = max(1, (int)records.size() / 2);
    HashTable ht(tableSize);

    for (const Record& r : records)
        ht.insert(r);

    string n_str = extractSizeFromFilename(filename);

    string foundFilename = "dataset_" + n_str + "_hash_table_search_step_" + to_string(foundTarget) + ".txt";
    ofstream foundOut(foundFilename);
    ht.searchStep(foundTarget, foundOut);
    foundOut.close();

    string notFoundFilename = "dataset_" + n_str + "_hash_table_search_step_" + to_string(notFoundTarget) + ".txt";
    ofstream notFoundOut(notFoundFilename);
    ht.searchStep(notFoundTarget, notFoundOut);
    notFoundOut.close();

    cout << "Found target step output saved to: " << foundFilename << endl;
    cout << "Not-found target step output saved to: " << notFoundFilename << endl;

    return 0;
}
