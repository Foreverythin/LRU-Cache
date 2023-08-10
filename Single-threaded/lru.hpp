#ifndef LRU_HPP
#define LRU_HPP

#include <list>
#include <unordered_map>

using namespace std;

template <typename KEY_T, typename VAL_T>
class LRUCache {
private:
    list<pair<KEY_T, VAL_T> > itemList;
    unordered_map<KEY_T, decltype(itemList.begin())> itemMap;
    size_t cacheSize;

    void clean(void);

public:
    LRUCache(size_t cacheSize) : cacheSize(cacheSize) {}

    void add(const KEY_T &key, const VAL_T &val);

    VAL_T get(const KEY_T &key);

    void showItemMap();
};

#endif // LRU_HPP