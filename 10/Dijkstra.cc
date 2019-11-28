#include <fstream>
#include <iostream>
#include <functional>
#include <vector>
#include <chrono>
#include <limits>
#include <queue>

#include "BinaryHeap.h"
#include "FibonacciHeap.h"

struct Node {
    int id;
    struct Edge *firstEdge = nullptr;
    Node *previousNode;
    int dist;

    FibonacciHeap<std::pair<int, Node *>>::NodeProxy nodeInFibonacciHeap;
};

std::vector<Node> nodes;

struct Edge {
    Node *from, *to;
    int w;
    Edge *next;
};

void addEdge(int u, int v, int w) {
    nodes[u].firstEdge = new Edge{&nodes[u], &nodes[v], w, nodes[u].firstEdge};
}

void printPath(std::ostream &out, Node *v) {
    std::function<void (Node *)> printPathForNode = [&](Node *u) {
        if (!u->previousNode) {
            out << u->id;
        } else {
            printPathForNode(u->previousNode);
            out << " -> " << u->id;
        }
    };

    out << "[" << v->dist << "] ";
    printPathForNode(v);
    out << std::endl;
}

void dijkstraWithBinaryHeap(Node *start) {
    for (auto &node : nodes) {
        node.previousNode = nullptr;
        node.dist = std::numeric_limits<int>::max();
    }

    BinaryHeap<std::pair<int, Node *>> heap;
    start->dist = 0;
    heap.push({0, start});

    while (!heap.empty()) {
        auto pair = heap.getMin();
        heap.deleteMin();

        if (pair.first > pair.second->dist) continue;

        Node *v = pair.second;
        for (Edge *e = v->firstEdge; e; e = e->next) {
            if (e->to->dist > v->dist + e->w) {
                e->to->dist = v->dist + e->w;
                e->to->previousNode = v;
                heap.push({e->to->dist, e->to});
            }
        }
    }
}

void dijkstraWithFibonacciHeap(Node *start) {
    for (auto &node : nodes) {
        node.previousNode = nullptr;
        node.dist = std::numeric_limits<int>::max();
    }

    FibonacciHeap<std::pair<int, Node *>> heap;
    start->dist = 0;

    for (auto &node : nodes) {
        node.nodeInFibonacciHeap = heap.push(std::make_pair(node.dist, &node));
    }

    while (!heap.empty()) {
        auto pair = heap.getMin().getKey();
        heap.deleteMin();

        Node *v = pair.second;
        for (Edge *e = v->firstEdge; e; e = e->next) {
            if (e->to->dist > v->dist + e->w) {
                e->to->dist = v->dist + e->w;
                e->to->previousNode = v;
                e->to->nodeInFibonacciHeap.decreaseKey(std::make_pair(e->to->dist, e->to));
            }
        }
    }
}

void measureTime(std::string actionName, std::function<void ()> function) {
    auto startTime = std::chrono::high_resolution_clock::now();

    function();

    auto endTime = std::chrono::high_resolution_clock::now();
    double timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    std::cerr << actionName << " costs " << timeElapsed / 1000 << "s" << std::endl;
}

int main() {
    std::ifstream fin("./graph.txt");
    std::ofstream fout("./ans.out");

    int n, m;
    fin >> n >> m;
    n++;
    nodes.resize(n);
    for (int i = 0; i < n; i++) nodes[i].id = i;

    for (int u, v, w; fin >> u >> v >> w; ) {
        addEdge(u, v, w);
    }

    measureTime("Dijkstra with Binary Heap", []() {
        dijkstraWithBinaryHeap(&nodes[0]);
    });

    for (int i = 1; i < n; i++) {
        printPath(fout, &nodes[i]);
    }

    measureTime("Dijkstra with Fibonacci Heap", []() {
        dijkstraWithFibonacciHeap(&nodes[0]);
    });

    for (int i = 1; i < n; i++) {
        printPath(fout, &nodes[i]);
    }
}
