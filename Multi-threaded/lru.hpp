#ifndef MULTI_THREADED_LRU_HPP
#define MULTI_THREADED_LRU_HPP

#include "LinkedList.hpp"
#include "libcuckoo/cuckoohash_map.hh"
#include <unordered_map>

using namespace std;

template <typename KEY_T, typename VAL_T>
class LRUCache {
private:
    LinkedList<pair<KEY_T, VAL_T> > itemList;
    libcuckoo::cuckoohash_map<KEY_T, decltype(itemList.begin())> itemMap;
//    libcuckoo::cuckoohash_map<KEY_T, typename LinkedList<pair<KEY_T, VAL_T> >::iterator> itemMap;
    size_t cacheSize;

    void clean();

public:
    LRUCache(size_t cacheSize, size_t numSegments) : cacheSize(cacheSize), itemList(numSegments) {}

    void add(const KEY_T &key, const VAL_T &val);

    VAL_T get(const KEY_T &key);

    void showItemMap();
};


#endif //MULTI_THREADED_LRU_HPP
