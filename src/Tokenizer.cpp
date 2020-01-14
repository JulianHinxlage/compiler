//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#include "util/strutil.h"
#include "Tokenizer.h"

Tokenizer::Tokenizer() {
    reset();
}

void Tokenizer::setSource(const std::string &source, int file) {
    this->source = source;
    this->file = file;
    sourceIndex = 0;
    line = 1;
    column = 0;
    stream.clear();
    streamPosition = -1;
    currentToken = Token();
}

void Tokenizer::setFile(const std::string &filename, int file) {
    setSource(util::readFile(filename), file);
}

void Tokenizer::defineToken(const std::string &type, const std::string &value) {
    tokens.add(Token(type, value));
}

void Tokenizer::defineToken(const std::string &type, const util::ArrayList<std::string> &values) {
    for(auto &value : values){
        defineToken(type, value);
    }
}

void Tokenizer::definePrecedence(int precedence, const std::string &value) {
    precedences.add({value, precedence});
}

void Tokenizer::definePrecedence(int precedence, const util::ArrayList<std::string> &values) {
    for(auto &value : values){
        definePrecedence(precedence, value);
    }
}

void Tokenizer::defineDefault() {
    defineToken("type", util::split("void char short int long float double"));
    defineToken("key", util::split("if else for while return break continue true false class private public protected"));
    defineToken("op", util::split("+ - * / % | & ^ ~ || && ^^ ! >> <<"));
    defineToken("aop", util::split("= += -= *= /= %= |= &= ^= ~= ||= &&= ^^= >>= <<="));
    defineToken("lop", util::split("> < => <= == !="));
    defineToken("uop", util::split("-- ++"));
    defineToken("com", util::split("// /* */"));
    defineToken("pun", util::split(". , ( ) [ ] { } ; : ' \""));
    defineToken("sep", util::split(" ,\t,\n", ","));

    definePrecedence(1, util::split("++ --"));
    definePrecedence(2, util::split("! ~"));
    definePrecedence(3, util::split("* / %"));
    definePrecedence(4, util::split("+ -"));
    definePrecedence(5, util::split(">> <<"));
    definePrecedence(6, util::split("< > <= =>"));
    definePrecedence(7, util::split("== !="));
    definePrecedence(8, util::split("&"));
    definePrecedence(9, util::split("^"));
    definePrecedence(10, util::split("|"));
    definePrecedence(11, util::split("&&"));
    definePrecedence(12, util::split("^^"));
    definePrecedence(13, util::split("||"));
    definePrecedence(14, util::split("= += -= *= /= %= >>= <<= &= ^= |= ||= &&= ^^="));
}

void Tokenizer::reset() {
    tokens.clear();
    precedences.clear();
    stream.clear();
    streamPosition = -1;
    currentToken = Token();
    source = "";
    sourceIndex = 0;
    file = 0;
    line = 1;
    column = 0;
}

bool Tokenizer::isType(const std::string &type) {
    if(util::split("ide num char str com hex bin float").contains(type)){
        return true;
    }
    for(auto &t : tokens){
        if(t.type == type){
            return true;
        }
    }
    return false;
}

int Tokenizer::getPrecedence(const std::string &value) {
    for(auto &p : precedences){
        if(p.first == value){
            return p.second;
        }
    }
    return -1;
}

void Tokenizer::shrinkStream() {
    if(streamPosition == stream.size() - 1){
        stream.clear();
        stream.add(currentToken);
        streamPosition = 0;
    }else{
        util::ArrayList<Token> tmp;
        for(int i = streamPosition;i < stream.size();i++){
            tmp.add(stream[i]);
        }
        stream.swap(tmp);
        tmp.clear();
        streamPosition = 0;
    }
}

Token Tokenizer::get() {
    return currentToken;
}

bool Tokenizer::next() {
    if(streamPosition < stream.size() - 1){
        streamPosition++;
        currentToken = stream[streamPosition];
        return true;
    }
    if(tokenizeNext()){
        stream.add(currentToken);
        streamPosition++;
        return true;
    }
    return false;
}

bool Tokenizer::prev() {
    if(streamPosition >= 0){
        streamPosition--;
        if(streamPosition == -1){
            currentToken = Token();
        }else{
            currentToken = stream[streamPosition];
        }
        return true;
    }
    return false;
}

bool Tokenizer::tokenizeNext() {
    currentToken.file = file;
    currentToken.line = line;
    currentToken.column = column + 1;

    //check token match
    int foundLength = 0;
    int foundIndex = -1;

    for(auto &token : tokens){
        int i = 0;
        for(i = 0; i < token.value.size();i++){
            if(sourceIndex + i < source.size()){
                if(source[sourceIndex + i] == token.value[i]){
                    continue;
                }
            }
            break;
        }
        if(i == token.value.size()){
            //match
            if(i > foundLength){
                foundLength = i;
                foundIndex = tokens.indexOf(token);
            }
        }
    }

    if(foundIndex != -1){
        currentToken.value = tokens[foundIndex].value;
        currentToken.type = tokens[foundIndex].type;
    }

    //check for non fixed tokens
    int size = checkMatch("__azAZ", "__09azAZ", "");
    if(size > foundLength){
        foundLength = size;
        currentToken.type = "ide";
        currentToken.value = source.substr(sourceIndex, foundLength);
    }

    size = checkMatch("09", "09", "");
    if(size > foundLength){
        foundLength = size;
        currentToken.type = "num";
        currentToken.value = source.substr(sourceIndex, foundLength);
    }

    size = checkMatch("0 x", "09af", "");
    if(size > foundLength){
        foundLength = size;
        currentToken.type = "hex";
        currentToken.value = source.substr(sourceIndex, foundLength);
    }

    size = checkMatch("0 b", "01", "");
    if(size > foundLength){
        foundLength = size;
        currentToken.type = "bin";
        currentToken.value = source.substr(sourceIndex, foundLength);
    }

    size = checkMatch("09", "09..", "");
    if(size > foundLength) {
        if (util::split(source.substr(sourceIndex, size), ".").size() == 2) {
            foundLength = size;
            currentToken.type = "float";
            currentToken.value = source.substr(sourceIndex, foundLength);
        }
    }

    size = checkMatch("\'", "*", "\'");
    if(size > foundLength){
        foundLength = size;
        currentToken.type = "char";
        currentToken.value = source.substr(sourceIndex, foundLength);
    }

    size = checkMatch("\"", "*", "\"");
    if(size > foundLength){
        foundLength = size;
        currentToken.type = "str";
        currentToken.value = source.substr(sourceIndex, foundLength);
    }

    size = checkMatch("/ /", "*", "\n");
    if(size > foundLength){
        foundLength = size;
        currentToken.type = "com";
        currentToken.value = source.substr(sourceIndex, foundLength);
    }

    size = checkMatch("/ *", "*", "* /");
    if(size > foundLength){
        foundLength = size;
        currentToken.type = "com";
        currentToken.value = source.substr(sourceIndex, foundLength);
    }

    //calculate line and column of next token
    for(int i = 0; i < foundLength;i++){
        char c = source[sourceIndex++];
        column++;
        if(c == '\n'){
            line++;
            column = 0;
        }
    }
    if(foundLength == 0){
        //if no match found => undef token
        if(sourceIndex + 1 < source.size()){
            currentToken.type = "undef";
            currentToken.value = "";
            currentToken.value += source[sourceIndex++];
            return true;
        }else{
            return false;
        }
    }
    return true;
}

bool checkPattern(char c, const std::string &pattern){
    if(pattern.size() == 1){
        return c == pattern[0];
    }
    if(pattern.size() % 2 != 0){
        return false;
    }
    for(int i = 0; i < pattern.size();i+=2){
        if(c >= pattern[i] && c <= pattern[i+1]){
            return true;
        }
    }
    return false;
}

int Tokenizer::checkMatch(const std::string &startPattern, const std::string &pattern, const std::string &endPattern){
    //start
    int index = sourceIndex;
    for(auto &p : util::split(startPattern)){
        if(index < source.size() && checkPattern(source[index], p)){
            index++;
        }else{
            return 0;
        }
    }

    int counter = 0;
    while(true) {
        for (auto &p : util::split(pattern)) {
            if (p == "*") {
                if(counter == 0){
                    break;
                }
                index++;
            } else {
                if (index < source.size() && checkPattern(source[index], p)) {
                    index++;
                } else {
                    if(endPattern == ""){
                        return index - sourceIndex;
                    }else{
                        return 0;
                    }
                }
            }
        }

        counter++;

        bool endHit = true;
        int endHitCount = 0;
        for (auto &p : util::split(endPattern)) {
            if (index < source.size() && checkPattern(source[index], p)) {
                index++;
                endHitCount++;
            } else {
                endHit = false;
                break;
            }
        }
        if (endHit && endHitCount > 0) {
            return index - sourceIndex;
        }else{
            index -= endHitCount;
        }
    }
}
