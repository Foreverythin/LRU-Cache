#include <iostream>
#include "lru.hpp"

using namespace std;

/**
 * @brief When the cache is full, the least recently used item is removed.
 *
 * @tparam KEY_T The type of the key.
 * @tparam VAL_T The type of the value.
 */
template <typename KEY_T, typename VAL_T>
void LRUCache<KEY_T, VAL_T>::clean(void) {
    while (itemMap.size() > cacheSize) {
        auto last_it = itemList.end();
        last_it--;
        itemMap.erase(last_it->first);
        itemList.pop_back();
    }
}

/**
 * @brief Adds a new item to the cache.
 *
 * @tparam KEY_T The type of the key.
 * @tparam VAL_T The type of the value.
 * @param key The key of the item.
 * @param val The value of the item.
 */
template <typename KEY_T, typename VAL_T>
void LRUCache<KEY_T, VAL_T>::add(const KEY_T &key, const VAL_T &val) {
    auto it = itemMap.find(key);
    if (it != itemMap.end()) {
        itemList.erase(it->second);
        itemMap.erase(it);
    }
    itemList.push_front(make_pair(key, val));
    itemMap[key] = itemList.begin();

    clean();
}

/**
 * @brief Retrieves the value associated with the given key from the cache.
 *
 * @tparam KEY_T The type of the key.
 * @tparam VAL_T The type of the value.
 * @param key The key to retrieve the value for.
 * @return The value associated with the key, or a default-constructed value if the key is not found.
 */
template <typename KEY_T, typename VAL_T>
VAL_T LRUCache<KEY_T, VAL_T>::get(const KEY_T &key) {
    auto it = itemMap.find(key);
    if (it == itemMap.end()) {
        return VAL_T();
    } else {
        return it->second->second;
    }
}

/**
 * @brief Displays the contents of the itemMap.
 *
 * @tparam KEY_T The type of the key.
 * @tparam VAL_T The type of the value.
 */
template <typename KEY_T, typename VAL_T>
void LRUCache<KEY_T, VAL_T>::showItemMap() {
    cout << "itemMap: " << endl;
    for (auto it = itemMap.begin(); it != itemMap.end(); it++) {
        cout << it->first << " " << it->second->first << " " << it->second->second << endl;
    }
}