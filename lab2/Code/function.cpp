#include "function.h"
#include "ast.h"
#include "type.h"

Function::Function(const Function &func) {
	name = func.name;
	ret = new Type(*func.ret);
	args = func.args;
	def = func.def;
	line_no = func.line_no;
}

Function::~Function() {
	delete ret;
	ret = nullptr;
}

Function& Function::operator=(const Function &func) {
	name = func.name;
	ret = new Type(*func.ret);
	args = func.args;
	def = func.def;
	line_no = func.line_no;
	return *this;
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

vector<Type> Function::getArgsType() const {
	vector<Type> types;
	for (auto ele : args) {
		auto temp = ele.getType();
		types.emplace_back(*temp);
		delete temp;
		temp = nullptr;
	}
	return types;
}

string Function::getArgsName() const {
	auto types = getArgsType();
	return transferArgsToName(types);
}