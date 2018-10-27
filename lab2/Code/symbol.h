#ifndef __SYMBOL__
#define __SYMBOL__

#include "type.h"
#include "function.h"
#include "common.h"

class Symbol {
	friend class Type;
    string name;
    Type type;
    int line_no;
public:
	explicit Symbol(AstNode *param);
    Symbol(AstNode *varDec, const Type &type);

    Symbol(const Symbol &symbol) = default;
    ~Symbol() = default;
    Symbol& operator=(const Symbol &symbol) = default;

    string getName() const { return name; }
    int getLineNo() const { return line_no; }
    bool operator==(const Symbol &symbol) { return type == symbol.type; }
    // symbols are euqal if their types are equal
    Type getType() const { return type; }
};

#endif