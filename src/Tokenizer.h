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
    void setPrecedence(const std::string &value, int precedence);
    void setPrecedence(const util::ArrayList<std::string> &values, int precedence);
    void setDefault();
    bool isType(const std::string &type);
    int getPrecedence(const std::string &value);

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
    util::ArrayList<std::pair<std::string,int>> precedences;
    std::string source;
    int index;
    Token token;
    int file;
    int line;
    int column;

    bool inEscape;
};


#endif //COMPILER_TOKENIZER_H
