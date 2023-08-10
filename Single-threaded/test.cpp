#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include "lru.cpp"

using namespace std;

typedef struct Operation {
    string opType;
    string key;
    string value;
} Operation;

vector<Operation> operations;

double DISK_LATENCY = 1.5;
string OPERATION_GET = "get";
string OPERATION_ADD = "add";

int main() {
    ifstream file("../../cluster045.txt");
    string line;

    while(getline(file, line)) {
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

    LRUCache<string, string> cache(100000);
    // TODO: change the cache size from 50000 to 400000, and compare the performance
    // ...
    // ...

    size_t requests = 0;
    size_t hits = 0;

    long duration = 0;

    cout << "Start processing..." << endl;

    for (auto op : operations) {
        requests++;
        auto eachStart = chrono::high_resolution_clock::now();
        if (op.opType == OPERATION_ADD) {
            cache.add(op.key, op.value);
            // wait for disk latency
            this_thread::sleep_for(chrono::microseconds(5));
        } else if (op.opType == OPERATION_GET) {
            string value = cache.get(op.key);
            if (!value.empty()) {
                hits++;
            } else {
                // wait for disk latency
                this_thread::sleep_for(chrono::microseconds(5));
                cache.add(op.key, op.value);
            }
        }
        auto eachEnd = chrono::high_resolution_clock::now();
        auto eachDuration = chrono::duration_cast<chrono::microseconds>(eachEnd - eachStart);

        duration += eachDuration.count();
    }

    cout << "End processing..." << endl;
    cout << "-----------------Results-------------------" << endl;

    cout << "requests: " << requests << endl;
    cout << "hits: " << hits << endl;
    cout << "hit ratio: " << (double)hits / requests << endl;
    cout << "total latency: " << duration << endl;
    cout << "average latency: " << (double)duration / requests << endl;

    return 0;
}