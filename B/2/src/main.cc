#include <cstdint>
#include <iostream>
#include <fstream>
#include <memory>

#include "TerminalColor/TerminalColor.h"

#include "Arguments.h"
#include "Utility.h"
#include "Graph.h"

void solveEdgeQueries(const Graph &graph, std::istream &in, std::ostream &out) {
    std::vector<std::pair<size_t, size_t>> questions;
    const double timeLoadingQueries = measureTime([&]() {
        for (size_t u, v; in >> u >> v; ) questions.push_back({u, v});
    });

    std::cerr << TerminalColor::Bold
              << TerminalColor::ForegroundGreen
              << "OK: "
              << TerminalColor::Reset
              << "Edge queries loaded in "
              << TerminalColor::Bold
              << timeLoadingQueries
              << "s"
              << TerminalColor::Reset
              << "."
              << std::endl;
    
    std::vector<int> answers;
    answers.reserve(questions.size());

    const double timeSolve = measureTime([&]() {
        for (auto [u, v] : questions) answers.push_back(graph.hasEdge(u, v));
    });
    
    std::cerr << TerminalColor::Bold
              << TerminalColor::ForegroundGreen
              << "OK: "
              << TerminalColor::Reset
              << "Edge queries solved in "
              << TerminalColor::Bold
              << timeSolve
              << "s"
              << TerminalColor::Reset
              << "."
              << std::endl;

    const double timeStoringResult = measureTime([&]() {
        for (auto answer : answers) out << answer << std::endl;
    });

    std::cerr << TerminalColor::Bold
              << TerminalColor::ForegroundGreen
              << "OK: "
              << TerminalColor::Reset
              << "Edge queries result stored in "
              << TerminalColor::Bold
              << timeStoringResult
              << "s"
              << TerminalColor::Reset
              << "."
              << std::endl;
}

void solveTriangleQueries(const Graph &graph, std::istream &in, std::ostream &out) {
    std::vector<std::pair<size_t, size_t>> questions;
    const double timeLoadingQueries = measureTime([&]() {
        for (size_t u, v; in >> u >> v; ) questions.push_back({u, v});
    });

    std::cerr << TerminalColor::Bold
              << TerminalColor::ForegroundGreen
              << "OK: "
              << TerminalColor::Reset
              << "Triangle queries loaded in "
              << TerminalColor::Bold
              << timeLoadingQueries
              << "s"
              << TerminalColor::Reset
              << "."
              << std::endl;
    
    std::vector<std::vector<size_t>> answers;
    answers.resize(questions.size());

    const double timeSolve = measureTime([&]() {
        for (size_t i = 0; i < questions.size(); i++) {
            auto [u, v] = questions[i];
            if (graph.hasEdge(u, v))
                answers[i] = graph.edgeIntersection(u, v);
        }
    });
    
    std::cerr << TerminalColor::Bold
              << TerminalColor::ForegroundGreen
              << "OK: "
              << TerminalColor::Reset
              << "Triangle queries solved in "
              << TerminalColor::Bold
              << timeSolve
              << "s"
              << TerminalColor::Reset
              << "."
              << std::endl;

    const double timeStoringResult = measureTime([&]() {
        for (size_t i = 0; i < questions.size(); i++) {
            auto [u, v] = questions[i];
            out << u << " " << v << std::endl;

            for (auto w : answers[i])
                out << w << std::endl;
        }
    });

    std::cerr << TerminalColor::Bold
              << TerminalColor::ForegroundGreen
              << "OK: "
              << TerminalColor::Reset
              << "Triangle queries result stored in "
              << TerminalColor::Bold
              << timeStoringResult
              << "s"
              << TerminalColor::Reset
              << "."
              << std::endl;

}

int main(int argc, char *argv[]) {
    Arguments arguments = parseArguments(argc, argv);

    std::ifstream graphFile(arguments.graph);
    if (!graphFile) {
        error("Failed to open graph file \"" + arguments.graph + "\".");
    }

    std::unique_ptr<Graph> graph;
    const double timeLoadingGraph = measureTime([&]() {
        graph = std::make_unique<Graph>(graphFile);
    });

    std::cerr << TerminalColor::Bold
              << TerminalColor::ForegroundGreen
              << "OK: "
              << TerminalColor::Reset
              << "Graph loaded in "
              << TerminalColor::Bold
              << timeLoadingGraph
              << "s"
              << TerminalColor::Reset
              << "."
              << std::endl;

    if (!arguments.edgeQueries.empty()) {
        std::ifstream in(arguments.edgeQueries);
        if (!in) {
            error("Failed to open edge queries file \"" + arguments.edgeQueries + "\".");
        }

        std::ofstream out(arguments.edgeQueriesResult);
        if (!out) {
            error("Failed to open edge queries result file \"" + arguments.edgeQueriesResult + "\".");
        }

        solveEdgeQueries(*graph, in, out);
    }

    if (!arguments.triangleQueries.empty()) {
        std::ifstream in(arguments.triangleQueries);
        if (!in) {
            error("Failed to open triangle queries file \"" + arguments.triangleQueries + "\".");
        }

        std::ofstream out(arguments.triangleQueriesResult);
        if (!out) {
            error("Failed to open triangle queries result file \"" + arguments.triangleQueriesResult + "\".");
        }

        solveTriangleQueries(*graph, in, out);
    }
}
