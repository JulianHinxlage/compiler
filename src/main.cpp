//
// Copyright (c) 2020 Julian Hinxlage. All rights reserved.
//

#include "util/strutil.h"
#include "util/log.h"
#include "Tokenizer.h"
#include "util/Clock.h"

int main(int argc, char *argv[]){
    util::Clock clock;
    auto code = util::readFile("../res/code.txt");

    Tokenizer t;
    t.setDefault();
    t.setSource(code);

    while(t.next()){
        if(t.get().type != "sep" && t.get().type != "com"){
            if(t.get().type == "undef"){
                util::logWarning(t.get().type, ":", t.get().value,":", t.get().line,":", t.get().column);
            }else{
                util::logInfo(t.get().type, ":", t.get().value,":", t.get().line,":", t.get().column);
            }
        }
    }
    util::logInfo("time: ", clock.elapsed());

    return 0;
}
