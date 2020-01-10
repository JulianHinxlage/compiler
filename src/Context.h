//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#ifndef COMPILER_KONTEXT_H
#define COMPILER_KONTEXT_H

#include "util/ArrayList.h"
#include <string>
#include <memory>

class Context;

class Variable{
public:
    std::string name;
    std::string type;
    std::string mods;
};

class Expresion{
public:
};

class Context {
public:
    std::string name;
    std::string returnType;
    util::ArrayList<Variable> parameter;

    util::ArrayList<Variable> variables;
    util::ArrayList<std::shared_ptr<Context>> contexts;
    std::shared_ptr<Context> parentContext = nullptr;

    enum Type{
        NONE = 0,
        FUNCTION = 1,
        BLOCK = 2
    };

    Type type = NONE;
};


#endif //COMPILER_KONTEXT_H
