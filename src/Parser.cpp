//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#include "Parser.h"
#include "util/log.h"

Parser::Parser() {
    tokenizer = nullptr;
}

bool Parser::step() {
    while(tokenizer->next()){
        token = tokenizer->get();
        if(token.type != "com" && token.type != "sep"){
            if(token.type == "undef"){
                util::logWarning("unknown symbol \"", token.value, "\" at ", token.line, ":", token.column);
            }else{
                return true;
            }
        }
    }
    return false;
}

void Parser::parse(Tokenizer &tokenizer) {
    globalContext = std::make_shared<Context>();
    globalContext->type = Context::BLOCK;
    context = globalContext;
    this->tokenizer = &tokenizer;
    if(step()){
        while(statement()){}
    }
}

bool Parser::statement() {
    if(token.type == "type" || token.type == "ide"){
        std::string type = token.value;
        if(step()){
            if(token.type == "ide"){
                std::string name = token.value;
                if(step()){
                    if(token.value == ";"){
                        Variable &v = context->variables.add();
                        v.type = type;
                        v.name = name;
                        return true;
                    }
                    else if(token.value == "="){
                        if(expresion()){
                            Variable &v = context->variables.add();
                            v.type = type;
                            v.name = name;
                            return true;
                        }else{
                            util::logWarning("expected expresion at ", token.line, ":", token.column);
                        }
                    }
                    else if(token.value == "("){
                        auto newContext = context->contexts.add(std::make_shared<Context>());
                        newContext->parentContext = context;
                        context = newContext;

                        context->returnType = type;
                        context->name = name;
                        context->type = Context::FUNCTION;

                        if(!step()){
                            return false;
                        }
                        while(parameter()){
                            if(token.value == ")"){
                                break;
                            }else if(token.value != ","){
                                util::logWarning("expected , or ) at ", token.line, ":", token.column);
                                return false;
                            }
                            if(!step()){
                                return false;
                            }
                        }
                        step();

                        if(function()){
                            if(context->parentContext != nullptr){
                                context = context->parentContext;
                            }
                            return true;
                        }
                    }
                }
            }
        }
    }
    else if(token.value == "{"){
        auto newContext = context->contexts.add(std::make_shared<Context>());
        newContext->parentContext = context;
        context = newContext;
        context->type = Context::BLOCK;

        if(function()){
            if(context->parentContext != nullptr){
                context = context->parentContext;
            }
            return true;
        }
    }
    else{
        if(expresion()){
            return true;
        }
    }
    return false;
}

bool Parser::parameter() {
    if(token.type == "type"){
        std::string type = token.value;
        if(step()){

            std::string mods;
            while(token.value == "*") {
                mods += "*";
                if(!step()){
                    return false;
                }
            }
            while(token.value == "&") {
                mods += "&";
                if(!step()){
                    return false;
                }
            }

            if(token.type == "ide") {
                std::string name = token.value;
                if(step()){

                    if(token.value == "="){
                        if(step()){
                            if(step()){

                                Variable &v = context->parameter.add();
                                v.type = type;
                                v.name = name;
                                v.mods = mods;

                                return true;
                            }
                        }
                    }else{

                        Variable &v = context->parameter.add();
                        v.type = type;
                        v.name = name;
                        v.mods = mods;

                        return true;
                    }

                }
            }
        }
    }
    return false;
}

bool Parser::expresion() {
    while(token.value != ";"){
        if(!step()){
            return false;
        }
    }
    return step();
}

bool Parser::function() {
    if (token.value == "{") {
        if(!step()){
            return false;
        }
        while (token.value != "}") {
            if(!statement()){
                if(!step()){
                    return false;
                }
            }
        }
        if(!step()){
            return false;
        }
    } else {
        util::logWarning("expected { at ", token.line, ":", token.column);
    }
    return true;
}
