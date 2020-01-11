//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#include "Parser.h"
#include "util/log.h"
#include "util/strutil.h"
#include "util/math.h"

Parser::Parser() {
    tokenizer = nullptr;
    tokenIndex = -1;
    context = nullptr;
    globalContext = nullptr;
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

bool Parser::checkAny(const std::string &symbols) {
    if (!next()) {
        return false;
    }
    for (auto &i : util::split(symbols)) {
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
    prev();
    return false;
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
    if(check("type ide") || check("ide ide")) {
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

            Expression &e = context->expressions.add(Expression(Expression::OPERATOR, get(0)));
            e.expressions.add(Expression(Expression::VAR, get(1)));

            Expression e2;
            if(expression(e2, ";")){
                e.expressions.add(e2);
                return true;
            }
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
    Expression e;
    if(ifelse(e)){
        context->expressions.add(e);
        return true;
    }
    if(check("{")){
        Expression &e = context->expressions.add();
        contextStepDown(Context::BLOCK);
        e.type = Expression::BLOCK;
        e.context = context;
        prev();
        return block();
    }
    if(expression(e, ";")){
        context->expressions.add(e);
        return true;
    }
    return false;
}

bool Parser::ifelse(Expression &e) {
    if(check("if")){
        if (check("(")) {
            e = Expression(Expression::OPERATOR, get(1));
            Expression e2;
            if (expression(e2, ")")) {
                e.expressions.add(e2);
                Expression e3 = Expression(Expression::BLOCK, Token());
                contextStepDown(Context::BLOCK);
                e3.context = context;
                e.expressions.add(e3);
                if(block()){
                    Expression e4;
                    if(ifelse(e4)){
                        e.expressions.add(e4);
                    }
                    return true;
                }
            }else{
                util::logWarning("expected expression at ", token.line, ":", token.column + token.value.size());
            }
            prev();
        }
        prev();
    }
    if(check("else")){
        if(ifelse(e)){
            return true;
        }

        e = Expression(Expression::BLOCK, Token());
        contextStepDown(Context::BLOCK);
        e.context = context;
        if (block()) {
            return true;
        }

    }
    return false;
}

bool Parser::parameter() {
    if(check("type ide") || check("ide ide")) {
        Variable &v = context->parameter.add();
        v.name = get(0).value;
        v.type = get(1).value;
        if (check("=")) {
            //TODO default parameter
            if(until(", )")){
                prev();
            }
        }
        return true;
    }
    return false;
}

bool Parser::factor(Expression &e){
    if(checkAny("num hex bin float str char")) {
        e = Expression(Expression::CONST, get(0));
        return true;
    }else if(check("ide")){
        if(check("(")){
            e = Expression(Expression::CALL, get(1));
            while(true){
                Expression e2;
                if(!expression(e2, ", )")){
                    break;
                }
                e.expressions.add(e2);
            }
            return true;
        }
        e = Expression(Expression::VAR, get(0));
        return true;
    }
    return false;
}

bool Parser::expression(Expression &e, const std::string &endSymbols) {
    if(check("(")){
        Expression e3;
        e.token = get(0);
        if(expression(e3, ")")){
            e.type = Expression::OPERATOR;
            e.expressions.add(e3);
        }
        if(token.value != ")"){
            util::logWarning("expected \")\" at ", token.line, ":", token.column + token.value.size());
        }
    }

    if(e.token.value.empty()){
        if(!factor(e)){
            return false;
        }
    }

    if(checkAny(endSymbols)){
        return true;
    }

    if(checkAny("op lop aop")){
        Expression op = Expression(Expression::OPERATOR, get(0));
        Expression e2;

        if(!factor(e2)){
            if(check("(")) {
                Expression e3;
                e2.token = get(0);
                e2.type = Expression::OPERATOR;
                if(!expression(e3, ")")){
                    util::logWarning("expected \")\" at ", token.line, ":", token.column + token.value.size());
                }
                if(token.value != ")"){
                    util::logWarning("expected \")\" at ", token.line, ":", token.column + token.value.size());
                }
                e2.expressions.add(e3);
            }else{
                util::logWarning("expected expression after ", token.line, ":", token.column + token.value.size());
            }
        }

        op.expressions.add(e);
        op.expressions.add(e2);
        expression(op, endSymbols);
        precedenceParsing(op);
        e = op;
        return true;
    }

    return false;
}

bool Parser::precedenceParsing(Expression &op){
    int p1 = tokenizer->getPrecedence(op.token.value);
    int p2 = tokenizer->getPrecedence(op.expressions[0].token.value);
    if(p1 != -1 && p2 != -1){
        if(p2 > p1) {
            util::swap(op.token, op.expressions[0].token);
            util::swap(op.expressions[0].expressions[0],op.expressions[1]);
            util::swap(op.expressions[0].expressions[0],op.expressions[0].expressions[1]);
            util::swap(op.expressions[0],op.expressions[1]);
        }
    }

    for(auto &e : op.expressions){
        if(e.type == Expression::OPERATOR){
            precedenceParsing(e);
        }
    }

    p1 = tokenizer->getPrecedence(op.token.value);
    p2 = tokenizer->getPrecedence(op.expressions[0].token.value);
    if(p1 != -1 && p2 != -1){
        if(p2 > p1) {
            util::swap(op.token, op.expressions[0].token);
            util::swap(op.expressions[0].expressions[0],op.expressions[1]);
            util::swap(op.expressions[0].expressions[0],op.expressions[0].expressions[1]);
            util::swap(op.expressions[0],op.expressions[1]);
        }
    }
    return true;
}

bool Parser::block() {
    if(!check("{")){
        util::logWarning("expected \"{\" at ", token.line, ":", token.column + token.value.size());
        contextStepUp();
        prev();//TODO next after failed statement
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
    while(checkAny(", (")){
        if(check(")")){
            prev();
            break;
        }
        if(!parameter()){
            util::logWarning("expected \")\" at ", token.line, ":", token.column + token.value.size());
            contextStepUp();
            return false;
        }
    }
    if(!check(")")){
        util::logWarning("expected \")\" at ", token.line, ":", token.column + token.value.size());
        contextStepUp();
        return false;
    }

    return block();
}

