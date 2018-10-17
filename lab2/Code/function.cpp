#include "function.h"
#include "ast.h"

Function::Function(AstNode *funDec, const Type &type, bool def) {
	assert(funDec->attr == FUNC_VAR or funDec->attr == FUNC_EMPTY);
	name = funDec->first_child->str;
	ret = type;
	this->def = def;
	if (funDec->attr == FUNC_VAR) {
		AstNode *varList = funDec->first_child->first_sibling->first_sibling;
		args = varList->parseVarList();
	}
}

string Function::getName() const {
	return name;
}