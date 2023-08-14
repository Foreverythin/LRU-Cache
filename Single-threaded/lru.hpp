#ifndef LRU_HPP
#define LRU_HPP

#include <list>
#include <unordered_map>

using namespace std;

/**
 * @brief A LRU cache class implemented by a hash map and a doubly-linked list.
 *
 * @tparam KEY_T The type of the key.
 * @tparam VAL_T The type of the value.
 */
template <typename KEY_T, typename VAL_T>
class LRUCache {
private:
    list<pair<KEY_T, VAL_T> > itemList;  // list of items
    unordered_map<KEY_T, decltype(itemList.begin())> itemMap;  // map from key to item iterator
    size_t cacheSize;  // maximum number of items in the cache

    void clean(void);  // clean the cache when it is full

public:
    LRUCache(size_t cacheSize) : cacheSize(cacheSize) {}  // constructor

    void add(const KEY_T &key, const VAL_T &val);  // add a new item to the cache

    VAL_T get(const KEY_T &key);  // get the value associated with the given key

    void showItemMap();  // show the item map
};

#endif // LRU_HPP