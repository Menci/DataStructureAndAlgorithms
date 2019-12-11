#include "Framework.h"

#include <stdexcept>
#include <algorithm>

class UnionFindSet {
	std::vector<size_t> a;

public:
	UnionFindSet(size_t n) : a(n) {
		for (size_t i = 0; i < n; i++) a[i] = i;
	}

	int find(size_t x) {
		return x == a[x] ? x : a[x] = find(a[x]);
	}

	void merge(size_t x, size_t y) {
		a[find(x)] = find(y);
	}
};

std::vector<UEdge> minimumSpanningTree(size_t n, std::vector<UEdge> &edges) {
	UnionFindSet ufs(n);
	std::sort(edges.begin(), edges.end(), [](const UEdge &a, const UEdge &b) {
		return a.w < b.w;
	});

	size_t k = 0;
	std::vector<UEdge> newUEdges;
	for (const UEdge &edge : edges) {
		if (ufs.find(edge.u) == ufs.find(edge.v)) continue;

		ufs.merge(edge.u, edge.v);
		newUEdges.push_back(edge);
		if (++k == n - 1) return newUEdges;
	}

	throw std::invalid_argument("Graph not connected");
}
