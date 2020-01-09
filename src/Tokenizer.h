//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#ifndef COMPILER_TOKENIZER_H
#define COMPILER_TOKENIZER_H

#include "Token.h"
#include "util/ArrayList.h"

class Tokenizer {
public:
    Tokenizer();
    explicit Tokenizer(const std::string &source);
    void setSource(const std::string &source, int file = 0);
    void setToken(const std::string &type, const std::string &value);
    void setToken(const std::string &type, const util::ArrayList<std::string> &values);
    void setDefault();

    Token get();
    bool next();

private:
    class Data{
    public:
        Token token;
        int matchCounter;
        int startIndex;

        Data(){
            token = Token();
            matchCounter = 0;
            startIndex = 0;
        }

        Data(const Token &token, int matchCounter, int startIndex){
            this->token = token;
            this->matchCounter = matchCounter;
            this->startIndex = startIndex;
        }
    };

    void check();

    util::ArrayList<Data> types;
    std::string source;
    int index;
    Token token;
    int file;
    int line;
    int column;

    bool inStr;
    bool inChar;
    bool inComment;
};


#endif //COMPILER_TOKENIZER_H