#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>

void randomizeInPlace(std::vector<int> &v) {
    auto startTime = std::chrono::high_resolution_clock::now();

    std::mt19937 mt19937((std::random_device())());
    for (size_t i = 1; i < v.size(); i++) {
        size_t j = std::uniform_int_distribution(0ul, i - 1)(mt19937);
        std::swap(v[i], v[j]);
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    std::cout << "RANDOMIZE-IN-PLACE costs " << timeElapsed << "ms" << std::endl;
}

void permuteBySorting(std::vector<int> &v) {
    auto startTime = std::chrono::high_resolution_clock::now();

    std::mt19937 mt19937((std::random_device())());
    std::vector<std::pair<decltype(mt19937()), int>> vp(v.size());
    for (size_t i = 0; i < v.size(); i++) {
        vp[i] = std::make_pair(mt19937(), v[i]);
    }

    std::sort(vp.begin(), vp.end());

    for (size_t i = 0; i < v.size(); i++) v[i] = vp[i].second;

    auto endTime = std::chrono::high_resolution_clock::now();
    auto timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    std::cout << "PERMUTE-BY-SORTING costs " << timeElapsed << "ms" << std::endl;
}

std::vector<int> generateArray(size_t n) {
    std::vector<int> v(n);
    std::mt19937 mt19937((std::random_device())());
    for (size_t i = 0; i < n; i++) v[i] = mt19937();
    return v;
}

int main() {
    const size_t N = 10000000;
    std::vector<int> v = generateArray(N), a = v, b = v;

    randomizeInPlace(a);
    permuteBySorting(b);
}
