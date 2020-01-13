//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#include "util/log.h"
#include "util/Clock.h"
#include "Compiler.h"
#include <fstream>

int main(int argc, char *argv[]){
    //util::Clock clock;

    Compiler compiler;
    compiler.compile("../res/code.txt");
    compiler.compile("../res/lib.txt");
    std::string &output = compiler.generate(true);

    std::ofstream stream("../res/output.c");
    stream << output;
    stream.close();

    system("gcc ../res/output.c -o ../res/output.out");
    system("../res/output.out");

    //util::logInfo("time: ", clock.elapsed());
    return 0;
}

