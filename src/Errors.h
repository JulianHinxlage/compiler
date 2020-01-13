//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#ifndef COMPILER_ERRORS_H
#define COMPILER_ERRORS_H

#include "util/ArrayList.h"
#include "Token.h"
#include "util/log.h"

class Errors {
public:
    int errorCount;
    int warningCount;

    Errors();
    void setFile(util::ArrayList<std::string> *files);
    template<typename... T>
    void error(Token &location, bool after, T... t);
    template<typename... T>
    void error(T... t);
    template<typename... T>
    void warning(Token &location, bool after, T... t);
    template<typename... T>
    void warning(T... t);
private:
    util::ArrayList<std::string> *files;
    std::string getFile(int id);
};

extern Errors errors;

template<typename... T>
void Errors::error(Token &location, bool after, T... t) {
    errorCount++;
    if(after){
        util::logError(getFile(location.file), ":", location.line, ":", location.column + location.value.size(), ": " , t...);
    }else{
        util::logError(getFile(location.file), ":", location.line, ":", location.column, ": " , t...);
    }
}

template<typename... T>
void Errors::error(T... t) {
    errorCount++;
    util::logError(t...);
}

template<typename... T>
void Errors::warning(Token &location, bool after, T... t) {
    warningCount++;
    if(after){
        util::logWarning(getFile(location.file), ":", location.line, ":", location.column + location.value.size(), ": " , t...);
    }else{
        util::logWarning(getFile(location.file), ":", location.line, ":", location.column, ": " , t...);
    }
}

template<typename... T>
void Errors::warning(T... t) {
    warningCount++;
    util::logWarning(t...);
}

#endif //COMPILER_ERRORS_H
