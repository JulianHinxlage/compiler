//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#include "util/strutil.h"
#include "util/log.h"
#include "Tokenizer.h"
#include "util/Clock.h"
#include "Parser.h"
#include "SemanticChecker.h"
#include "CGenerator.h"

#include <fstream>

int main(int argc, char *argv[]){
    util::Clock clock;
    Tokenizer tokenizer;
    Parser parser;

    tokenizer.setDefault();
    tokenizer.setFile("../res/code.txt");

    auto contextTree = parser.parse(tokenizer);

    SemanticChecker semantic;
    semantic.check(contextTree);

    CGenerator generator;
    std::string output;
    generator.generate(contextTree, output);

    std::ofstream stream("../res/output.c");
    stream << output;
    stream.close();

    util::logInfo("time: ", clock.elapsed());

    return 0;
}
