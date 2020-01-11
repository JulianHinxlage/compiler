//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#ifndef COMPILER_PRINT_H
#define COMPILER_PRINT_H

#include "Context.h"

std::string indent(int x);

void printContext(std::shared_ptr<Context> &context, int offset = 0);

void printExpression(Expression &expression, int offset = 0);

#endif //COMPILER_PRINT_H
