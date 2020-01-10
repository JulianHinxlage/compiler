//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#ifndef COMPILER_KONTEXT_H
#define COMPILER_KONTEXT_H

#include "util/ArrayList.h"
#include "Token.h"
#include <string>
#include <memory>

class Context;

class Variable{
public:
    std::string name;
    std::string type;
    std::string mods;
};

class Expression{
public:
    Token token;
    util::ArrayList<Expression> expressions;
    std::shared_ptr<Context> context = nullptr;

    enum Type{
        NONE = 0,
        CONST = 1,
        VAR = 2,
        UNARY_OPERATOR = 3,
        OPERATOR = 4,
        BLOCK = 5,
        CALL = 6,
        OBJ_CALL = 7
    };

    Type type;

    Expression(){
        type = NONE;
    }

    Expression(Type type, const Token &token){
        this->type = type;
        this->token = token;
    }
};

class Context {
public:
    std::string name;
    std::string returnType;
    util::ArrayList<Variable> parameter;

    util::ArrayList<Variable> variables;
    util::ArrayList<std::shared_ptr<Context>> contexts;
    std::shared_ptr<Context> parentContext = nullptr;
    util::ArrayList<Expression> expressions;

    enum Type{
        NONE = 0,
        FUNCTION = 1,
        BLOCK = 2
    };

    Type type = NONE;
};


#endif //COMPILER_KONTEXT_H
