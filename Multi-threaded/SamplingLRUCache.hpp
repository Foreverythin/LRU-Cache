#ifndef MULTI_THREADED_SAMPLINGLRUCACHE_HPP
#define MULTI_THREADED_SAMPLINGLRUCACHE_HPP

#include <shared_mutex>
#include <random>
#include <queue>

#include "libcuckoo/cuckoohash_map.hh"


/**
 * @brief A cache value class that stores the value and the number of times it has been accessed.
 *
 * @tparam V The type of the value.
 */
template <typename V>
struct CacheValue {
    V value;
    std::chrono::time_point<std::chrono::steady_clock> lastAccessed;
};

/**
 * @brief A LRU cache class implemented by a hash map and the sampling algorithm.
 *
 * @tparam Key The type of the key.
 * @tparam Value The type of the value.
 */
template <typename Key, typename Value>
class SamplingLRUCache {
private:
    libcuckoo::cuckoohash_map<Key, CacheValue<Value> > map_;  // map from key to value
    int capacity_;  // Maximum number of elements in the cache
    int sampleSize_;  // Number of elements to sample
    std::shared_mutex mutex_;  // Mutex for locking the cache

public:
    SamplingLRUCache(int capacity, int sampleSize) : capacity_(capacity), sampleSize_(sampleSize) {}  // constructor

    /**
     * @brief Get a new item from the cache.
     *
     * @param key The key of the item.
     * @return Value
     */
    Value get(const Key& key) {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        if (map_.contains(key)) {
            auto lockedMap = map_.lock_table();
            lockedMap[key].lastAccessed = std::chrono::steady_clock::now();
            Value value = lockedMap[key].value;
            lockedMap.unlock();
            return value;
        }
        return Value();
    }

    /**
     * @brief Put a new item into the cache.
     *
     * @param key The key of the item.
     * @param value The value of the item.
     */
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

    /**
     * @brief Evict an item from the cache using the sampling algorithm.
     */
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
        // Sample sampleSize_ items from the cache
        std::sample(lockTable.begin(), lockTable.end(), std::back_inserter(samples), sampleSize_, gen);
        minTime = samples[0].second.lastAccessed;
        // Find the item with the minimum lastAccessed time
        for (auto& sample : samples) {
            if (sample.second.lastAccessed <= minTime) {
                minTime = sample.second.lastAccessed;
                minKey = sample.first;
            }
        }
        lockTable.erase(minKey);  // Erase the item with the minimum lastAccessed time
        lockTable.unlock();
    }

    /**
     * @brief Print the cache contents.
     */
    void print() {
        std::cout << "Cache contents:" << std::endl;
        for (auto& kv : map_.lock_table()) {
            std::cout << kv.first << ": " << kv.second.value << std::endl;
        }
    }
};


#endif //MULTI_THREADED_SAMPLINGLRUCACHE_HPP
