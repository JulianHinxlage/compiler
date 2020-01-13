//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#ifndef COMPILER_COMPILER_H
#define COMPILER_COMPILER_H

#include "Parser.h"

class Compiler {
public:
    Tokenizer tokenizer;
    Parser parser;
    std::shared_ptr<Context> ast;
    util::ArrayList<std::string> files;

    Compiler();
    void compile(const std::string &file);
    std::string &generate(bool obfuscate = false);
private:
    void setFile(const std::string &file);
    std::string output;
};


#endif //COMPILER_COMPILER_H
