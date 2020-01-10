//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#include "util/strutil.h"
#include "util/log.h"
#include "Tokenizer.h"
#include "util/Clock.h"
#include "Parser.h"

std::string indent(int x){
    std::string str;
    for(int i = 0; i < x;i++) {
        str += " ";
    }
    return str;
}

void printContext(std::shared_ptr<Context> &context, int offset = 0){
    if(context->type == Context::BLOCK){
        util::logInfo(indent(offset), "{");
        offset++;
    }
    else if(context->type == Context::FUNCTION){
        util::logInfo(indent(offset), "func: ", context->returnType, " ", context->name);
        for(auto &p : context->parameter){
            util::logInfo(indent(offset), "param: ", p.type, " ", p.name);
        }
        util::logInfo(indent(offset), "{");
        offset++;
    }

    for(auto &v : context->variables){
        util::logInfo(indent(offset), "var: ", v.type, " ", v.name);
    }

    for(auto &c : context->contexts){
        printContext(c, offset);
    }

    offset--;
    util::logInfo(indent(offset), "}");
}

int main(int argc, char *argv[]){
    util::Clock clock;
    auto code = util::readFile("../res/code.txt");

    Tokenizer tokenizer;
    tokenizer.setDefault();
    tokenizer.setSource(code);

    Parser parser;
    auto context = parser.parse(tokenizer);

    printContext(context);
    util::logInfo("time: ", clock.elapsed());

    return 0;
}
