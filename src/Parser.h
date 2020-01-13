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

    bool check(const std::string &pattern);
    bool checkAny(const std::string &symbols);
    bool until(const std::string &symbols);
    Token get(int offset);
    void contextStepDown(Context::Type type);
    void contextStepUp();

    bool statement();
    bool type(Expression &e);
    bool variable(const std::string &endSymbols, bool param = false);
    bool ifelse(Expression &e);
    bool loop(Expression &e);
    bool factor(Expression &e, bool hasUnary = false);
    bool postFactor(Expression &e);
    bool expression(Expression &e, const std::string &endSymbols = ";");
    bool precedenceParsing(Expression &op);
    bool function();
    bool block();

    Tokenizer *tokenizer;
    Token token;
    std::shared_ptr<Context> context;
    std::shared_ptr<Context> globalContext;
    util::ArrayList<Token> tokens;
    int tokenIndex;
};


#endif //COMPILER_PARSER_H
