#ifndef __FUNCTION__
#define __FUNCTION__

#include "type.h"
#include "common.h"

class Function {
	string name;
    Type *ret;
    vector<Symbol> args;
    bool def;
public:
	Function(const Function &func);
	~Function();
	Function(AstNode *funDec, Type *type, bool def);
    string getName() const;
};

#endif