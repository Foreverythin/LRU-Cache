#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include "lru.cpp"

using namespace std;

#define OPERATION_GET "get"
#define OPERATION_ADD "add"

typedef struct Operation {
    string opType;
    string key;
    string value;
} Operation;

int DISK_LATENCY = 5000;  // in nanoseconds

vector<Operation> operations;

void cacheCapacityLab() {
    /* ------------------------ Open the target file ------------------------ */
    // Save the results to a file
    ofstream outfile;
    outfile.open("cacheCapacityLab_cluster010.csv", ios::app);
    if (!outfile.is_open()) {
        cout << "Failed to open file" << endl;
        return;
    }

    outfile << "requests,cacheSize,hitRatio\n";  // write the header
    /* --------------------------------------------------------------------- */


    /* -------------------------- Start processing -------------------------- */
    cout << "Start processing..." << endl;
    for (int cap = 2500; cap <= 20000; cap = cap + 2500) {  // change the cache capacity
        LRUCache<string, string> cache(cap);

        size_t requests = 0;  // number of requests
        size_t hits = 0;  // number of cache hits

        long duration = 0;  // total duration of the requests which will be updated by each thread

        for (auto op: operations) {
            requests++;
            auto eachStart = chrono::high_resolution_clock::now();
            if (op.opType == OPERATION_ADD) {  // if adding a new item to the cache
                cache.add(op.key, op.value);
                this_thread::sleep_for(chrono::nanoseconds(DISK_LATENCY));  // wait for disk latency
            } else if (op.opType == OPERATION_GET) {  // if getting the value associated with the given key
                string value = cache.get(op.key);
                if (!value.empty()) {
                    hits++;
                } else {
                    this_thread::sleep_for(chrono::nanoseconds(DISK_LATENCY));  // wait for disk latency
                    cache.add(op.key, op.value);  // add the new item to the cache
                }
            }
            auto eachEnd = chrono::high_resolution_clock::now();
            auto eachDuration = chrono::duration_cast<chrono::microseconds>(eachEnd - eachStart);

            duration += eachDuration.count();  // update the total duration
        }
        outfile << requests << "," << cap << "," << hits / (double) requests << "\n";  // write the result to the file
    }
    cout << "End processing..." << endl;
    outfile.close();
    /* --------------------------------------------------------------------- */
}

int main() {
    /* ------------------------ Read the input file ------------------------ */
    ifstream file("../../data/cluster010_5.txt");  // The path may need to be changed
    if (!file.is_open()) {
        cout << "Failed to open file" << endl;
        return 0;
    }
    /* --------------------------------------------------------------------- */


    /* --------------------- Initialize the operations --------------------- */
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        Operation op;
        string tmp;
        getline(ss, tmp, ',');
        getline(ss, op.key, ',');
        getline(ss, tmp, ',');
        getline(ss, tmp, ',');
        getline(ss, tmp, ',');
        getline(ss, op.opType, ',');
        op.value = op.key;
        operations.push_back(op);
    }
    /* --------------------------------------------------------------------- */


    /* ------------------------------ Testing ------------------------------ */
    cacheCapacityLab();
    /* --------------------------------------------------------------------- */


    return 0;
}