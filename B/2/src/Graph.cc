#include "Graph.h"

Graph::Graph(std::istream &in) {
    size_t nodeCount;
    in >> nodeCount;

    for (size_t u, v; in >> u >> v; ) {
        edges.push_back({u, v});
        edges.push_back({v, u});
    }

    std::sort(edges.begin(), edges.end(), [](auto a, auto b) {
        return a.from != b.from ? a.from < b.from : a.to < b.to;
    });

    firstEdgeByNode.resize(nodeCount);
    for (auto it = edges.cbegin(); it != edges.cend(); it++) {
        if (it == edges.cbegin() || it->from != (it - 1)->from) {
            firstEdgeByNode[it->from] = it;
        }
    }
}

bool Graph::hasEdge(size_t nodeA, size_t nodeB) const {
    return std::binary_search(
        firstEdgeByNode[nodeA],
        nodeA == firstEdgeByNode.size() - 1 ? edges.cend() : firstEdgeByNode[nodeA + 1],
        nodeB,
        [](auto a, auto b) {
            return (size_t)a < (size_t)b;
        }
    );
}

std::vector<size_t> Graph::edgeIntersection(size_t nodeA, size_t nodeB) const {
    auto beginA = firstEdgeByNode[nodeA],
         endA = nodeA == firstEdgeByNode.size() - 1 ? edges.cend() : firstEdgeByNode[nodeA + 1],
         beginB = firstEdgeByNode[nodeB],
         endB = nodeB == firstEdgeByNode.size() - 1 ? edges.cend() : firstEdgeByNode[nodeB + 1];
    
    std::vector<size_t> result;
    std::set_intersection(
        beginA, endA,
        beginB, endB,
        std::back_inserter(result),
        [](auto a, auto b) {
            return (size_t)a < (size_t)b;
        }
    );

    return result;
}
