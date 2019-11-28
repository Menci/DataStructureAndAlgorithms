#ifndef _MENCI_MINHEAP_H
#define _MENCI_MINHEAP_H

#include <algorithm>
#include <vector>
#include <utility>

template <typename T,
          typename Container = std::vector<T>,
          typename Compare = std::less<T>>
class MinHeap {
    Container container;

    void adjustHeapPush(size_t i) {
        if (i == 0) return;

        size_t parentNode = (i - 1) / 2;
        if (Compare()(container[i], container[parentNode])) {
            std::swap(container[i], container[parentNode]);
            adjustHeapPush(parentNode);
        }
    }

    void adjustHeapPop(size_t i) {
        size_t leftChildNode = i * 2 + 1, rightChildNode = i * 2 + 2;
        if (leftChildNode >= container.size()) return;
        
        size_t lessChildNode;
        if (rightChildNode >= container.size()) {
            lessChildNode = leftChildNode;
        } else {
            if (Compare()(container[leftChildNode], container[rightChildNode])) {
                lessChildNode = leftChildNode;
            } else {
                lessChildNode = rightChildNode;
            }
        }

        if (Compare()(container[lessChildNode], container[i])) {
            std::swap(container[i], container[lessChildNode]);
            adjustHeapPop(lessChildNode);
        }
    }

public:
    void push(const T &value) {
        container.push_back(value);
        adjustHeapPush(container.size() - 1);
    }

    void push(T &&value) {
        container.push_back(std::forward<T>(value));
        adjustHeapPush(container.size() - 1);
    }

    void pop() {
        std::swap(container.front(), container.back());
        container.pop_back();
        adjustHeapPop(0);
    }

    const T &top() const {
        return container.front();
    }

    size_t size() const {
        return container.size();
    }

    bool empty() const {
        return container.empty();
    }
};

#endif // _MENCI_MINHEAP_H
