//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#include "CGenerator.h"

void CGenerator::generate(std::shared_ptr<Context> context, std::string &output) {
    output = "";

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

void CGenerator::generateFunction(std::shared_ptr<Context> context, std::string &output, int offset) {
    //generate nested functions
    for(auto &f : context->contexts){
        if(f->type == Context::FUNCTION){
            generateFunction(f, output, offset);
        }
    }

    if(context->type == Context::FUNCTION) {
        //function head
        output += indent(offset);
        output += context->func.type;
        output += " ";
        output += context->func.mods;
        output += functionName(context);
        output += "(";

        //parameter
        for (auto &v : context->parameter) {
            output += v.type;
            output += " ";
            output += v.mods;
            output += v.name;
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
        output += indent(offset);
        output += v.type;
        output += " ";
        output += v.mods;
        output += v.name;
        output += ";\n";
    }

    //body
    for(auto &e : context->expressions){
        generateExpression(context, e, output, offset);
        output += ";\n";
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

void CGenerator::generateExpression(std::shared_ptr<Context> context, Expression &expression, std::string &output, int offset) {
    output += indent(offset);

    switch(expression.type){
        case Expression::OPERATOR:{
            generateExpression(context, expression.expressions[0], output, 0);
            output += " ";
            output += expression.token.value;
            if(expression.expressions.size() >= 2){
                output += " ";
                generateExpression(context, expression.expressions[1], output, 0);
            }
            return;
        }
        case Expression::KEY_WORD:{
            output += expression.token.value;

            if(expression.token.value != "return"){
                output += "(";
                generateExpression(context, expression.expressions[0], output, 0);
                output += ")";
                generateFunction(expression.expressions[1].context, output, offset);
                //output += "}\n";
            }else{
                output += " ";
                for(auto &e : expression.expressions){
                    generateExpression(context, e, output, 0);
                }
            }

            return;
        }
        case Expression::CALL:{
            output += functionName(getFunc(context, expression.token.value));
            output += "(";

            for(auto &e : expression.expressions){
                if(expression.expressions.indexOf(e) != 0){
                    output += ", ";
                }
                generateExpression(context, e, output, 0);
            }

            output += ")";
            return;
        }
        case Expression::VAR:{
            output += expression.token.value;
            return;
        }
        case Expression::CONST:{
            output += expression.token.value;
            return;
        }
        default:{
            output += expression.token.value;
            output += " ";
            for(auto &e : expression.expressions){
                generateExpression(context, e, output, 0);
            }
        }
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
