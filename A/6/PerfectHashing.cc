#include <iostream>
#include <random>
#include <algorithm>
#include <tuple>
#include <vector>
#include <cassert>
#include <optional>
#include <memory>
#include <chrono>

template <typename T>
class PerfectHashing {
    std::vector<std::tuple<size_t, size_t, std::vector<std::optional<T>>>> data;
    size_t n, m, p, a, b;

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

    size_t hashFunction(const T &x, std::optional<size_t> i = std::nullopt) const {
        if (i) {
            auto &[a, b, v] = data[*i];
            return ((a * x) % p + b) % p % v.size();
        } else {
            return ((a * x) % p + b) % p % m;
        }
    }

    void initUniversalHashing(size_t &a, size_t &b) {
        std::random_device rng;
        do a = rng() % p; while (a == 0);
        b = rng() % p;
    }

    template <typename RAIter>
    bool build(RAIter begin, RAIter end) {
        m = n = std::distance(begin, end);
        p = getPrime(m, 2 * m - 1);

        data.clear();
        data.resize(m);

        initUniversalHashing(a, b);

        std::vector<std::vector<T>> buckets(m);
        for (RAIter it = begin; it != end; it++) {
            size_t i = hashFunction(*it);
            buckets[i].push_back(*it);
        }

        size_t sumSize = 0;
        for (size_t i = 0; i < m; i++) {
            size_t size = buckets[i].size() * buckets[i].size();

            if (size < buckets[i].size()) {
                std::cerr << "rebuild since inner size overflow" << std::endl;
                return false;
            }

            sumSize += size;
        }

        constexpr size_t MAX_SPACE_TIMES = 4;
        if (sumSize > MAX_SPACE_TIMES * n) {
            std::cerr << "rebuild since space > 4n" << std::endl;
            return false;
        }

        for (size_t i = 0; i < m; ) {
            auto &[a, b, v] = data[i];
            initUniversalHashing(a, b);

            v.clear();
            v.resize(buckets[i].size() * buckets[i].size());

            bool rebuild = false;
            for (auto &x : buckets[i]) {
                size_t j = hashFunction(x, i);
                if (v[j]) {
                    rebuild = true;
                    continue;
                }

                v[j] = x;
            }

            if (rebuild) continue;

            i++;
        }

        return true;
    }

public:
    template <typename RAIter>
    PerfectHashing(RAIter begin, RAIter end) {
        while (!build(begin, end));
    }

    template <typename Container>
    PerfectHashing(Container c) : PerfectHashing(std::begin(c), std::end(c)) {}

    bool operator[](const T &x) const {
        size_t i = hashFunction(x);
        
        auto &[a, b, v] = data[i];
        if (a == 0) return false;
        
        size_t j = hashFunction(x, i);
        if (v[j]) return *v[j] == x;

        return false;
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

    std::shared_ptr<PerfectHashing<uint64_t>> h;

    measureTime("Create hash table", [&]() {
        h = std::make_shared<PerfectHashing<uint64_t>>(data);
    });

    measureTime("Check hash table", [&]() {
        for (size_t i = 0; i < DATA_SIZE; i++) {
            assert((*h)[data[i]]);
        }
    });
}
