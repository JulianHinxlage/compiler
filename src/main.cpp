//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#include "util/strutil.h"
#include "util/log.h"
#include "Tokenizer.h"
#include "util/Clock.h"
#include "Parser.h"
#include "print.h"
#include "SemanticChecker.h"

int main(int argc, char *argv[]){
    util::Clock clock;
    Tokenizer tokenizer;
    Parser parser;

    tokenizer.setDefault();
    tokenizer.setFile("../res/code.txt");

    auto contextTree = parser.parse(tokenizer);

    printContext(contextTree);
    SemanticChecker semantic;
    semantic.check(contextTree);
    util::logInfo("time: ", clock.elapsed());

    return 0;
}
