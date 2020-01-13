//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#include "util/log.h"
#include "util/Clock.h"
#include "Compiler.h"
#include "Errors.h"
#include <fstream>
#include "util/strutil.h"

int main(int argc, char *argv[]){
    bool showTime = false;
    bool outputCFile = false;

    util::Clock clock;
    util::logSetTime(false);
    util::logSetDate(false);

    Compiler compiler;
    compiler.compile("../res/code.txt");
    compiler.compile("../res/lib.txt");
    std::string &output = compiler.generate(true);

    if(errors.errorCount > 0){
        util::logInfo(errors.errorCount, " error(s) occurred");
        return 1;
    }

    if(outputCFile){
        std::ofstream stream("../res/output.c");
        stream << output;
        stream.close();
        system("gcc -O4 -s -Wno-builtin-declaration-mismatch ../res/output.c -o ../res/output.out");
    }else{
        FILE *fd = popen("gcc -O4 -s -Wno-builtin-declaration-mismatch -xc - -o ../res/output.out", "w");
        fwrite(output.c_str(), output.size(), 1, fd);
        pclose(fd);
    }


    if(showTime){
        util::logInfo("compile time: ", clock.round());
    }
    system("../res/output.out");
    if(showTime){
        util::logInfo("run time: ", clock.round());
    }
    return 0;
}

