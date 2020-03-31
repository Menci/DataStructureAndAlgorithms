#ifndef _MENCI_GRAPH_ARGUMENTS_H
#define _MENCI_GRAPH_ARGUMENTS_H

#include <string>
#include <vector>

struct Arguments {
    std::string graph;
    std::string edgeQueries;
    std::string edgeQueriesResult;
    std::string triangleQueries;
    std::string triangleQueriesResult;
};

Arguments parseArguments(int argc, char *argv[]);

#endif // _MENCI_GRAPH_ARGUMENTS_H
