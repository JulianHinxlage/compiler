//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#include <util/strutil.h>
#include "CGenerator.h"

void CGenerator::generate(std::shared_ptr<Context> context, std::string &output) {
    output = "#include <unistd.h>\n";

    //global variables
    for(auto &v : context->variables){
        output += v.type;
        output += " ";
        output += v.mods;
        output += v.name;
        output += ";\n";
    }

    //functions
    for(auto &f : context->contexts){
        if(f->type == Context::FUNCTION){
            generateFunction(f, output);
        }
    }
}

void CGenerator::generateVariable(Variable &v, std::string &output, int offset) {
    output += indent(offset);
    output += v.type;
    output += " ";
    if(util::strContains(v.mods, " ")){
        if(util::split(v.mods).size() == 2){
            output += util::split(v.mods)[0];
            output += v.name;
            output += util::split(v.mods)[1];
        }else{
            output += v.name;
            output += util::split(v.mods)[0];
        }
    }else{
        output += v.mods;
        output += v.name;
    }
}

void CGenerator::generateFunction(std::shared_ptr<Context> context, std::string &output, int offset) {
    //generate nested functions
    for(auto &f : context->contexts){
        if(f->type == Context::FUNCTION){
            generateFunction(f, output, offset);
        }
    }

    if(context->type == Context::FUNCTION) {
        //function head
        generateVariable(context->func, output, offset);
        output += "(";

        //parameter
        for (auto &v : context->parameter) {
            generateVariable(v, output, 0);
            if (context->parameter.indexOf(v) != context->parameter.size() - 1) {
                output += ", ";
            }
        }
        output += ")";
    }
    output += "{\n";

    offset++;

    //local variables
    for(auto &v : context->variables){
        generateVariable(v, output, offset);
        output += ";\n";
    }

    //body
    for(auto &e : context->expressions){
        generateExpression(context, e, output, offset, true);
    }

    offset--;
    output += indent(offset);
    output += "}\n";
}

std::string CGenerator::indent(int count) {
    std::string str;
    for(int i = 0; i < count;i++) {
        str += "    ";
    }
    return str;
}

void CGenerator::generateExpression(std::shared_ptr<Context> context, Expression &expression, std::string &output, int offset, bool top) {
    output += indent(offset);

    switch(expression.type){
        case Expression::BLOCK:{
            generateFunction(expression.context, output, offset);
            top = false;
            break;
        }
        case Expression::OPERATOR:{
            if(expression.token.value == "("){
                output += expression.token.value;
                generateExpression(context, expression.expressions[0], output, 0);
            }else{
                generateExpression(context, expression.expressions[0], output, 0);
                output += " ";
                output += expression.token.value;
            }
            if(expression.expressions.size() >= 2){
                output += " ";
                generateExpression(context, expression.expressions[1], output, 0);
            }
            if(expression.token.value == "["){
                output += "]";
            }
            if(expression.token.value == "("){
                output += ")";
            }
            break;
        }
        case Expression::KEY_WORD:{
            output += expression.token.value;

            if(expression.token.value == "return"){
                output += " ";
                for(auto &e : expression.expressions){
                    generateExpression(context, e, output, 0);
                }
            }
            else if(expression.token.value == "for"){
                output += "(";
                generateExpression(context, expression.expressions[0], output, 0);
                output += "; ";
                generateExpression(context, expression.expressions[1], output, 0);
                output += "; ";
                generateExpression(context, expression.expressions[2], output, 0);
                output += ")";
                generateExpression(context, expression.expressions[3], output, offset);
                top = false;
            }else if(expression.token.value == "if"){
                output += "(";
                generateExpression(context, expression.expressions[0], output, 0);
                output += ")";
                if(expression.expressions.size() >= 3){
                    generateExpression(context, expression.expressions[1], output, offset);
                    output += indent(offset);
                    output += "else";
                    generateExpression(context, expression.expressions[2], output, offset);
                }else{
                    generateExpression(context, expression.expressions[1], output, offset);
                }
                top = false;
            }else{
                output += "(";
                generateExpression(context, expression.expressions[0], output, 0);
                output += ")";
                generateExpression(context, expression.expressions[1], output, offset);
                top = false;
            }
            break;
        }
        case Expression::CALL:{

            auto func = getFunc(context, expression.token.value);
            if(func != nullptr){
                output += functionName(func);
            }else{
                output += expression.token.value;
            }
            output += "(";

            for(auto &e : expression.expressions){
                if(expression.expressions.indexOf(e) != 0){
                    output += ", ";
                }
                generateExpression(context, e, output, 0);
            }

            output += ")";
            break;
        }
        case Expression::VAR:{
            output += expression.token.value;
            break;
        }
        case Expression::CONST:{
            output += expression.token.value;
            break;
        }
        default:{
            output += expression.token.value;
            output += " ";
            for(auto &e : expression.expressions){
                generateExpression(context, e, output, 0);
            }
            break;
        }
    }
    if(top){
        output += ";\n";
    }
}

std::string CGenerator::functionName(std::shared_ptr<Context> context) {
    std::string name;
    std::shared_ptr<Context> iter = context;
    while(iter->parentContext != nullptr){
        iter = iter->parentContext;
        if(iter->type == Context::FUNCTION){
            if(name.empty()){
                name = iter->func.name;
            }else{
                name = iter->func.name + "_"  + name;
            }
        }
    }
    if(name.empty()){
        name = context->func.name;
    }else{
        name = context->func.name + "_"  + name;
    }
    return name;
}

std::shared_ptr<Context> CGenerator::getFunc(std::shared_ptr<Context> context, const std::string &func) {
    for(auto &c : context->contexts){
        if(c->type == Context::FUNCTION){
            if(c->func.name == func){
                return c;
            }
        }
    }
    if(context->parentContext != nullptr){
        return getFunc(context->parentContext, func);
    }
    return nullptr;
}
