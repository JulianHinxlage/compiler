//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#include "print.h"
#include "util/log.h"

std::string indent(int x){
    std::string str;
    for(int i = 0; i < x;i++) {
        str += " ";
    }
    return str;
}

void printExpression(Expression &expression, int offset){
    if(expression.type == Expression::BLOCK){
        printContext(expression.context, offset);
        return;
    }

    util::logInfo(indent(offset), expression.token.value);

    for(auto &e : expression.expressions){
        printExpression(e, offset + 1);
    }
}

void printContext(std::shared_ptr<Context> &context, int offset){
    if(context->type == Context::BLOCK){
        util::logInfo(indent(offset), "{");
        offset++;
    }
    else if(context->type == Context::FUNCTION){
        util::logInfo(indent(offset), "func: ", context->func.type, " ", context->func.mods, context->func.name);
        for(auto &p : context->parameter){
            util::logInfo(indent(offset), "param: ", p.type, " ", p.mods, p.name);
        }
        util::logInfo(indent(offset), "{");
        offset++;
    }

    for(auto &v : context->variables){
        util::logInfo(indent(offset), "var: ", v.type, " ", v.mods, v.name);
    }

    for(auto &c : context->contexts){
        if(c->type != Context::BLOCK) {
            printContext(c, offset);
        }
    }

    for(auto &e : context->expressions){
        printExpression(e, offset);
    }

    offset--;
    util::logInfo(indent(offset), "}");
}

