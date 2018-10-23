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
    ~Symbol() { name.~string(); delete type; }
    string getName() const;
    int getLineNo() const;
};

#endif