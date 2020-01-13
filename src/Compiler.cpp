//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#include "Compiler.h"
#include "SemanticChecker.h"
#include "CGenerator.h"
#include "util/math.h"
#include "Errors.h"

Compiler::Compiler() {
    tokenizer.setDefault();
    util::rands();
}

void Compiler::compile(const std::string &file) {
    setFile(file);
    auto newAst = parser.parse(tokenizer);
    tokenizer.setSource("",0);

    if(ast == nullptr){
        ast = newAst;
    }else{
        //fuse ast's
        for(auto &e : newAst->expressions){
            ast->expressions.add(e);
        }

        for(auto &v : newAst->variables){
            ast->variables.add(v);
        }

        for(auto &c : newAst->contexts){
            c->parentContext = ast;
            ast->contexts.add(c);
        }
    }

}

std::string &Compiler::generate(bool obfuscate){
    SemanticChecker semantic;
    CGenerator generator;
    if(obfuscate){
        generator.nameTranslation = true;
        generator.nameTranslationHex = true;
    }

    if(ast != nullptr){
        semantic.check(ast);
        if(errors.errorCount == 0){
            generator.generate(ast, output);
        }
    }
    return output;
}

void Compiler::setFile(const std::string &file) {
    bool set = false;
    for(int i = 0; i < files.size();i++){
        if(files[i] == file){
            tokenizer.setFile(file, i);
            set = true;
            break;
        }
    }
    if(!set){
        tokenizer.setFile(file, files.size());
        files.add(file);
    }
    errors.setFile(&files);
}
