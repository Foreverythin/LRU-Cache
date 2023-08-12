#ifndef MULTI_THREADED_SAMPLINGLRUCACHE_HPP
#define MULTI_THREADED_SAMPLINGLRUCACHE_HPP

#include <shared_mutex>
#include <random>

#include "libcuckoo/cuckoohash_map.hh"

template <typename V>
struct CacheValue {
    V value;
    std::chrono::time_point<std::chrono::steady_clock> lastAccessed;
};

template <typename Key, typename Value>
class SamplingLRUCache {
private:
    libcuckoo::cuckoohash_map<Key, CacheValue<Value> > map_;
    int capacity_;  // Maximum number of elements in the cache
    int sampleSize_;  // Number of elements to sample
    std::shared_mutex mutex_;  // Mutex for locking the cache

public:
    SamplingLRUCache(int capacity, int sampleSize) : capacity_(capacity), sampleSize_(sampleSize) {}

    Value get(const Key& key) {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        if (map_.contains(key)) {
            map_.lock_table()[key].lastAccessed = std::chrono::steady_clock::now();
//            std::cout << "Timestamp:   " << std::chrono::duration_cast<std::chrono::milliseconds>(map_.find(key).lastAccessed.time_since_epoch()).count() << std::endl;
            return map_.lock_table()[key].value;
        }
        return Value();
    }

    void put(const Key& key, const Value& value) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        CacheValue<Value> cv;
        if (map_.contains(key)) {
            cv.value = value;
            cv.lastAccessed = std::chrono::steady_clock::now();
            map_.lock_table()[key] = cv;
            return;
        }
        if (map_.size() >= capacity_) {
            evict();
        }
        cv.value = value;
        cv.lastAccessed = std::chrono::steady_clock::now();
        map_.insert(key, cv);
    }

    void evict() {
        if (map_.empty()) {
            return;
        }
        std::random_device rd;
        std::mt19937 gen(rd());
        Key minKey;
        std::chrono::time_point<std::chrono::steady_clock> minTime;
        std::vector<std::pair<Key, CacheValue<Value> > > samples;
        auto lockTable = map_.lock_table();
        std::sample(lockTable.begin(), lockTable.end(), std::back_inserter(samples), sampleSize_, gen);
//        // show samples
//        std::cout << "=======SAMPLES========" << std::endl;
//        for (auto it = samples.begin(); it != samples.end(); ++it) {
//            std::cout << "key:   " << it->first << std::endl;
//            std::cout << "value:   " << it->second.value << std::endl;
//            std::cout << "timestamp:   " << std::chrono::duration_cast<std::chrono::milliseconds>(it->second.lastAccessed.time_since_epoch()).count() << std::endl;
//        }
        minTime = samples[0].second.lastAccessed;
        for (auto& sample : samples) {
            if (sample.second.lastAccessed <= minTime) {
                minTime = sample.second.lastAccessed;
                minKey = sample.first;
            }
        }
        lockTable.erase(minKey);
    }

    void print() {
        std::cout << "Cache contents:" << std::endl;
        for (auto& kv : map_.lock_table()) {
            std::cout << kv.first << ": " << kv.second.value << std::endl;
        }
    }
};


#endif //MULTI_THREADED_SAMPLINGLRUCACHE_HPP
