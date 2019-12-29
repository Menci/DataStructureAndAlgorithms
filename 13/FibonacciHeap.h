#ifndef _MENCI_FIBONACCIHEAP_H
#define _MENCI_FIBONACCIHEAP_H

#include <list>
#include <vector>
#include <memory>
#include <utility>
#include <stdexcept>
#include <cassert>

template <typename TKey,
          typename Compare = std::less<TKey>>
class FibonacciHeap {
    class Node {
        friend class FibonacciHeap;

        TKey key;
        std::weak_ptr<Node> parent;
        bool marked = false;
        std::list<std::shared_ptr<Node>> childList;

        typename std::list<std::shared_ptr<Node>>::iterator iterator;

        template <typename T>
        Node(T &&key, std::shared_ptr<Node> parent = nullptr) : key(key), parent(parent) {}
    };

    std::list<std::shared_ptr<Node>> rootList;
    int n = 0;
    std::shared_ptr<Node> minNode = nullptr;

    static size_t log2(size_t n) {
        size_t res = 0;
        while ((1UL << (res + 1)) <= n) res++;
        return res;
    }

    void computeMin() {
        minNode = nullptr;
        for (auto &node : rootList) {
            if (!minNode || Compare()(node->key, minNode->key)) {
                minNode = node;
            }
        }
    }

    template <typename T>
    void decreaseKey(std::shared_ptr<Node> node, T &&newKey) {
        node->key = newKey;
        if (Compare()(node->key, minNode->key)) minNode = node;

        auto y = node->parent.lock();
        if (!y || Compare()(y->key, node->key)) return;

        std::shared_ptr<Node> x = node;
        do {
            auto y = x->parent.lock();
            y->childList.erase(x->iterator);
            rootList.push_back(x);

            x->iterator = --rootList.end();
            x->parent = std::weak_ptr<Node>();
            x->marked = false;

            x = y;
        } while (x->marked && x->parent.lock());
    }

public:
    class NodeProxy {
        friend FibonacciHeap;

        std::weak_ptr<Node> node;
        FibonacciHeap *heap;

        NodeProxy(std::shared_ptr<Node> node, FibonacciHeap *heap) : node(node), heap(heap) {}

    public:
        NodeProxy() : NodeProxy(nullptr, nullptr) {}
        NodeProxy(const NodeProxy &) = default;

        const TKey &getKey() const {
            return node.lock()->key;
        }

        template <typename T>
        void decreaseKey(T &&newKey) {
            if (!Compare()(newKey, getKey())) {
                throw std::logic_error("new key is greater than current key");
            }

            heap->decreaseKey(node.lock(), newKey);
        }
    };

    template <typename T>
    NodeProxy push(T &&key) {
        std::shared_ptr<Node> newNode(new Node(std::move(key)));
        rootList.push_back(newNode);
        newNode->iterator = --rootList.end();

        if (!minNode || Compare()(newNode->key, minNode->key)) {
            minNode = newNode;
        }

        n++;
        return NodeProxy(newNode, this);
    }

    void deleteMin() {
        if (!minNode) return;

        rootList.erase(minNode->iterator);
        rootList.splice(rootList.end(), minNode->childList);

        n--;

        size_t maxDegree = 0;
        for (auto &root : rootList) maxDegree = std::max(maxDegree, root->childList.size());

        std::vector<std::shared_ptr<Node>> withDegree(2 * log2(n) + 1);
        for (auto &root : rootList) {
            size_t degree = root->childList.size();

            auto x = root;
            while (auto y = withDegree[degree]) {
                if (!Compare()(x->key, y->key)) std::swap(x, y);

                x->childList.push_back(y);
                y->parent = x;
                y->iterator = --x->childList.end();
                y->marked = false;

                withDegree[degree] = nullptr;
                degree++;
            }

            withDegree[degree] = x;
        }

        std::list<std::shared_ptr<Node>> newRootList;
        for (auto &root : withDegree) {
            if (root) newRootList.push_back(root);
        }

        rootList = std::move(newRootList);
        for (auto it = rootList.begin(); it != rootList.end(); it++) (*it)->iterator = it; 
        computeMin();
    }

    NodeProxy getMin() {
        return NodeProxy(minNode, this);
    }

    size_t size() const {
        return n;
    }

    bool empty() const {
        return n == 0;
    }

    static FibonacciHeap<TKey, Compare> merge(FibonacciHeap<TKey, Compare> &&heap1, FibonacciHeap<TKey, Compare> &&heap2) {
        FibonacciHeap<TKey, Compare> newHeap;
        newHeap.rootList = std::move(heap1.rootList);
        newHeap.rootList.splice(newHeap.rootList.end(), heap2.rootList);
        newHeap.n = heap1.n + heap2.n;
        newHeap.computeMin();
        return newHeap;
    }
};

#endif // _MENCI_FIBONACCIHEAP_H
