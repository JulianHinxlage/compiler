//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#include "util/strutil.h"
#include "util/log.h"
#include "Tokenizer.h"
#include "util/Clock.h"
#include "Parser.h"
#include "print.h"

int main(int argc, char *argv[]){
    util::Clock clock;
    auto code = util::readFile("../res/code.txt");

    Tokenizer tokenizer;
    tokenizer.setDefault();
    tokenizer.setSource(code);

    Parser parser;
    auto context = parser.parse(tokenizer);

    printContext(context);
    util::logInfo("time: ", clock.elapsed());

    return 0;
}
