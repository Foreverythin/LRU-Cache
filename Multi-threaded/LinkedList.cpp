#include <iostream>
#include <thread>

#include "LinkedList.hpp"

template <typename T>
LinkedList<T>::~LinkedList() {
    Node* curr = head;
    while (curr != nullptr) {
        Node* temp = curr;
        curr = curr->next;
        delete temp;
    }
}

template <typename T>
void LinkedList<T>::append(const T& data) {
//    std::unique_lock lock(rwLock);
    size_t segmentIndex = std::hash(data) % segmentLocks.size();
    std::unique_lock lock(segmentLocks[segmentIndex]);  // 获取段锁的独占锁，保证只有一个线程写入
    Node* newNode = new Node(data);
    if (!head) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
}

template <typename T>
bool LinkedList<T>::erase(const T& data) {
//    std::unique_lock lock(rwLock);
    size_t segmentIndex = std::hash(data) % segmentLocks.size();
    std::unique_lock lock(segmentLocks[segmentIndex]);  // 获取段锁的独占锁，保证只有一个线程写入
    Node* current = head;
    while (current) {
        if (current->data == data) {
            if (current->prev) {
                current->prev->next = current->next;
            } else {
                head = current->next;
            }

            if (current->next) {
                current->next->prev = current->prev;
            } else {
                tail = current->prev;
            }

            delete current;
            return true;
        }
        current = current->next;
    }
    return false;
}

template <typename T>
void LinkedList<T>::print() const {
    std::shared_lock lock(rwLock);
    Node* current = head;
    while (current) {
        std::cout << current->data << " ";
        current = current->next;
    }
    std::cout << std::endl;
}

template <typename T>
void LinkedList<T>::pop_back() {
//    std::unique_lock lock(rwLock);
    if (tail) {
        Node* temp = tail;
        size_t segmentIndex = std::hash(temp) % segmentLocks.size();
        std::unique_lock lock(segmentLocks[segmentIndex]);  // 获取段锁的独占锁，保证只有一个线程写入
        tail = tail->prev;
        if (tail) {
            tail->next = nullptr;
        } else {
            head = nullptr;
        }
        delete temp;
    }
}

template <typename T>
void LinkedList<T>::push_front(const T& data) {
//    std::unique_lock lock(rwLock);
    size_t segmentIndex = std::hash(data) % segmentLocks.size();
    std::unique_lock lock(segmentLocks[segmentIndex]);  // 获取段锁的独占锁，保证只有一个线程写入
    Node* newNode = new Node(data);
    if (!head) {
        head = tail = newNode;
    } else {
        newNode->next = head;
        head->prev = newNode;
        head = newNode;
    }
}
