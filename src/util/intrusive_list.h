#pragma once

#include <vector>

template <typename T>
struct IntrusiveListNode {
    virtual ~IntrusiveListNode() = default;
    IntrusiveListNode<T> *prev = nullptr;
    IntrusiveListNode<T> *next = nullptr;
};

template <typename T>
void IntrusiveListInit(IntrusiveListNode<T> *head) {
    head->prev = head;
    head->next = head;
}

template <typename T>
void IntrusiveListInsert(IntrusiveListNode<T> *head, IntrusiveListNode<T> *node) {
    node->prev = head;
    node->next = head->next;
    head->next->prev = node;
    head->next = node;
}

template <typename T>
void IntrusiveListRemove(IntrusiveListNode<T> *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

template <typename T>
bool IntrusiveListIsEmpty(IntrusiveListNode<T> *head) {
    return head->next == head;
}

template <typename T>
T* IntrusiveListFront(T *head) {
    if (IntrusiveListIsEmpty(head)) {
        return nullptr;
    }
    return dynamic_cast<T*>(head->next);
}

template <typename T>
std::vector<T*> IntrusiveListGetAllNodes(T *head) {
    std::vector<T*> result;
    auto cur = head->next;
    while(cur != head) {
        result.push_back(dynamic_cast<T*>(cur));
        cur = cur->next;
    }
    return result;
}