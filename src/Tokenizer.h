//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#ifndef COMPILER_TOKENIZER_H
#define COMPILER_TOKENIZER_H

#include "Token.h"
#include "util/ArrayList.h"

//converts a file or source into a token stream
class Tokenizer {
public:
    Tokenizer();

    //sets the source which is tokenized
    //the stream is reset
    //file is the file id tokens refer to
    void setSource(const std::string &source, int file = 0);

    //sets the file which is tokenized
    //the stream is reset
    //file is the file id tokens refer to
    void setFile(const std::string &filename, int file = 0);

    //defines a token with type
    void defineToken(const std::string &type, const std::string &value);
    void defineToken(const std::string &type, const util::ArrayList<std::string> &values);

    //defines the precedence of a token
    void definePrecedence(int precedence, const std::string &value);
    void definePrecedence(int precedence, const util::ArrayList<std::string> &values);

    //defines all tokens and precedence of the default tokenizer
    void defineDefault();

    //resets all defines
    void reset();

    //checks if the value is a type
    bool isType(const std::string &type);

    //gets the precedence of a value
    int getPrecedence(const std::string &value);

    //resets stream history
    //prev calls will fail after call
    void shrinkStream();

    //gets current token on stream position
    Token get();

    //increments the stream position
    bool next();

    //decrements the token position
    bool prev();

private:
    util::ArrayList<Token> tokens;
    util::ArrayList<std::pair<std::string,int>> precedences;
    util::ArrayList<Token> stream;
    int streamPosition;
    Token currentToken;
    std::string source;
    int sourceIndex;

    int file;
    int line;
    int column;

    bool tokenizeNext();
    int checkMatch(const std::string &startPattern, const std::string &pattern, const std::string &endPattern);
};


#endif //COMPILER_TOKENIZER_H
