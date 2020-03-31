#ifndef _MENCI_GRAPH_H
#define _MENCI_GRAPH_H

#include <istream>
#include <algorithm>
#include <vector>

class Graph {
    struct Edge {
        size_t from, to;

        operator size_t() const {
            return to;
        }
    };

    // All edges stored in an sorted array. This structure can help us finding edges efficiently.
    // See "2.2. Forward and Reverse Star Representation" in http://ncgia.ucsb.edu/units/u064/u064.html
    std::vector<Edge> edges;

    // The first out edge's position of each node.
    std::vector<std::vector<Edge>::const_iterator> firstEdgeByNode;

public:
    Graph(std::istream &in);

    bool hasEdge(size_t nodeA, size_t nodeB) const;
    std::vector<size_t> edgeIntersection(size_t nodeA, size_t nodeB) const;
};

#endif // _MENCI_GRAPH_H
