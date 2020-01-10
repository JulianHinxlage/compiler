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
    std::shared_ptr<Context>  parse(Tokenizer &tokenizer);
private:
    bool next();
    bool prev();

    bool statement();
    bool parameter();
    bool expresion();
    bool function();
    bool block();

    bool check(const std::string &pattern);
    bool until(const std::string &symbols);
    Token get(int offset);
    void contextStepDown(Context::Type type);
    void contextStepUp();

    Tokenizer *tokenizer;
    Token token;
    std::shared_ptr<Context> context;
    std::shared_ptr<Context> globalContext;
    util::ArrayList<Token> tokens;
    int tokenIndex;
};


#endif //COMPILER_PARSER_H
