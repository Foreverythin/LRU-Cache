#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>

#include "SamplingLRUCache.hpp"

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

size_t requests = 0;

size_t hits = 0;

void task(SamplingLRUCache<string, string>& cache, Operation& op){
    requests++;
    if (op.opType == OPERATION_ADD) {
        cache.put(op.key, op.value);
        // wait for disk latency
        this_thread::sleep_for(chrono::nanoseconds(DISK_LATENCY));
    } else if (op.opType == OPERATION_GET) {
        string value = cache.get(op.key);
        if (!value.empty()) {
            hits++;
        } else {
            // wait for disk latency
            this_thread::sleep_for(chrono::nanoseconds(DISK_LATENCY));
            cache.put(op.key, op.value);
        }
    }
}

void execute(SamplingLRUCache<string, string>& cache, vector<Operation>& operations_, atomic<int>& index) {
    while (true) {
        int i = index.fetch_add(1);
        if (i >= operations_.size()) {
            break;
        }
        Operation op = operations_[i];
        task(cache, op);
        if (i % 10000 == 0) {
            std::cout << "Thread " << this_thread::get_id() << " executes task " << i  << "\n";
        }
    }
}

int main() {
    ifstream file("../../cluster022_5.txt");
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

    SamplingLRUCache<string, string> cache(10000, 10);

    long duration = 0;

    cout << "Start processing..." << endl;
    auto start = chrono::high_resolution_clock::now();

    atomic<int> index(0);
    vector<thread> threads(20);
    for (int i = 0; i < 20; i++) {
        threads[i] = thread(execute, ref(cache), ref(operations), ref(index));
    }

    // 等待所有线程结束
    for (auto& thread : threads) {
        thread.join();
    }

    auto end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    cout << "End processing..." << endl;
    cout << "-----------------Results-------------------" << endl;

    cout << "requests: " << requests << endl;
    cout << "hits: " << hits << endl;
    cout << "hit ratio: " << (double)hits / requests << endl;
    cout << "total latency: " << duration << endl;
    cout << "average latency: " << (double)duration / requests << endl;


    return 0;
}