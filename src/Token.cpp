//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#include "Token.h"

Token::Token() {
    type = "";
    value = "";
    file = 0;
    line = 0;
    column = 0;
}

Token::Token(const std::string &type, const std::string &value) {
    this->type = type;
    this->value = value;
    file = 0;
    line = 0;
    column = 0;
}

Token::Token(const std::string &type, const std::string &value, int line, int column) {
    this->type = type;
    this->value = value;
    file = 0;
    this->line = line;
    this->column = column;
}
