#ifndef __SYMBOL__
#define __SYMBOL__

#include "type.h"
#include "function.h"
#include "common.h"

class Symbol {
	friend class Type;
    string name;
    Type *type;
    int line_no;
public:
	explicit Symbol(AstNode *param);
    Symbol(AstNode *varDec, Type *type);
    Symbol(const Symbol &symbol);
    ~Symbol();
    string getName() const { return name; }
    int getLineNo() const { return line_no; }
    bool operator==(const Symbol &symbol) { return *type == *symbol.type; }
    // symbols are euqal if their types are equal
};

#endif