//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#include "Compiler.h"
#include "util/math.h"

Compiler::Compiler() {
    tokenizer.setDefault();
    util::rands();
}

void Compiler::compile(const std::string &file) {
    setFile(file);
    ast = parser.parse(tokenizer);
    tokenizer.setSource("",0);
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
}