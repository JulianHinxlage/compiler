//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#ifndef COMPILER_TOKEN_H
#define COMPILER_TOKEN_H

#include <string>

class Token {
public:
    std::string type;
    std::string value;
    int file;
    int line;
    int column;

    Token();
    Token(const std::string &type, const std::string &value);
    Token(const std::string &type, const std::string &value, int line, int column);
};


#endif //COMPILER_TOKEN_H
