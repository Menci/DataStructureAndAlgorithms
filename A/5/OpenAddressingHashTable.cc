#include <iostream>
#include <vector>
#include <cstdint>
#include <exception>
#include <cassert>
#include <random>
#include <functional>
#include <chrono>

template <typename TKey, typename TValue, typename HashFunction, size_t SIZE>
class OpenAddressingHashTable {
    struct HashItem {
        TKey key;
        TValue value;
        bool nonEmpty;
    };
    std::vector<HashItem> data;

    HashItem &find(const TKey &key, bool emptyStatus) {
    }

public:
    OpenAddressingHashTable() : data(SIZE) {}

    template <typename TValueRef>
    void set(const TKey &key, TValueRef &&value) {
        for (size_t i = 0; i < SIZE; i++) {
            size_t hash = HashFunction()(key, i, SIZE);
            if (!data[hash].nonEmpty) {
                data[hash].key = key;
                data[hash].value = std::forward<TValueRef>(value);
                data[hash].nonEmpty = true;
                return;
            }
        }
        throw std::logic_error("hash table full");
    }

    const TValue &get(const TKey &key) {
        for (size_t i = 0; i < SIZE; i++) {
            size_t hash = HashFunction()(key, i, SIZE);
            if (!data[hash].nonEmpty) {
                throw std::logic_error("key not found");
            } else if (data[hash].key == key) return data[hash].value;
        }
        throw std::logic_error("key not found");
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
    const size_t TABLE_SIZE = 1u << 20, DATA_SIZE = 1000000;
    std::vector<uint64_t> data = generateRandomData(DATA_SIZE);
    std::cout << "Random data generated" << std::endl;

    {
        struct LinearProbing {
            size_t operator()(uint64_t key, size_t i, size_t hashSize) const {
                return (key % hashSize + i) % hashSize;
            }
        };
        OpenAddressingHashTable<uint64_t, uint64_t, LinearProbing, TABLE_SIZE> hashTable;
        measureTime("LinearProbing", [&]() {
            for (auto x : data) hashTable.set(x, x + x);
            for (auto x : data) assert(hashTable.get(x) == x + x);
        });
    }

    {
        struct QuadraticProbing {
            size_t operator()(uint64_t key, size_t i, size_t hashSize) const {
                return ((key % hashSize + 1 * i) % hashSize + 2 * i * i) % hashSize;
            }
        };
        OpenAddressingHashTable<uint64_t, uint64_t, QuadraticProbing, TABLE_SIZE> hashTable;
        measureTime("QuadraticProbing", [&]() {
            for (auto x : data) hashTable.set(x, x + x);
            for (auto x : data) assert(hashTable.get(x) == x + x);
        });
    }

    {
        struct DoubleHashing {
            size_t operator()(uint64_t key, size_t i, size_t hashSize) const {
                const uint64_t MOD = 998244353;
                return (key % hashSize + i * (key % MOD * 2 + 1)) % hashSize;
            }
        };
        OpenAddressingHashTable<uint64_t, uint64_t, DoubleHashing, TABLE_SIZE> hashTable;
        measureTime("DoubleHashing", [&]() {
            for (auto x : data) hashTable.set(x, x + x);
            for (auto x : data) assert(hashTable.get(x) == x + x);
        });
    }
}
