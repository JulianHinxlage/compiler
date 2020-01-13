//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#include "util/log.h"
#include "Compiler.h"
#include "Errors.h"
#include <fstream>
#include "util/strutil.h"
#include <iostream>

const char *helpMsg = &R"(
-o <output-file>   : output of compilation
-r                 : run program
-c <c-output-file> : output c code
-h                 : show help
)"[1];

int main(int argc, char *argv[]){
    bool useOutputCFile = false;
    bool runProgram = false;

    util::logSetTime(false);
    util::logSetDate(false);

    std::string inputFiles = "";
    std::string outputFile = "a.out";
    std::string outputCFile = "a.c";

    //argument parsing
    for(int i = 1; i < argc;i++){
        std::string arg = argv[i];
        if(arg == "-o"){
            i++;
            if(i >= argc){
                util::logInfo("expected argument after -o flag");
            }else{
                outputFile = argv[i];
            }
        }else if(arg == "-c"){
            useOutputCFile = true;
            i++;
            if(i >= argc){
                util::logInfo("expected argument after -c flag");
            }else{
                outputCFile = argv[i];
            }
        }else if(arg == "-r"){
            runProgram = true;
        }else if(arg == "-h" || arg == "--help" || arg == "-help"){
            std::cout << helpMsg;
            return 0;
        }
        else{
            if(!inputFiles.empty()){
                inputFiles += " ";
            }
            inputFiles += arg;
        }
    }

    if(inputFiles.empty()){
        util::logInfo("no source files");
        return 0;
    }

    //compile
    Compiler compiler;
    for(auto &i : util::split(inputFiles)){
        compiler.compile(i);
    }
    std::string &output = compiler.generate(true);

    if(errors.errorCount > 0){
        util::logInfo(errors.errorCount, " error(s) occurred");
        return 1;
    }

    if(output.empty()){
        return 0;
    }

    //compile c code
    if(useOutputCFile){
        std::ofstream stream(outputCFile);
        stream << output;
        stream.close();
        system((std::string("gcc -O4 -s -Wno-builtin-declaration-mismatch ") + outputCFile + " -o " + outputFile).c_str());
    }else{
        FILE *fd = popen((std::string("gcc -O4 -s -Wno-builtin-declaration-mismatch -xc - -o ") + outputFile).c_str(), "w");
        fwrite(output.c_str(), output.size(), 1, fd);
        pclose(fd);
    }

    //run
    if(runProgram){
        system((std::string("./") + outputFile).c_str());
    }
    return 0;
}

