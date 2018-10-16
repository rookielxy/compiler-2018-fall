#include "symbolTable.h"

Type* Scope::findStruct(const string &name) {
	auto it = decTypes.find(name);
	if (it != decTypes.end())
		return &it->second;
	else
		return nullptr;
}

Symbol* Scope::findSymbol(const string &name) {
	auto it = symbols.find(name);
	if (it != symbols.end())
		return &it->second;
	else
		return nullptr;
}

SymbolTable::SymbolTable() {
    Scope global;
    scopes.emplace_back(global);
}

void SymbolTable::enterScope() {
    Scope newScope;
    scopes.emplace_back(newScope);
}

void SymbolTable::leaveScope() {
    scopes.pop_back();
}

void SymbolTable::defineStruct(const Type &type) {
    scopes.back().decTypes.emplace(type.getStructName(), type);
}

void SymbolTable::defineSymbol(const Symbol &symbol) {
    scopes.back().symbols.emplace(symbol.getName(), symbol);
}

void SymbolTable::defineFunc(const Function &func) {
	decFunc.emplace(func.getName(), func);
}

Type* SymbolTable::findLocalStruct(const string &name) {
	return scopes.back().findStruct(name);
}

Type* SymbolTable::findGlobalStruct(const string &name) {
	for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
		auto p = it->findStruct(name);
		if (p != nullptr)
			return p;
	}
	return nullptr;
}

Symbol* SymbolTable::findLocalSymbol(const string &name) {
	return scopes.back().findSymbol(name);
}

Symbol* SymbolTable::findGlobalSymbol(const string &name) {
	for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
		auto p = it->findSymbol(name);
		if (p != nullptr)
			return p;
	}
	return nullptr;
}

Function* SymbolTable::findFunc(const string &name) {
	auto it = decFunc.find(name);
	if (it != decFunc.end())
		return &it->second;
	else
		return nullptr;
}