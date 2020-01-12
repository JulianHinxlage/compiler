//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#ifndef COMPILER_CGENERATOR_H
#define COMPILER_CGENERATOR_H

#include "Context.h"

class CGenerator {
public:
    void generate(std::shared_ptr<Context> context,std::string &output);
private:
    void generateFunction(std::shared_ptr<Context> context,std::string &output, int offset = 0);
    void generateExpression(std::shared_ptr<Context> context, Expression &expression,std::string &output, int offset = 0, bool top = false);
    std::shared_ptr<Context> getFunc(std::shared_ptr<Context> context, const std::string &func);
    std::string functionName(std::shared_ptr<Context> context);
    std::string indent(int count);
};


#endif //COMPILER_CGENERATOR_H
