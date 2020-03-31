#include <iostream>
#include <random>
#include <algorithm>
#include <vector>
#include <cassert>
#include <optional>
#include <functional>
#include <memory>
#include <chrono>

template <typename T, size_t M>
class CuckooHashing {
    std::vector<std::optional<T>> t[2];
    std::function<size_t (size_t)> f[2];

    // Get random prime number in [min, max]
    static size_t getPrime(size_t min, size_t max) {
        std::vector<bool> isNotPrime(max + 1);
        std::vector<size_t> primes;

        // Euler sieve
        isNotPrime[0] = isNotPrime[1] = true;
        for (size_t i = 2; i <= max; i++) {
            if (!isNotPrime[i]) primes.push_back(i);
            for (size_t p : primes) {
                size_t x = i * p;
                if (x > max) break;

                isNotPrime[x] = true;
                if (i % p == 0) break;
            }
        }

        // Get the random range
        std::vector<size_t>::const_iterator l = std::lower_bound(primes.begin(), primes.end(), min),
                                            r = std::upper_bound(primes.begin(), primes.end(), max);
        
        std::random_device rng;
        std::uniform_int_distribution<> dis(0, r - l);

        return *(dis(rng) + l);
    }

    void init() {
        t[0].clear();
        t[1].clear();
        t[0].resize(M);
        t[1].resize(M);

        std::random_device rng;
        size_t p0 = getPrime(M, 2 * M - 1),
               a0 = std::uniform_int_distribution<>(1, p0 - 1)(rng),
               b0 = std::uniform_int_distribution<>(0, p0 - 1)(rng),
               p1 = getPrime(M, 2 * M - 1),
               a1 = std::uniform_int_distribution<>(1, p1 - 1)(rng),
               b1 = std::uniform_int_distribution<>(0, p1 - 1)(rng);
        
        f[0] = [p0, a0, b0](const T &x) {
            return ((a0 * x) % p0 + b0) % p0 % M;
        };
        f[1] = [p1, a1, b1](const T &x) {
            return ((a1 * x) % p1 + b1) % p1 % M;
        };
    }

    void rebuild() {
        std::cerr << "rebuild" << std::endl;

        std::vector<T> values;
        for (auto &item : t[0]) if (item) values.push_back(std::move(*item));
        for (auto &item : t[1]) if (item) values.push_back(std::move(*item));

        init();

        for (auto &item : values) insert(std::move(item));
    }

    bool insert(const T &x, size_t which, size_t depth) {
        if (1ULL << depth > M) return false;

        size_t i = f[which](x);
        if (t[which][i]) {
            T y = *t[which][i];
            if (!insert(y, 1 - which, depth + 1)) return false;
        }

        t[which][i] = x;
        return true;
    }

public:
    CuckooHashing() {
        init();
    }

    void insert(const T &x) {
        while (!insert(x, 0, 0)) rebuild();
    }

    bool find(const T &x) {
        size_t i0 = f[0](x), i1 = f[1](x);
        return (t[0][i0] && *t[0][i0] == x) || (t[1][i1] && *t[1][i1] == x);
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
    const size_t DATA_SIZE = 5000000;
    std::vector<uint64_t> data = generateRandomData(DATA_SIZE);
    std::cout << "Random data generated" << std::endl;

    CuckooHashing<uint64_t, DATA_SIZE * 2> h;

    measureTime("Insert data", [&]() {
        for (auto x : data) h.insert(x);
    });

    measureTime("Check hash table", [&]() {
        for (size_t i = 0; i < DATA_SIZE; i++) {
            assert(h.find(data[i]));
        }
    });
}
