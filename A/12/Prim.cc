#include "Framework.h"

#include <stdexcept>
#include <algorithm>
#include <numeric>

#include "FibonacciHeap.h"

std::vector<UEdge> minimumSpanningTree(size_t n, std::vector<UEdge> &edges) {
	struct Edge;

	struct Node {
		size_t id;
		const Edge *previousEdge = nullptr;
		size_t w = std::numeric_limits<size_t>::max();
		FibonacciHeap<std::pair<size_t, Node *>>::NodeProxy nodeInFibonacciHeap;
		bool poped = false;

		std::vector<Edge> edges;
	};

	struct Edge {
		Node &from, &to;
		size_t w;
	};

	std::vector<Node> nodes(n);

	for (const UEdge &edge : edges) {
		nodes[edge.u].edges.push_back(Edge{nodes[edge.u], nodes[edge.v], edge.w});
		nodes[edge.v].edges.push_back(Edge{nodes[edge.v], nodes[edge.u], edge.w});
	}

	FibonacciHeap<std::pair<size_t, Node *>> heap;
	size_t i = 0;
	for (Node &node : nodes) {
		node.nodeInFibonacciHeap = heap.push(std::make_pair(node.w, &node));
		node.id = i++;
	}

	nodes[0].w = 0;
	nodes[0].nodeInFibonacciHeap.decreaseKey(std::make_pair(0, &nodes[0]));

	std::vector<UEdge> newEdges;
	while (!heap.empty()) {
		auto pair = heap.getMin().getKey();
		heap.deleteMin();

		Node &u = *pair.second;
		u.poped = true;

		if (u.previousEdge) newEdges.push_back(UEdge{u.previousEdge->from.id, u.previousEdge->to.id, u.previousEdge->w});

		for (const Edge &edge : u.edges) {
			if (!edge.to.poped && edge.to.w > edge.w) {
				edge.to.w = edge.w;
				edge.to.previousEdge = &edge;
				edge.to.nodeInFibonacciHeap.decreaseKey(std::make_pair(edge.to.w, &edge.to));
			}
		}
	}

	if (newEdges.size() != n - 1) throw std::invalid_argument("Graph not connected");
	return newEdges;
}
