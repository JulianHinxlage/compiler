//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#ifndef COMPILER_SEMANTICCHECKER_H
#define COMPILER_SEMANTICCHECKER_H

#include "Context.h"

class SemanticChecker {
public:
    Variable *getVar(std::shared_ptr<Context> context, const std::string &var);
    Context *getFunc(std::shared_ptr<Context> context, const std::string &func);
    void check(Expression &e, std::shared_ptr<Context> context);
    void check(std::shared_ptr<Context> context);
};


#endif //COMPILER_SEMANTICCHECKER_H
