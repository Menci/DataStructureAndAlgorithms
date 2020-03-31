#include <iostream>
#include <fstream>
#include <cstdint>
#include <cassert>
#include <random>
#include <functional>
#include <chrono>
#include <algorithm>
#include <vector>

template <typename RAIter, typename Compare>
RAIter partition(RAIter begin, RAIter end, RAIter pivot, Compare compare) {
	std::swap(*pivot, *(end - 1));

	decltype(*begin) pivotValue = *(end - 1);
	RAIter curr = begin;
	for (RAIter it = begin; it != end; it++) {
		if (compare(*it, pivotValue)) {
			std::swap(*it, *curr++);
		}
	}

	std::swap(*curr, *(end - 1));
	return curr;
}

template <typename RAIter, typename Compare = std::less<decltype(*std::declval<RAIter>())>>
auto &randomSelect(RAIter begin, RAIter nth, RAIter end, Compare compare = Compare()) {
	if (begin == end - 1) return *begin;

	static std::mt19937 rng((std::random_device()()));

	std::uniform_int_distribution<size_t> rd(0, end - begin - 1);
	RAIter pivot = begin + rd(rng);

	pivot = partition(begin, end, pivot, compare);

	if (pivot == nth) return *pivot;
	else if (nth < pivot) return randomSelect(begin, nth, pivot);
	else return randomSelect(pivot + 1, nth, end);
}

template <typename RAIter, typename Compare = std::less<decltype(*std::declval<RAIter>())>>
auto &quickSelect(RAIter begin, RAIter nth, RAIter end, Compare compare = Compare()) {
	const size_t MIN_LEN = 141;
	if (end - begin < MIN_LEN) {
		std::sort(begin, end, compare);
		return *nth;
	}

	size_t n = end - begin, groups = n / 5 + !!(n % 5);
	RAIter curr = begin;
	for (size_t i = 0; i < groups; i++) {
		RAIter gbegin = begin + i * 5, gend = std::min(begin + (i + 1) * 5, end);
		std::sort(gbegin, gend, compare);
		RAIter gmid = gbegin + (gend - gbegin - 1) / 2;
		std::swap(*curr++, *gmid);
	}

	RAIter tbegin = begin, tend = curr, pivot = tbegin + (tend - tbegin - 1) / 2;
	quickSelect(tbegin, pivot, tend, compare);
	pivot = partition(begin, end, pivot, compare);

	if (pivot == nth) return *pivot;
	else if (nth < pivot) return randomSelect(begin, nth, pivot);
	else return randomSelect(pivot + 1, nth, end);
}

std::vector<uint64_t> generateRandomData(size_t n) {
	static std::mt19937 rng((std::random_device()()));
	std::vector<uint64_t> v;
	for (size_t i = 0; i < n; i++) v.push_back(rng());
	return v;
}

template <typename RAIter, typename Compare = std::less<decltype(*std::declval<RAIter>())>>
auto &sortSelect(RAIter begin, RAIter nth, RAIter end, Compare compare = Compare()) {
	std::sort(begin, end, compare);
	return *nth;
}

template <typename RAIter, typename Compare = std::less<decltype(*std::declval<RAIter>())>>
auto &stdSelect(RAIter begin, RAIter nth, RAIter end, Compare compare = Compare()) {
	std::nth_element(begin, nth, end, compare);
	return *nth;
}

void measureTime(std::string actionName, std::function<void ()> function) {
    auto startTime = std::chrono::high_resolution_clock::now();

    function();

    auto endTime = std::chrono::high_resolution_clock::now();
    double timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    std::cerr << actionName << " costs " << timeElapsed / 1000 << "s" << std::endl;
}

int main() {
	std::ifstream fileData("input.txt"), fileAnswer("answer.txt");
	std::vector<long> data;
	for (long x; fileData >> x; data.push_back(x));

	std::vector<std::pair<size_t, long>> questions;
	for (std::pair<size_t, long> p; fileAnswer >> p.first >> p.second; questions.push_back(p));

	measureTime("sortSelect  ", [&]() {
		auto v0 = data;
		for (const auto &p : questions) assert(sortSelect(data.begin(), data.begin() + p.first - 1, data.end()) == p.second);
	});

	measureTime("randomSelect", [&]() {
		auto v0 = data;
		for (const auto &p : questions) assert(randomSelect(data.begin(), data.begin() + p.first - 1, data.end()) == p.second);
	});

	measureTime("quickSelect ", [&]() {
		auto v0 = data;
		for (const auto &p : questions) assert(quickSelect(data.begin(), data.begin() + p.first - 1, data.end()) == p.second);
	});

	measureTime("stdSelect   ", [&]() {
		auto v0 = data;
		for (const auto &p : questions) assert(stdSelect(data.begin(), data.begin() + p.first - 1, data.end()) == p.second);
	});
}
