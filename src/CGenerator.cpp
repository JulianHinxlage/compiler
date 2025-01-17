//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#include "CGenerator.h"
#include "util/strutil.h"
#include "util/math.h"

void CGenerator::generate(std::shared_ptr<Context> context, std::string &output) {
    output = "#include <unistd.h>\n";

    //global variables
    for(auto &v : context->variables){
        output += v.type;
        output += " ";
        output += v.mods;
        output += translateName(v.name);
        output += ";\n";
    }

    //functions
    generateFunctionDeclaration(context, output);
    for(auto &f : context->contexts){
        if(f->type == Context::FUNCTION){
            generateFunction(f, output);
        }
    }
}

void CGenerator::generateFunctionDeclaration(std::shared_ptr<Context> context, std::string &output, int offset) {
    for(auto &f : context->contexts){
        if(f->type == Context::FUNCTION){
            generateFunctionDeclaration(f, output);
        }
    }
    if(context->type == Context::FUNCTION){
        //function head
        std::string name = context->func.name;
        context->func.name = functionName(context);
        generateVariable(context->func, output, offset);
        context->func.name = name;

        output += "(";

        //parameter
        for (auto &v : context->parameter) {
            generateVariable(v, output, 0);
            if (context->parameter.indexOf(v) != context->parameter.size() - 1) {
                output += ", ";
            }
        }
        output += ");\n";
    }
}

void CGenerator::generateVariable(Variable &v, std::string &output, int offset) {
    output += indent(offset);
    output += v.type;
    output += " ";
    if(util::strContains(v.mods, " ")){
        if(util::split(v.mods).size() == 2){
            output += util::split(v.mods)[0];
            output += translateName(v.name);
            output += util::split(v.mods)[1];
        }else{
            output += translateName(v.name);
            output += util::split(v.mods)[0];
        }
    }else{
        output += v.mods;
        output += translateName(v.name);
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
        std::string name = context->func.name;
        context->func.name = functionName(context);
        generateVariable(context->func, output, offset);
        context->func.name = name;
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
                output += translateName(functionName(func));
            }else{
                output += translateName(expression.token.value);
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
            output += translateName(expression.token.value);
            break;
        }
        case Expression::CONST:{
            output += expression.token.value;
            break;
        }
        case Expression::CAST:{
            output += "(";
            output += expression.token.value;
            output += ")";
            generateExpression(context, expression.expressions[0], output, offset);
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
                name = util::replace(iter->func.name, "_", "__");
            }else{
                name = util::replace(iter->func.name, "_", "__") + "_"  + name;
            }
        }
    }
    if(name.empty()){
        name = util::replace(context->func.name, "_", "__");
    }else{
        name = util::replace(context->func.name, "_", "__") + "_"  + name;
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

std::string CGenerator::translateName(const std::string &name) {
    if(!nameTranslation){
        return name;
    }else{
        if(name == "main" || name == "syscall"){
            return name;
        }

        for(auto &i : names){
            if(i.first == name){
                return i.second;
            }
        }

        if(nameTranslationHex){
            std::string translation = "x";
            for(int i = 0; i < 4;i++){
                translation += "0123456789abcdef"[util::randi(0,15)];
            }
            names.add({name, translation});
            return translation;
        }else{
            std::string translation = "";
            int num = names.size();
            while(num >= 26){
                translation += (num % 26) + 'a';
                num /= 26;
            }
            translation += (num % 26) + 'a';
            names.add({name, translation});
            return translation;
        }
    }
}
