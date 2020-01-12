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

void Tokenizer::reset(){
    source = "";
    index = 0;
    file = 0;
    line = 1;
    column = 0;
    inEscape = false;
}

void Tokenizer::setSource(const std::string &source, int file) {
    this->source = source;
    index = 0;
    this->file = file;
    line = 1;
    column = 0;
    inEscape = false;
}

void Tokenizer::setFile(const std::string &filename, int file) {
    this->source = util::readFile(filename);
    index = 0;
    this->file = file;
    line = 1;
    column = 0;
    inEscape = false;
}

void Tokenizer::setToken(const std::string &type, const std::string &value) {
    types.add(Token(type,value));
}

void Tokenizer::setToken(const std::string &type, const util::ArrayList<std::string> &values) {
    for(auto &value : values){
        setToken(type, value);
    }
}

void Tokenizer::setPrecedence(const std::string &value, int precedence) {
    precedences.add({value,precedence});
}

void Tokenizer::setPrecedence(const util::ArrayList<std::string> &values, int precedence) {
    for(auto &value : values){
        setPrecedence(value,precedence);
    }
}

void Tokenizer::setDefault() {
    setToken("type", util::split("void char short int long float double"));
    setToken("key", util::split("if else for while return break continue true false"));
    setToken("op", util::split("+ - * / % | & ^ ~ || && ^^ ! >> <<"));
    setToken("aop", util::split("= += -= *= /= %= |= &= ^= ~= ||= &&= ^^= >>= <<="));
    setToken("lop", util::split("> < => <= == !="));
    setToken("uop", util::split("-- ++"));
    setToken("com", util::split("// /* */"));
    setToken("pun", util::split(". , ( ) [ ] { } ; : ' \""));
    setToken("sep", util::split(" ,\t,\n", ","));

    setPrecedence(util::split("++ --"),1);
    setPrecedence(util::split("! ~"),2);
    setPrecedence(util::split("* / %"),3);
    setPrecedence(util::split("+ -"),4);
    setPrecedence(util::split(">> <<"),5);
    setPrecedence(util::split("< > <= =>"),6);
    setPrecedence(util::split("== !="),7);
    setPrecedence(util::split("&"),8);
    setPrecedence(util::split("^"),9);
    setPrecedence(util::split("|"),10);
    setPrecedence(util::split("&&"),11);
    setPrecedence(util::split("^^"),12);
    setPrecedence(util::split("||"),13);
    setPrecedence(util::split("= += -= *= /= %= >>= <<= &= ^= |= ||= &&= ^^="),14);
}

bool Tokenizer::isType(const std::string &type) {
    if(util::split("undef str char com num hex bin float ide").contains(type)){
        return true;
    }
    for(auto &t : types){
        if(t.type == type){
            return true;
        }
    }
    return false;
}

int Tokenizer::getPrecedence(const std::string &value) {
    for(auto &i : precedences){
        if(i.first == value){
            return  i.second;
        }
    }
    return -1;
}

Token Tokenizer::get() {
    return token;
}

bool Tokenizer::next() {
    token = Token("","",line,column);
    token.file = file;
    if(index >= (int)source.size()){
        return false;
    }
    int maxLength = 0;
    Token found;

    for(auto &data : types){
        int i = 0;
        for(i = 0; i < (int)data.value.size();i++){
            if(index + i < (int)source.size()){
                if(source[index + i] == data.value[i]){
                    continue;
                }
            }
            break;
        }

        if(i == (int)data.value.size()){
            //match
            if(i > maxLength){
                maxLength = i;
                found = data;
            }
        }
    }

    if(maxLength != 0) {
        token.value = found.value;
        token.type = found.type;
    }else {
        token.type = "undef";
        token.value = source[index];

        //check for max length undef
        Token last = token;
        int i = 0;

        while(true) {
            checkUndef();
            if (token.type != "undef") {
                last = token;
                token.type = "undef";
                i++;
                token.value += source[index + i];
            }else{
                break;
            }
        }

        token = last;
        maxLength = token.value.size();
    }

    //calculate line and column for next token
    for(int i = 0; i < maxLength;i++){
        char c = source[index + i];
        column++;
        if(c == '\n'){
            line++;
            column = 0;
        }
    }
    index += maxLength;

    //check for multi token reduction
    check();

    return true;
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
    }
}

void Tokenizer::checkUndef() {
    if(token.type == "undef"){
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
