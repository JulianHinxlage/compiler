//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#include "Parser.h"
#include "util/log.h"
#include "util/strutil.h"

Parser::Parser() {
    tokenizer = nullptr;
    tokenIndex = -1;
    context = nullptr;
    globalContext = nullptr;
}

bool Parser::next() {
    if(tokenIndex < tokens.size() - 1){
        tokenIndex++;
        token = tokens[tokenIndex];
        return true;
    }

    while(tokenizer->next()){
        token = tokenizer->get();
        if(token.type != "com" && token.type != "sep"){
            if(token.type == "undef"){
                util::logWarning("unknown symbol \"", token.value, "\" at ", token.line, ":", token.column);
            }else{
                //util::logInfo("token ", token.type, " ", token.value, " at ", token.line, ":", token.column);
                tokens.push(token);
                tokenIndex++;
                return true;
            }
        }
    }
    return false;
}

bool Parser::prev(){
    if(tokenIndex > 0){
        tokenIndex--;
        token = tokens[tokenIndex];
        return true;
    }
    return false;
}

std::shared_ptr<Context> Parser::parse(Tokenizer &tokenizer) {
    this->tokenizer = &tokenizer;
    tokens.clear();
    tokenIndex = -1;

    globalContext = std::make_shared<Context>();
    globalContext->type = Context::BLOCK;
    context = globalContext;

    while(true){
        if(!statement()){
            if(!next()){
                break;
            }
        }
    }

    auto ret = globalContext;
    context = nullptr;
    globalContext = nullptr;
    this->tokenizer = nullptr;
    tokens.clear();
    tokenIndex = -1;
    return ret;
}

bool Parser::check(const std::string &pattern) {
    int count = 0;
    bool hit;
    for(auto &i : util::split(pattern)){
        hit = false;
        if(next()) {
            count++;
            if (tokenizer->isType(i)) {
                if (token.type == i) {
                    hit = true;
                }
            } else {
                if (token.value == i) {
                    hit = true;
                }
            }
        }
        if(!hit){
            for(int j = 0; j < count;j++){
                prev();
            }
            return false;
        }
    }
    return true;
}

Token Parser::get(int offset) {
    if(tokenIndex >= offset){
        return tokens[tokenIndex - offset];
    }
    return Token();
}

bool Parser::until(const std::string &symbols) {
    while(next()){
        for(auto &i : util::split(symbols)) {
            if (tokenizer->isType(i)) {
                if (token.type == i) {
                    return true;
                }
            } else {
                if (token.value == i) {
                    return true;
                }
            }
        }
    }
    return false;
}

void Parser::contextStepDown(Context::Type type) {
    auto parent = context;
    context = context->contexts.add(std::make_shared<Context>());
    context->parentContext = parent;
    context->type = type;
}

void Parser::contextStepUp() {
    if(context->parentContext != nullptr){
        context = context->parentContext;
    }
}

bool Parser::statement() {
    if(check("type ide")) {
        if (check(";")) {
            Variable &v = context->variables.add();
            v.name = get(1).value;
            v.type = get(2).value;
            return true;
        }
        if (check("=")) {
            Variable &v = context->variables.add();
            v.name = get(1).value;
            v.type = get(2).value;
            expresion();
            return true;
        }
        if (check("(")) {
            contextStepDown(Context::FUNCTION);
            context->name = get(1).value;
            context->returnType = get(2).value;
            prev();
            return function();
        }
    }
    if(check("{")){
        contextStepDown(Context::BLOCK);
        prev();
        return block();
    }
    return false;
}

bool Parser::parameter() {
    if(check("type ide")) {
        Variable &v = context->parameter.add();
        v.name = get(0).value;
        v.type = get(1).value;
        if (check("=")) {
            if(until(", )")){
                prev();
            }
        }
        return true;
    }
    return false;
}

bool Parser::expresion() {
    return until(";");
}

bool Parser::block() {
    if(!check("{")){
        util::logWarning("expected \"{\" at ", token.line, ":", token.column);
        contextStepUp();
        return false;
    }

    while(true){
        if(!statement()){
            if(check("}")){
                break;
            }
            if(!next()){
                break;
            }
        }
    }

    contextStepUp();
    return true;
}

bool Parser::function() {
    while(check("(") || check(",")){
        if(check(")")){
            prev();
            break;
        }
        if(!parameter()){
            util::logWarning("error at ", token.line, ":", token.column);
            contextStepUp();
            return false;
        }
    }
    if(!check(")")){
        util::logWarning("expected \")\" at ", token.line, ":", token.column);
        contextStepUp();
        return false;
    }

    return block();
}
