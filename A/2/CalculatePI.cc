#include <iostream>
#include <iomanip>
#include <random>
#include <cmath>

bool checkError(double value) {
    const static double MAX_ERROR = 0.0001;
    const static double PI = acos(-1);
    return fabs(value - PI) <= MAX_ERROR;
}

double monteCarlo(size_t times) {
    size_t hit = 0;
    auto random = std::default_random_engine(std::random_device()());
    std::uniform_real_distribution<double> dis(0, 1);
    for (size_t i = 0; i < times; i++) {
        double x = dis(random), y = dis(random);
        if (x * x + y * y < 1) hit++;
    }

    return static_cast<double>(hit) / times * 4;
}

int main() {
    const size_t CHECK_TIMES = 20;
    const size_t MONTE_CARLO_TIMES = 50000000;

    size_t satisfiedCount = 0;
    for (size_t i = 0; i < CHECK_TIMES; i++) {
        double res = monteCarlo(MONTE_CARLO_TIMES);

        bool satisfied = checkError(res);
        satisfiedCount += satisfied;

        std::cout << std::setprecision(10) << std::fixed << std::boolalpha
                  << "result = " << res << ", "
                  << "satisfied = " << satisfied << std::endl;
    }

    std::cout << satisfiedCount << "/" << CHECK_TIMES << " runs satisfied" << ", "
              << "satisfied rate = " << static_cast<double>(satisfiedCount) / CHECK_TIMES << std::endl;
}
