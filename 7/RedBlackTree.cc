#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <functional>
#include <chrono>
#include <cassert>

template <typename T>
class RedBlackTree {
    enum Relation {
        L = 0,
        R = 1
    };

    struct Node {
        Node *child[2], *parent, **root;
        enum Color {
            Red, Black
        } color;
        T value;
        size_t blackHeight;

        Node(const T &value, Node *parent, Color color, Node **root) : parent(parent), root(root), color(color), value(value), blackHeight(1) {
            child[Relation::L] = child[Relation::R] = nullptr;
        }

        void calcBlackHeight() {
            const Node *ch = child[Relation::L] ? child[Relation::L] : child[Relation::R];
            blackHeight = ch ? ch->blackHeight + (ch->color == Color::Black) : 1;
        }

        Relation which() const {
            assert(parent != nullptr);
            return this == parent->child[Relation::R] ? Relation::R : Relation::L;
        }

        Node *grandparent() {
            return !parent ? nullptr : parent->parent;
        }

        Node *uncle() {
            return !grandparent() ? nullptr : grandparent()->child[parent->which() ^ 1];
        }

        const Node *nearest(int direction) const {
            if (child[direction]) {
                const Node *v = child[direction];
                while (v->child[direction ^ 1]) v = v->child[direction ^ 1];
                return v;
            } else {
                const Node *v = this;
                while (v->parent)
                    if (v->which() == (direction ^ 1)) return v->parent;
                    else v = v->parent;
                return nullptr;
            }
        }

        void rotateUp() {
            Node *oldParent = parent;
            Relation relation = which();

            // self <--> grand parent
            parent = oldParent->parent;
            if (oldParent->parent) oldParent->parent->child[oldParent->which()] = this;

            // child <--> parent
            if (child[relation ^ 1]) child[relation ^ 1]->parent = oldParent;
            oldParent->child[relation] = child[relation ^ 1];

            // parent <--> self
            oldParent->parent = this;
            child[relation ^ 1] = oldParent;

            if (!parent) *root = this;
        }

        void validate() {
            if (child[Relation::L]) child[Relation::L]->validate();
            if (child[Relation::R]) child[Relation::R]->validate();

            if (child[Relation::L]) assert(blackHeight == child[Relation::L]->blackHeight + (child[Relation::L]->color == Color::Black));
            if (child[Relation::R]) assert(blackHeight == child[Relation::R]->blackHeight + (child[Relation::R]->color == Color::Black));
            if (!child[Relation::L] && !child[Relation::R]) assert(blackHeight == 1);

            if (color == Node::Red) {
                if (child[Relation::L]) assert(child[Relation::L]->color == Node::Black);
                if (child[Relation::R]) assert(child[Relation::R]->color == Node::Black);
            }
        }
    } *root;

    const Node *find(const T &x) const {
        const Node *v = root;
        while (v && x != v->value) v = v->child[!(x < v->value)];
        return v;
    }

public:
    RedBlackTree() : root(nullptr) {}

    void insert(const T &x) {
        Node **p = &root, *parent = nullptr;
        while (Node *v = *p) {
            parent = v;
            p = &v->child[!(x < v->value)];
        }
        *p = new Node(x, parent, Node::Color::Red, &root);

        insertFixUp(*p);
        for (Node *v = *p; v; v = v->parent) v->calcBlackHeight();
    }

    void insertFixUp(Node *v) {
        if (v == root) {
            v->color = Node::Color::Black;
            return;
        }

        if (v->parent->color == Node::Color::Black) return;

        Node *uncle = v->uncle();
        if (uncle && uncle->color == Node::Color::Red) {
            v->parent->color = Node::Color::Black;
            uncle->color = Node::Color::Black;
            v->grandparent()->color = Node::Color::Red;
            insertFixUp(v->grandparent());
        } else {
            Node *u;
            if (v->which() != v->parent->which()) {
                v->rotateUp();
                u = v;
            } else u = v->parent;

            u->color = Node::Color::Black;
            u->parent->color = Node::Color::Red;
            u->rotateUp();
        }
    }

    const T *predecessor(const T &x) const {
        const Node *v = find(x);
        if (!v) return nullptr;

        const Node *u = v->nearest(0);
        return u ? &u->value : nullptr;
    }

    const T *successor(const T &x) const {
        const Node *v = find(x);
        if (!v) return nullptr;

        const Node *u = v->nearest(1);
        return u ? &u->value : nullptr;
    }

    const T *minimum() const {
        const Node *v = root;
        while (v && v->child[Relation::L]) v = v->child[Relation::L];
        return v ? &v->value : nullptr;
    }

    const T *maximum() const {
        const Node *v = root;
        while (v && v->child[Relation::R]) v = v->child[Relation::R];
        return v ? &v->value : nullptr;
    }

    void validate() const {
        if (root) root->validate();
    }
};

std::vector<uint64_t> generateRandomData(size_t n) {
	static std::mt19937_64 rng((std::random_device()()));
	std::vector<uint64_t> v;
	for (size_t i = 0; i < n; i++) v.push_back(rng());
	return v;
}

void measureTime(std::string actionName, std::function<void ()> function) {
    auto startTime = std::chrono::high_resolution_clock::now();

    function();

    auto endTime = std::chrono::high_resolution_clock::now();
    double timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    std::cerr << actionName << " costs " << timeElapsed / 1000 << "s" << std::endl;
}

int main() {
    const size_t DATA_SIZE = 500000;
    std::vector<uint64_t> data = generateRandomData(DATA_SIZE);

    RedBlackTree<uint64_t> tree;

    measureTime("Insert data into tree", [&]() {
        for (auto x : data) tree.insert(x);
    });
    
    measureTime("Validate tree", [&]() {
        tree.validate();
    });

    std::sort(data.begin(), data.end());
    measureTime("Find predecessor and successor (for all elements)", [&]() {
        for (size_t i = 1; i < data.size(); i++) {
            assert(*tree.predecessor(data[i]) == data[i - 1]);
        }

        for (size_t i = 0; i < data.size() - 1; i++) {
            assert(*tree.successor(data[i]) == data[i + 1]);
        }
    });

    measureTime("Find minimum and maximum", [&]() {
        assert(*tree.minimum() == data.front());
        assert(*tree.maximum() == data.back());
    });
}
