// Dinic's algorithm -- the optimized version of Edmonds-Karp algorithm
// Runs in O(n^2m) while the original algorithm runs in O(nm^2)

#include <iostream>
#include <numeric>
#include <algorithm>
#include <queue>

struct Node {
	struct Edge *firstEdge, *currentEdge;
	int level;
};

std::vector<Node> nodes;

struct Edge {
	Node *from, *to;
	int capacity, flow;
	Edge *next, *reversed;
};

void addEdge(int s, int t, int capacity) {
	nodes[s].firstEdge = new Edge{&nodes[s], &nodes[t], capacity, 0, nodes[s].firstEdge};
	nodes[t].firstEdge = new Edge{&nodes[t], &nodes[s], 0, 0, nodes[t].firstEdge};

	nodes[s].firstEdge->reversed = nodes[t].firstEdge;
	nodes[t].firstEdge->reversed = nodes[s].firstEdge;
}

// Use leveled graph to speed up finding shortest augmenting paths
bool makeLeveledGraph(Node *s, Node *t) {
	for (auto &node : nodes) {
		node.level = 0;
		node.currentEdge = node.firstEdge;
	}

	std::queue<Node *> queue;

	s->level = 1;
	queue.push(s);

	while (!queue.empty()) {
		Node *v = queue.front();
		queue.pop();

		for (Edge *e = v->firstEdge; e; e = e->next) {
			// If (the edge is not full) and (the next node is not visited)
			if (e->flow < e->capacity && e->to->level == 0) {
				e->to->level = v->level + 1;

				// Return when found the destination node (other nodes aren't in shortest paths)
				if (e->to == t) return true;
				queue.push(e->to);
			}
		}
	}

	// No augmenting paths
	return false;
}

// Find an augmenting path from the current node s to destination t, the previous max flow
// of the current path is previousMaxFlow
int findAugmentingPath(Node *s, Node *t, int previousMaxFlow = std::numeric_limits<int>::max()) {
	if (s == t) return previousMaxFlow;

	for (Edge *&e = s->currentEdge; e; e = e->next) {
		// Shortest path in leveled graph
		if (e->flow < e->capacity && e->to->level == s->level + 1) {
			int flow = findAugmentingPath(e->to, t, std::min(previousMaxFlow, e->capacity - e->flow));
			if (flow > 0) {
				e->flow += flow;
				e->reversed->flow -= flow;
				return flow;
			}
		}
	}

	return 0;
}

int maxFlow(int s, int t) {
	int result = 0;
	while (makeLeveledGraph(&nodes[s], &nodes[t])) {
		int flow;
		while ((flow = findAugmentingPath(&nodes[s], &nodes[t])) > 0)
			result += flow;
	}

	return result;
}

int main() {
	int n, m;
	std::cin >> n >> m;

	nodes.resize(n);

	while (m--) {
		int u, v, w;
		std::cin >> u >> v >> w;
		addEdge(u - 1, v - 1, w);
	}

	std::cout << maxFlow(0, n - 1) << std::endl;
}
