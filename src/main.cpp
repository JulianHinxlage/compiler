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
#include "print.h"
#include "Compiler.h"

#include <fstream>

int main(int argc, char *argv[]){
    //util::Clock clock;

    Compiler compiler;
    compiler.compile("../res/code.txt");

    //printContext(compiler.ast);
    SemanticChecker semantic;
    semantic.check(compiler.ast);

    CGenerator generator;
    generator.nameTranslation = true;
    std::string output;
    generator.generate(compiler.ast, output);

    std::ofstream stream("../res/output.c");
    stream << output;
    stream.close();

    system("gcc ../res/output.c -o ../res/output.out");
    system("../res/output.out");

    //util::logInfo("time: ", clock.elapsed());
    return 0;
}

