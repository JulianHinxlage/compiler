//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "Tokenizer.h"
#include "Context.h"

class Parser {
public:
    Parser();
    void parse(Tokenizer &tokenizer);
private:
    bool step();

    bool statement();
    bool parameter();
    bool expresion();
    bool function();

    Tokenizer *tokenizer;
    Token token;
    std::shared_ptr<Context> context;
public:
    std::shared_ptr<Context> globalContext;
};


#endif //COMPILER_PARSER_H
