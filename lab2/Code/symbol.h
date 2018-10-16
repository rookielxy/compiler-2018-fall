#ifndef __SYMBOL__
#define __SYMBOL__

#include "type.h"
#include "function.h"
#include "common.h"

class Symbol {
    string name;
    Type type;
public:
    Symbol(AstNode *varDec, const Type &type);
    string getName() const;
};

#endif