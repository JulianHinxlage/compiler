//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#include "util/strutil.h"
#include "Tokenizer.h"

Tokenizer::Tokenizer() {
    source = "";
    index = 0;
    file = 0;
    line = 1;
    column = 0;
    inEscape = false;
}

Tokenizer::Tokenizer(const std::string &source) : Tokenizer() {
    setSource(source);
}

void Tokenizer::setSource(const std::string &source, int file) {
    this->source = source;
    index = 0;
    this->file = file;
    line = 1;
    column = 0;
    inEscape = false;
}

void Tokenizer::setToken(const std::string &type, const std::string &value) {
    types.add(Data(Token(type,value), 0, 0));
}

void Tokenizer::setToken(const std::string &type, const util::ArrayList<std::string> &values) {
    for(auto &value : values){
        setToken(type, value);
    }
}

void Tokenizer::setDefault() {
    setToken("type", util::split("void char short int long float double"));
    setToken("key", util::split("if else for while return break continue true false"));
    setToken("op", util::split("+ - * / % | & ^ ~ || && ^^ !"));
    setToken("aop", util::split("= += -= *= /= %= |= &= ^= ~= ||= &&= ^^="));
    setToken("lop", util::split("> < => <= == !="));
    setToken("com", util::split("// /* */"));
    setToken("pun", util::split(". , ( ) [ ] { } ; : ' \""));
    setToken("sep", util::split(" ,\t,\n", ","));
}

Token Tokenizer::get() {
    return token;
}

bool Tokenizer::isType(const std::string &type) {
    if(util::split("undef str char com num hex bin float ide").contains(type)){
        return true;
    }
    for(auto &t : types){
        if(t.token.type == type){
            return true;
        }
    }
    return false;
}

bool Tokenizer::next() {
    int startIndex = index;
    int endIndex = -1;
    bool found = false;
    bool inProcess = true;
    int startLine = line;
    int startColumn = column;

    while(!(found && !inProcess)){
        if(index >= (int)source.size()){
            if(!found){
                token.type = "undef";
                token.value = source.substr(startIndex, startIndex - index);
                token.line = startLine;
                token.column = startColumn + 1;
                endIndex = index;
                found = index > startIndex;
            }
            break;
        }

        char c = source[index];
        column++;
        if(c == '\n'){
            line++;
            column = 0;
        }

        inProcess = false;
        //check all values
        for(auto &data : types){
            if(data.matchCounter < (int)data.token.value.size()){
                if (data.token.value[data.matchCounter] == c) {
                    inProcess = true;
                    if(data.matchCounter == 0){
                        data.startIndex = index;
                        data.token.line = line;
                        data.token.column = column;
                        data.token.file = file;
                    }
                    data.matchCounter++;
                }else{
                    data.matchCounter = 0;
                }

                if(data.matchCounter >= (int)data.token.value.size()) {
                    //match
                    if(data.startIndex == startIndex){
                        token = data.token;
                        endIndex = index + 1;
                    }else if(!found){
                        token.type = "undef";
                        token.value = source.substr(startIndex, data.startIndex - startIndex);
                        token.line = startLine;
                        token.column = startColumn + 1;
                        endIndex = data.startIndex;
                    }
                    found = true;
                }
            }

        }

        index++;
    }

    if(endIndex != -1){
        index = endIndex;
        line = startLine;
        column = startColumn;

        for(int i = startIndex;i < index;i++){
            char c = source[i];
            column++;
            if(c == '\n'){
                line++;
                column = 0;
            }
        }
    }

    for(auto &data : types) {
        data.matchCounter = 0;
        data.startIndex = 0;
    }

    check();

    return found;
}

bool inRange(char c, char r1, char r2){
    return c >= r1 && c <= r2;
}

bool inAnyRange(char c, const std::string &str){
    if(str.size() % 2 == 1){
        return false;
    }
    for(int i = 0; i < (int)str.size();i += 2){
        if(inRange(c,str[i], str[i+1])){
            return true;
        }
    }
    return false;
}

bool isMatch(const std::string &str, const std::string &range1, const std::string &range2){
    bool is = true;
    for(int i = 0; i < (int)str.size();i++) {
        char c = str[i];
        if(i == 0){
            if(!inAnyRange(c, range1)){
                is = false;
                break;
            }
        }else{
            if(!inAnyRange(c, range2)){
                is = false;
                break;
            }
        }
    }
    return is;
}

void Tokenizer::check() {
    if(token.value == "\"" && !inEscape){
        inEscape = true;
        Token tmp = token;
        while(next()){
            tmp.value += get().value;
            if(get().value == "\""){
                break;
            }
        }
        tmp.type = "str";
        token = tmp;
        inEscape = false;
    }else if(token.value == "\'" && !inEscape){
        inEscape = true;
        Token tmp = token;
        while(next()){
            tmp.value += get().value;
            if(get().value == "\'"){
                break;
            }
        }
        tmp.type = "char";
        token = tmp;
        inEscape = false;
    }else if(token.value == "//" && !inEscape){
        inEscape = true;
        Token tmp = token;
        while(next()){
            tmp.value += get().value;
            if(get().value == "\n"){
                break;
            }
        }
        tmp.type = "com";
        token = tmp;
        inEscape = false;
    }else if(token.value == "/*" && !inEscape){
        inEscape = true;
        Token tmp = token;
        while(next()){
            tmp.value += get().value;
            if(get().value == "*/"){
                break;
            }
        }
        tmp.type = "com";
        token = tmp;
        inEscape = false;
    }else if(token.type == "undef"){

        //identifier
        if(isMatch(token.value, "__azAZ", "__09azAZ")){
            token.type = "ide";
            return;
        }

        //number(int)
        if(isMatch(token.value, "09", "09")){
            token.type = "num";

            if(source[index] == '.'){
                Token tmp = token;
                next();
                tmp.value += get().value;
                next();
                tmp.value += get().value;
                token = tmp;
                token.type = "undef";
            }else{
                return;
            }
        }

        //hex
        if(token.value[0] == '0'){
            if(isMatch(token.value.substr(1), "xx", "09af")){
                token.type = "hex";
                return;
            }
        }

        //bin
        if(token.value[0] == '0'){
            if(isMatch(token.value.substr(1), "bb", "01")){
                token.type = "bin";
                return;
            }
        }

        //float
        if(isMatch(token.value, "09..", "09..")){
            if(util::split(token.value,".").size() < 3){
                token.type = "float";
                return;
            }
        }
    }
}
