#include "function.h"
#include "ast.h"

Function::Function(const Function& func) {
	name = func.name;
	ret = new Type(*func.ret);
	args = func.args;
	def = func.def;
	line_no = func.line_no;
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
	line_no = funDec->line_no;
}

bool Function::operator==(const Function &func) {
	if (name != func.name)
		return false;
	if (not (*ret == *func.ret))
		return false;
	if (args.size() != func.args.size())
		return false;
	for (int i = 0; i < args.size(); ++i) {
		if (not (args[i] == func.args[i]))
			return false;
	}
	return true;
}