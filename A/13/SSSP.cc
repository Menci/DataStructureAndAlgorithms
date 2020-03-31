#include <fstream>
#include <iostream>
#include <functional>
#include <vector>
#include <chrono>
#include <limits>
#include <queue>

#include "FibonacciHeap.h"

struct Node {
    size_t id;

	// adjacency list
    struct Edge *firstEdge = nullptr;

	// for common sssp
    int dist, distAnswer;

	// for dijkstra
    FibonacciHeap<std::pair<int, Node *>>::NodeProxy nodeInFibonacciHeap;

	// for spfa
	bool enqueued;
	int relaxationCount;

	// for johnson
	int h;
};

std::vector<Node> nodes;

struct Edge {
    Node *from, *to;
    int w;
    Edge *next;
};

size_t addNode() {
	size_t i = nodes.size();
	nodes.push_back(Node{i});
	return i;
}

void addEdge(int u, int v, int w) {
    nodes[u].firstEdge = new Edge{&nodes[u], &nodes[v], w, nodes[u].firstEdge};
}

bool spfa(Node *start) {
	for (auto &node : nodes) {
        node.dist = std::numeric_limits<int>::max();
		node.enqueued = false;
		node.relaxationCount = 0;
	}

	std::queue<Node *> queue;

	start->dist = 0;
	start->enqueued = true;
	queue.push(start);

	while (!queue.empty()) {
		Node *v = queue.front();
		queue.pop();

		v->enqueued = false;

		for (Edge *e = v->firstEdge; e; e = e->next) {
			if (e->to->dist > v->dist + e->w) {
				e->to->dist = v->dist + e->w;
				e->to->relaxationCount++;

				// negative circle
				if (e->to->relaxationCount > nodes.size()) {
					return false;
				}

				if (!e->to->enqueued) {
					e->to->enqueued = true;
					queue.push(e->to);
				}
			}
		}
	}

	return true;
}

void dijkstra(Node *start) {
    for (auto &node : nodes) {
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
                e->to->nodeInFibonacciHeap.decreaseKey(std::make_pair(e->to->dist, e->to));
            }
        }
    }
}

bool johnson() {
	size_t extra = addNode();
	for (size_t i = 0; i < nodes.size() - 1; i++) addEdge(extra, i, 0);
	if (!spfa(&nodes[extra])) return false;

	for (auto &node : nodes) node.h = node.dist;

	for (auto &node : nodes) {
		for (Edge *e = node.firstEdge, **p = &node.firstEdge; e; ) {
			if (e->from->id == extra || e->to->id == extra) {
				// edges for johnson: delete it
				Edge *tmp = e;
				e = *p = e->next;
				delete tmp;
			} else {
				// edges in origin graph: transform it
				e->w = e->w + e->from->dist - e->to->dist;
				assert(e->w >= 0);

				p = &e->next;
				e = e->next;
			}
		}
	}

	nodes.pop_back();

	return true;
}

void measureTime(std::string actionName, std::function<void ()> function) {
    auto startTime = std::chrono::high_resolution_clock::now();

    function();

    auto endTime = std::chrono::high_resolution_clock::now();
    double timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    std::cerr << actionName << " costs " << timeElapsed / 1000 << "s" << std::endl;
}

int main() {
    int n, m;
    std::cin >> n >> m;
    for (int i = 0; i < n; i++) addNode();

    while (m--) {
		int u, v, w;
		std::cin >> u >> v >> w;
        addEdge(u, v, w);
    }

	bool noCheck = false;
	for (auto &node : nodes) {
		if (!(std::cin >> node.distAnswer)) {
			noCheck = true;
			break;
		}
	}

    measureTime("Bellman-Ford (SPFA)", []() {
        if (!spfa(&nodes[0])) std::cerr << "Bellman-Ford: negative circle found" << std::endl;;
    });

	if (!noCheck)
		for (auto &node : nodes) assert(node.dist == node.distAnswer);

	measureTime("Johnson + Dijkstra", []() {
		bool negativeCircle;
		measureTime("Johnson", [&]() {
			negativeCircle = !johnson();
			if (negativeCircle) std::cerr << "Johnson: negative circle found" << std::endl;
		});

		if (!negativeCircle) {
			measureTime("Dijkstra", []() {
				dijkstra(&nodes[0]);
			});

			for (auto &node : nodes) node.dist = node.dist + node.h - nodes[0].h;
		}
	});

	if (!noCheck)
		for (auto &node : nodes) assert(node.dist == node.distAnswer);
}
