#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <chrono>
#include <functional>
#include <cmath>
#include <cstdint>

void radixSort(std::vector<std::string> &input) {
    size_t n = input.size();

    // The largest possiable radix
    size_t radixLength = n < 10 ? 1 : (std::to_string(n).length() - 1),
           radix = pow(10, radixLength);

    size_t maxLength = 0;
    for (const auto &s : input) maxLength = std::max(maxLength, s.length());

    size_t digitCount = maxLength / radixLength + !!(maxLength % radixLength);
    // Splitted input, in each array from lowest digit to highest
    std::vector<std::vector<uint32_t>> splitted(n);
    for (size_t i = 0; i < n; i++) {
        const auto &s = input[i];
        auto &v = splitted[i];
        for (size_t i = 0; i < maxLength; i += radixLength) {
            if (i >= s.length())
                v.push_back(0);
            else if (i + radixLength > s.length())
                v.push_back(std::stoul(s.substr(0, s.length() % radixLength)));
            else
                v.push_back(std::stoul(s.substr(s.length() - i - radixLength, radixLength)));
        }
    }

    std::vector<size_t> counter(radix + 1), rank(n), atRank(n);
    std::iota(atRank.begin(), atRank.end(), 0);
    for (size_t i = 0; i < digitCount; i++) {
        std::fill(counter.begin(), counter.end(), 0);

        for (size_t j = 0; j < n; j++) {
            uint32_t value = splitted[atRank[j]][i];
            counter[value + 1]++;
        }

        for (size_t j = 1; j < radix + 1; j++) counter[j] += counter[j - 1];

        for (size_t j = 0; j < n; j++) {
            uint32_t value = splitted[atRank[j]][i];
            rank[atRank[j]] = counter[value]++;
        }

        for (size_t j = 0; j < n; j++) atRank[rank[j]] = j;
    }

    std::vector<std::string> result(n);
    for (size_t j = 0; j < n; j++) result[j] = std::move(input[atRank[j]]);

    input = std::move(result);
}

void measureTime(std::string actionName, std::function<void ()> function) {
    auto startTime = std::chrono::high_resolution_clock::now();

    function();

    auto endTime = std::chrono::high_resolution_clock::now();
    double timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    std::cerr << actionName << " costs " << timeElapsed / 1000 << "s" << std::endl;
}

int main() {
    std::vector<std::string> numbers;
    measureTime("Input", [&]() {
        // Speed up the stream IO
        std::ios::sync_with_stdio(false);
        std::cin.tie(NULL);
        for (std::string temp; std::cin >> temp; numbers.push_back(temp));
    });

    measureTime("Radix sort", [&]() {
        radixSort(numbers);
    });

    measureTime("Output", [&]() {
        size_t outputLength = 0;
        for (const auto &number : numbers) outputLength += number.length() + 1;
        
        std::string data;
        data.reserve(outputLength);
        for (const auto &number : numbers)  data += number + '\n';

        fwrite(data.data(), data.length(), 1, stdout);
    });
}
