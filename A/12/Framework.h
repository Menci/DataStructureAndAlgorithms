#include <iostream>
#include <cassert>
#include <vector>

struct UEdge {
	size_t u, v, w;
};

std::vector<UEdge> minimumSpanningTree(size_t, std::vector<UEdge> &);

int main() {
	size_t n, m;
	std::cin >> n >> m;

	std::vector<UEdge> edges(m);

	for (UEdge &edge : edges) std::cin >> edge.u >> edge.v >> edge.w;

	std::vector<UEdge> newUEdges = minimumSpanningTree(n, edges);
	assert(newUEdges.size() == n - 1);

	for (const UEdge &edge : newUEdges) std::cout << edge.u << " " << edge.v << std::endl;
}
