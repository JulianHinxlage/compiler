//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#include "SemanticChecker.h"
#include "util/log.h"

Variable *SemanticChecker::getVar(std::shared_ptr<Context> context, const std::string &var){
    for(auto &p : context->parameter){
        if(p.name == var){
            return &p;
        }
    }
    for(auto &v : context->variables){
        if(v.name == var){
            return &v;
        }
    }

    if(context->type == Context::BLOCK){
        if(context->parentContext != nullptr){
            return getVar(context->parentContext, var);
        }
    }else{
        //check global variables
        std::shared_ptr<Context> c = context;
        while(c->parentContext != nullptr){
            c = c->parentContext;
        }
        return getVar(c, var);
    }
    return nullptr;
}

Context *SemanticChecker::getFunc(std::shared_ptr<Context> context, const std::string &func){
    for(auto &c : context->contexts){
        if(c->type == Context::FUNCTION){
            if(c->func.name == func){
                return c.get();
            }
        }
    }
    if(context->parentContext != nullptr){
        return getFunc(context->parentContext, func);
    }
    return nullptr;
}

void SemanticChecker::check(Expression &e, std::shared_ptr<Context> context) {
    //is variable existing
    if(e.type == Expression::VAR){
        if(getVar(context, e.token.value) == nullptr){
            util::logWarning("variable ", e.token.value, " is not defined at ", e.token.line, ":", e.token.column);
        }
    }

    //is called function existing
    if(e.type == Expression::CALL){
        Context *func = getFunc(context, e.token.value);
        if(func == nullptr){
            util::logWarning("function ", e.token.value, " is not defined at ", e.token.line, ":", e.token.column);
        }else{
            if(func->parameter.size() != e.expressions.size()){
                util::logWarning("function ", e.token.value, " takes ", func->parameter.size(), " arguments, but ", e.expressions.size(), " arguments provided at ", e.token.line, ":", e.token.column);
            }
        }
    }

    if(e.type == Expression::BLOCK){
        check(e.context);
    }

    //recursively check expression
    for(auto &e2 : e.expressions){
        check(e2, context);
    }
}

void SemanticChecker::check(std::shared_ptr<Context> context){
    //variable redefinition
    for(int i1 = 0; i1 < context->variables.size(); i1++){
        auto &v1 = context->variables[i1];
        for(int i2 = i1 + 1; i2 < context->variables.size(); i2++) {
            auto &v2 = context->variables[i2];
            if (v1.name == v2.name) {
                util::logWarning("variable ", v2.name, " at ", v2.location.line, ":", v2.location.column, " is already defined at ", v1.location.line, ":", v1.location.column);
            }
        }
        for(auto &v2 : context->parameter) {
            if (v1.name == v2.name) {
                util::logWarning("variable ", v1.name, " at ", v1.location.line, ":", v1.location.column, " is already defined at ", v2.location.line, ":", v2.location.column);
            }
        }
    }

    //expressions
    for(auto &e : context->expressions){
        check(e, context);
    }

    //functions
    for(auto &c : context->contexts){
        if(c->type == Context::FUNCTION){
            check(c);
        }
    }
}
