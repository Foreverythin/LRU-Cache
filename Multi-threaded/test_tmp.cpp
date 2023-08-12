#include <iostream>
#include <string>
#include <chrono>
#include <random>

#include "libcuckoo/cuckoohash_map.hh"

// 定义一个结构体类型，用来存储缓存值和lru时间戳
struct CacheValue {
    std::string value;
    std::chrono::time_point<std::chrono::steady_clock> lru;
};

// 定义一个cache类
class Cache {
private:
    // 使用cuckoohash_map来存储键值对
    libcuckoo::cuckoohash_map<std::string, CacheValue> map;
    // 缓存容量上限
    int capacity;
    // 抽样数量
    int sample;
public:
    // 构造函数
    Cache(int capacity, int sample) {
        this->capacity = capacity;
        this->sample = sample;
    }

    // get函数
    std::string get(std::string key) {
        // 如果缓存中存在该键，则返回其值，并更新其lru时间戳
        CacheValue cv;
        std::cout << "get key: " << key << std::endl;
        if (map.find(key, cv)) {
            std::cout << "get key: " << key << std::endl;
            std::cout << "hhh:   " << std::chrono::duration_cast<std::chrono::milliseconds>(map.find(key).lru.time_since_epoch()).count() << std::endl;
            cv.lru = std::chrono::steady_clock::now();
            std::cout << "hhh:   " << std::chrono::duration_cast<std::chrono::milliseconds>(map.find(key).lru.time_since_epoch()).count() << std::endl;
            map.lock_table()[key] = cv;
            std::cout << "hhh:   " << std::chrono::duration_cast<std::chrono::milliseconds>(map.find(key).lru.time_since_epoch()).count() << std::endl;
//            map.upsert(key, cv);
            return cv.value;
        }
        // 否则返回空值
        return "";
    }

    // put函数
    void put(std::string key, std::string value) {
        // 如果缓存中已经存在该键，则更新其值和lru时间戳
        CacheValue cv;
        if (map.find(key, cv)) {
            std::cout << 1 << std::endl;
            cv.value = value;
            cv.lru = std::chrono::steady_clock::now();
            std::cout << 2 << std::endl;
            map.lock_table()[key] = cv;
            std::cout << 3 << std::endl;
//            map.upsert(key, cv);
            return;
        }
        // 如果缓存已满，则调用淘汰函数
        if (map.size() == capacity) {
            std::cout << 4 << std::endl;
            evict();
        }
        // 将该键值对插入到缓存中，并设置其lru时间戳
        cv.value = value;
        cv.lru = std::chrono::steady_clock::now();
        map.insert(key, cv);
    }

    // 淘汰函数
    void evict() {
        // 如果缓存为空，则直接返回
        if (map.empty()) {
            return;
        }
        // 创建一个随机数生成器
        std::random_device rd;
        std::mt19937 gen(rd());
        // 创建一个均匀分布的随机索引
        std::uniform_int_distribution<int> dis(0, map.size() - 1);
        // 从缓存中随机抽取一定数量的键，并记录其中lru最小的那个
        std::string min_key;
        std::chrono::time_point<std::chrono::steady_clock> min_lru;
//        for (int i = 0; i < sample; i++) {
//            // 生成一个随机索引
//            int index = dis(gen);
//            // 根据索引取出一个键和值
//            std::string key;
//            CacheValue cv;
//            map.get_item(index, key, cv);
//            // 比较该键的lru时间戳，如果比当前最小的还小，则更新最小的键和时间戳
//            if (i == 0 || cv.lru < min_lru) {
//                min_key = key;
//                min_lru = cv.lru;
//            }
//        }
        std::cout << 5 << std::endl;
        std::vector<std::pair<std::string, CacheValue> > sampleValues;
        auto lockTable = map.lock_table();
        std::sample(lockTable.begin(), lockTable.end(), std::back_inserter(sampleValues), sample, gen);
        min_lru = sampleValues[0].second.lru;
        std::cout << 6 << std::endl;
        for (auto &item : sampleValues) {
            if (item.second.lru < min_lru) {
                min_key = item.first;
                min_lru = item.second.lru;
            }
        }
        std::cout << 7 << std::endl;
        // 将lru最小的键从缓存中删除
        lockTable.erase(min_key);
        std::cout << 8 << std::endl;
    }

    // 打印函数，用来显示缓存中的内容
    void print() {
        auto lock_map = map.lock_table();
        for (auto it = lock_map.begin(); it != lock_map.end(); ++it) {
            std::cout << it->first << " : " << it->second.value << "\n";
        }
    }
};

// 测试代码
int main() {
    // 创建一个容量为3，抽样数量为2的cache对象
    Cache cache(3, 2);
    // 对其进行一些get和put操作，并打印结果
    cache.put("a", "1");
    cache.put("b", "2");
    cache.put("c", "3");
    cache.print();
    std::cout << "\n";
    cache.get("a");
//    cache.put("d", "4");
//    cache.print();
//    std::cout << "\n";
//    cache.get("b");
//    cache.put("e", "5");
//    cache.print();

    return 0;
}
