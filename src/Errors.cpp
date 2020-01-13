//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#include "Errors.h"
#include "util/strutil.h"

Errors errors;

Errors::Errors() {
    errorCount = 0;
    warningCount = 0;
    files = nullptr;
}

void Errors::setFile(util::ArrayList<std::string> *files) {
    this->files = files;
}

std::string Errors::getFile(int id) {
    if(files == nullptr){
        return "";
    }
    if(id >= files->size()){
        return "";
    }
    auto str = util::split((*files)[id], "/");
    return str[str.size() - 1];
}