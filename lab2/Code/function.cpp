#include "function.h"
#include "ast.h"

Function::Function(const Function& func) {
	name = func.name;
	ret = new Type(*func.ret);
	args = func.args;
	def = func.def;
}

Function::~Function() {
	name.~string();
	delete ret;
	ret = nullptr;
	args.~vector();
}

Function::Function(AstNode *funDec, Type *type, bool def) {
	assert(funDec->attr == FUNC_VAR or funDec->attr == FUNC_EMPTY);
	name = funDec->first_child->str;
	ret = new Type(*type);
	this->def = def;
	if (funDec->attr == FUNC_VAR) {
		AstNode *varList = funDec->first_child->first_sibling->first_sibling;
		args = varList->parseVarList();
	}
}

string Function::getName() const {
	return name;
}