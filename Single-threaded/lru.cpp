#include <iostream>
#include "lru.hpp"

using namespace std;

template <typename KEY_T, typename VAL_T>
void LRUCache<KEY_T, VAL_T>::clean(void) {
    while (itemMap.size() > cacheSize) {
        auto last_it = itemList.end();
        last_it--;
        itemMap.erase(last_it->first);
        itemList.pop_back();
    }
}

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

template <typename KEY_T, typename VAL_T>
VAL_T LRUCache<KEY_T, VAL_T>::get(const KEY_T &key) {
    auto it = itemMap.find(key);
    if (it == itemMap.end()) {
        return VAL_T();
    } else {
        return it->second->second;
    }
}

template <typename KEY_T, typename VAL_T>
void LRUCache<KEY_T, VAL_T>::showItemMap() {
    cout << "itemMap: " << endl;
    for (auto it = itemMap.begin(); it != itemMap.end(); it++) {
        cout << it->first << " " << it->second->first << " " << it->second->second << endl;
    }
}