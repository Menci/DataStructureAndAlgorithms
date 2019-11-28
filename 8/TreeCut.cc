#include <iostream>
#include <limits>
#include <algorithm>
#include <vector>

const int MAXN = 1000;

struct Node {
    std::vector<struct Edge> edges;
    bool visited;
} nodes[MAXN + 1];

struct Edge {
    Node &from, &to;
    int weight;
};

inline void addEdge(int from, int to, int weight) {
    nodes[from].edges.push_back(Edge{nodes[from], nodes[to], weight});
    nodes[to].edges.push_back(Edge{nodes[to], nodes[from], weight});
}

inline int dfs(Node &v) {
    v.visited = true;

    int sum = 0;
    bool isLeaf = true;
    for (Edge &e : v.edges) {
        if (!e.to.visited) {
            sum += std::min(dfs(e.to), e.weight);

            isLeaf = false;
        }
    }

    if (isLeaf) return std::numeric_limits<int>::max();

    return sum;
}

int main() {
    for (int n, root; (std::cin >> n >> root) && n; ) {
        for (int i = 0; i < n - 1; i++) {
            int u, v, w;
            std::cin >> u >> v >> w;
            addEdge(u, v, w);
        }

        int ans = dfs(nodes[root]);
        if (ans == std::numeric_limits<int>::max()) ans = 0;

        std::cout << ans << std::endl;

        for (int i = 1; i <= MAXN; i++) {
            nodes[i].visited = false;
            nodes[i].edges.clear();
        }
    }
}
