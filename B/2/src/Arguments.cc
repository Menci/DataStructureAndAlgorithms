#include "Arguments.h"

#include "ArgumentParser/ArgumentParser.h"

Arguments parseArguments(int argc, char *argv[]) {
    Arguments arguments;
    ArgumentParser(argc, argv)
        .setProgramDescription("The solution program of the graph connectivity problems.")
        .addPositional("graph", "Your graph dataset. (e.g. YouTube-u.txt)",
                       ArgumentParser::stringParser(arguments.graph))
        .addOption("edge-queries", "e",
                   "file",
                   "The query input of the edge problem.",
                   ArgumentParser::stringParser(arguments.edgeQueries),
                   true, "")
        .addOption("edge-queries-result", "E",
                   "file",
                   "The file to write answer of the edge problem. Ignored if -e is omitted.",
                   ArgumentParser::stringParser(arguments.edgeQueriesResult),
                   true, "")
        .addOption("triangle-queries", "t",
                   "file",
                   "The query input of the triangle problem.",
                   ArgumentParser::stringParser(arguments.triangleQueries),
                   true, "")
        .addOption("triangle-queries-result", "T",
                   "file",
                   "The file to write answer of the triangle problem. Ignored if -t is omitted.",
                   ArgumentParser::stringParser(arguments.triangleQueriesResult),
                   true, "")
        .parse();
    return arguments;
}
