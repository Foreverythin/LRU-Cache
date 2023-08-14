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
int NUM_THREADS = 20;  // number of threads
int DEFAULT_CACHE_SIZE = 10000;  // default cache size
double DEFAULT_SAMPLE_RATE = 0.05;  // default sample rate

vector<Operation> operations;

size_t requests = 0;

size_t hits = 0;

// The function to add or get an item from the cache
void task(SamplingLRUCache<string, string> &cache, Operation &op) {
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

// The function to allocate tasks to threads
void execute(SamplingLRUCache<string, string> &cache, vector<Operation> &operations_, atomic<int> &index) {
    while (true) {
        int i = index.fetch_add(1);
        if (i >= operations_.size()) {
            break;
        }
        Operation op = operations_[i];
        task(cache, op);

        // Print the progress
        if (i % 10000 == 0) {
            std::cout << "Thread " << this_thread::get_id() << " executes task " << i << "\n";
        }
    }
}

// The function to run the cache capacity's lab
void cacheCapacityLab() {
    /* ------------------------ Open the target file ------------------------ */
    ofstream outfile;
    outfile.open("cacheCapacityLab_20threads_cluster010.csv", ios::app);
    if (!outfile.is_open()) {
        cout << "Failed to open file" << endl;
        return;
    }

    outfile << "requests,cacheSize,hitRatio,throughput\n";  // write the header
    /* ---------------------------------------------------------------------- */


    /* -------------------------- Start processing -------------------------- */
    cout << "Start processing..." << endl;

    for (int cap = 2500; cap <= 20000; cap = cap + 2500) {
        requests = 0;

        SamplingLRUCache<string, string> cache(cap,
                                               int(cap * DEFAULT_SAMPLE_RATE));  // each cache with 5% sampling rate

        long duration;  // total duration of the requests which will be updated by each thread

        auto start = chrono::high_resolution_clock::now();  // start time

        atomic<int> index(0);  // the index of the next task to be executed
        vector<thread> threads(NUM_THREADS);
        for (int i = 0; i < NUM_THREADS; i++) {
            threads[i] = thread(execute, ref(cache), ref(operations), ref(index));
        }

        // Wait for all threads to finish
        for (auto &thread: threads) {
            thread.join();
        }

        auto end = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();  // update the total duration

        outfile << requests << "," << cap << "," << (double) hits / requests << ","
                << ((double) requests / duration) * 1000 << "\n";  // write the result to the file
    }
    outfile.close();
    cout << "End processing..." << endl;
    /* ---------------------------------------------------------------------- */
}


// The function to run the thread number's lab
void threadNumLab(int threadNum) {
    /* ------------------------ Open the target file ------------------------ */
    ofstream outfile;
    outfile.open("threadNumLab_cluster010.csv", ios::app);
    if (!outfile.is_open()) {
        cout << "Failed to open file" << endl;
        return;
    }

    outfile << "requests,numThread,cacheSize,hitRatio,throughput\n";
    /* ---------------------------------------------------------------------- */


    /* -------------------------- Start processing -------------------------- */
    cout << "Start processing..." << endl;

    requests = 0;  // reset the requests

    cout << "Processing thread number: " << threadNum << endl;

    SamplingLRUCache<string, string> cache(DEFAULT_CACHE_SIZE, DEFAULT_CACHE_SIZE *
                                                               DEFAULT_SAMPLE_RATE);  // initialize the cache with default size and 5% sampling rate

    long duration;  // total duration of the requests which will be updated by each thread

    auto start = chrono::high_resolution_clock::now();  // start time

    atomic<int> index(0);  // the index of the next task to be executed
    vector<thread> threads(threadNum);
    for (int i = 0; i < threadNum; i++) {
        threads[i] = thread(execute, ref(cache), ref(operations), ref(index));
    }

    // Wait for all threads to finish
    for (auto &thread: threads) {
        thread.join();
    }

    auto end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();  // update the total duration

    outfile << requests << "," << threadNum << "," << 10000 << "," << (double) hits / requests << ","
            << ((double) requests / duration) * 1000 << "\n";  // write the result to the file

    outfile.close();
    cout << "End processing..." << endl;
    /* ---------------------------------------------------------------------- */
}

int main() {
    /* ------------------------ Read the input file ------------------------ */
    ifstream file("../../data/cluster010_5.txt");  // The path may need to be changed
    if (!file.is_open()) {
        cout << "Failed to open file" << endl;
        return 0;
    }

    string line;
    /* --------------------------------------------------------------------- */


    /* --------------------- Initialize the operations --------------------- */
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
    // threadNumLab(72);
    /* --------------------------------------------------------------------- */


    return 0;
}