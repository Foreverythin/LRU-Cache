#include <thread>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <sstream>

#include "lru.cpp"
#include "LinkedList.cpp"

using namespace std;

#define OPERATION_GET "get"
#define OPERATION_ADD "add"

typedef struct Operation {
    string opType;
    string key;
    string value;
} Operation;

int DISK_LATENCY = 1500;  // in nanoseconds

vector<Operation> operations;

int main(void) {
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

    LRUCache<string, string> cache(100000, 10);

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
            this_thread::sleep_for(chrono::nanoseconds(DISK_LATENCY));
        } else if (op.opType == OPERATION_GET) {
            string value = cache.get(op.key);
            if (!value.empty()) {
                hits++;
            } else {
                // wait for disk latency
                this_thread::sleep_for(chrono::nanoseconds(DISK_LATENCY));
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