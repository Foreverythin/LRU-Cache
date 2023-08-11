// Using Segmented locks
#ifndef MULTI_THREADED_LINKEDLIST_HPP
#define MULTI_THREADED_LINKEDLIST_HPP

#include <shared_mutex>
#include <vector>
#include <list>

template <typename T>
class LinkedList {
private:
    struct Node {
        T data;
        Node* prev;
        Node* next;

        Node(const T& data) : data(data), prev(nullptr), next(nullptr) {}
    };

    Node* head;
    Node* tail;
    mutable std::shared_timed_mutex rwLock; // 全局读写锁
    std::vector<std::shared_timed_mutex> segmentLocks; // 段读写锁

//    //定义一个内部类，用于实现迭代器
//    class LinkedListIterator {
//    private:
//        Node* curr;
//        mutable std::shared_timed_mutex* rwLock;
//    public:
//        LinkedListIterator(Node* curr, std::shared_timed_mutex* rwLock) : curr(curr), rwLock(rwLock) {}
//        LinkedListIterator& operator++() {
//            std::unique_lock lock(*rwLock);
//            curr = curr->next;
//            return *this;
//        }
//        LinkedListIterator operator++(int) {
//            std::unique_lock lock(*rwLock);
//            LinkedListIterator iterator = *this;
//            curr = curr->next;
//            return iterator;
//        }
//        bool operator==(const LinkedListIterator& other) const {
//            std::shared_lock lock(*rwLock);
//            return curr == other.curr;
//        }
//        bool operator!=(const LinkedListIterator& other) const {
//            std::shared_lock lock(*rwLock);
//            return curr != other.curr;
//        }
//        T& operator*() const {
//            std::shared_lock lock(*rwLock);
//            return curr->data;
//        }
//    };

public:
    LinkedList(size_t numSegments) : head(nullptr), tail(nullptr), segmentLocks(numSegments) {}

    ~LinkedList();

    void append(const T& data);

    bool erase(const T& data);

    void pop_back();

    void push_front(const T& data);

    auto begin() const {
        // return the first node
        std::shared_lock lock(rwLock);
        return *head;
    }

    auto end() const {
        // return the last node
        std::shared_lock lock(rwLock);
        return *tail;
    }

    void print() const;

};


#endif //MULTI_THREADED_LINKEDLIST_HPP
