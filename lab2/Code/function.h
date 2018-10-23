#ifndef __FUNCTION__
#define __FUNCTION__

#include "type.h"
#include "common.h"

class Function {
	string name;
    Type *ret;
    vector<Symbol> args;
    bool def;
    int line_no;
public:
	Function(const Function &func);
	~Function();
	Function(AstNode *funDec, Type *type, bool def);
    string getName() const { return name; }
    bool isDef() const { return def; }
    bool operator==(const Function &func);
    int getLineNo() const { return line_no; }
    vector<Symbol> getArgs() const { return args; }
};

#endif