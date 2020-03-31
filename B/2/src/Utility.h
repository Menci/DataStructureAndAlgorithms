#ifndef _MENCI_GRAPH_UTILITY_H
#define _MENCI_GRAPH_UTILITY_H

#include <cstdlib>
#include <iostream>
#include <functional>
#include <chrono>

#include "TerminalColor/TerminalColor.h"

void error(const std::string &message) {
    std::cerr << TerminalColor::Bold
              << TerminalColor::ForegroundRed
              << "Error: "
              << TerminalColor::Reset
              << message
              << std::endl;
    exit(1);
}

double measureTime(std::function<void ()> function) {
    auto startTime = std::chrono::high_resolution_clock::now();

    function();

    auto endTime = std::chrono::high_resolution_clock::now();
    double timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    return timeElapsed / 1000;
}

#endif // _MENCI_GRAPH_UTILITY_H
