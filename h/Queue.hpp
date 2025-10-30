#ifndef PROJECT_BASE_V1_1_QUEUE_HPP
#define PROJECT_BASE_V1_1_QUEUE_HPP

#include "../h/MemoryAllocator.hpp"

template<typename T>
class Queue {
private:
    struct Elem {
        T* data;
        Elem* next;

        Elem(T* data, Elem* next) : data(data), next(next) {}
    };

    Elem *head, *tail;

public:
    Queue() : head(nullptr), tail(nullptr) {}

    Queue(const Queue<T> &) = delete;

    Queue<T> &operator= (const Queue<T> &) = delete;

    void addFirst(T* data) {
        Elem* elem = new Elem(data, head);
        head = elem;
        if (!tail) {
            tail = head;
        }
    }

    void addLast(T* data) {
        Elem* elem = new Elem(data, nullptr);
        if (tail) {
            tail->next = elem;
            tail = elem;
        } else {
            head = tail = elem;
        }
    }

    T* removeFirst() {
        if (!head) return nullptr;

        Elem* elem = head;
        head = head->next;
        if (!head) tail = nullptr;

        T* ret = elem->data;
        delete elem;
        return ret;
    }

    T* removeLast() {
        if (!head) return nullptr;

        Elem* prev = nullptr;
        for (Elem* curr = head; curr && curr != tail; curr = curr->next) {
            prev = curr;
        }

        Elem* elem = tail;
        if (prev) prev->next = nullptr;
        else head = nullptr;
        tail = prev;

        T* ret = elem->data;
        delete elem;
        return ret;
    }

    T* peekFirst() {
        if (!head) return nullptr;
        return head->data;
    }

    T* peekLast() {
        if (!tail) return nullptr;
        return tail->data;
    }

};

#endif //PROJECT_BASE_V1_1_QUEUE_HPP