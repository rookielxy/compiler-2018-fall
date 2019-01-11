#ifndef __FUNCTION__
#define __FUNCTION__

#include "type.h"
#include "common.h"

class Function {
	string name;
    Type ret;
    vector<Symbol> args;
    bool def;
    int line_no;
public:
    Function() = default;
	Function(const Function &func) = default;
	~Function() = default;
	Function& operator=(const Function &func) = default;

	Function(AstNode *funDec, const Type &type, bool def);
    Function(const string &name, const Type &type, vector<Symbol> args): 
            name(name), ret(type), args(args), def(true) {}
    string getName() const { return name; }
    bool isDef() const { return def; }
    bool operator==(const Function &func);
    int getLineNo() const { return line_no; }
    vector<Symbol> getArgs() const { return args; }
    vector<Type> getArgsType() const;
    string getArgsName() const;
    Type getRetType() const { return ret; }
};

#endif