#include <iostream>

#include "MinHeap.h"

int main() {
    MinHeap<int> heap;
    for (int x; std::cin >> x; heap.push(x));
    while (!heap.empty()) {
        std::cout << heap.top() << std::endl;
        heap.pop();
    }
}
