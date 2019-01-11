#include "function.h"
#include "ast.h"
#include "type.h"

Function::Function(AstNode *funDec, const Type &type, bool def): ret(type) {
	assert(funDec->attr == FUNC_VAR or funDec->attr == FUNC_EMPTY);
	name = funDec->first_child->str;
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
	if (not (ret == func.ret))
		return false;
	if (args.size() != func.args.size())
		return false;
	for (int i = 0; i < args.size(); ++i) {
		if (not (args[i] == func.args[i]))
			return false;
	}
	return true;
}

vector<Type> Function::getArgsType() const {
	vector<Type> types;
	for (auto ele : args) {
		auto temp = ele.getType();
		types.emplace_back(temp);
	}
	return types;
}

string Function::getArgsName() const {
	auto types = getArgsType();
	return transferArgsToName(types);
}