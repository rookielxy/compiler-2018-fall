#include "symbolTable.h"
#include "ast.h"

void reportError(int type, string msg, int line_no) {
    cout << "Error type " << type << " at Line "
        << line_no << ": " << msg << endl;
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

void SymbolTable::enterScope(const Function &func) {
	Scope newScope;
	scopes.emplace_back(newScope);
	for (auto arg: func.getArgs())
		defineSymbol(arg);
}

void SymbolTable::leaveScope() {
    scopes.pop_back();
}

void SymbolTable::defineStruct(const Type &type) {
	if (findStruct(type.getStructName()) != nullptr or
		findLocalSymbol(type.getStructName()) != nullptr) {
        string msg("Duplicate name ");                  // defined Structure's name conflict
        msg += "\"" + type.getStructName() + "\".";    	// with previously defined variable
        reportError(16, msg, type.getLineNo());        	// or structure
	} else {
    	decTypes.emplace(type.getStructName(), type);
	}
}

void SymbolTable::defineSymbol(const Symbol &symbol) {
	if (findStruct(symbol.getName()) != nullptr or 
		findLocalSymbol(symbol.getName()) != nullptr) {
		string msg("Redefined variable");
		msg += "\"" + symbol.getName() + "\".";
		reportError(3, msg, symbol.getLineNo());
	} else {
    	scopes.back().symbols.emplace(symbol.getName(), symbol);
	}
}

void SymbolTable::declareFunc(const Function &func) {
	auto funcPtr = findFunc(func.getName());
	if (funcPtr != nullptr) {
		if (not (*funcPtr == func)) {
			string msg = "Inconsistent declaration of function ";
			msg += "\"" + func.getName() + "\"";
			reportError(19, msg, func.getLineNo());
		}
	} else {
		decFunc.emplace(func.getName(), func);
	}
}

void SymbolTable::defineFunc(const Function &func) {
	auto funcPtr = findFunc(func.getName());
	if (funcPtr != nullptr) {
		if (funcPtr->isDef()) {
			string msg = "Redefined function ";
			msg += "\"" + func.getName() + "\"";
			reportError(4, msg, func.getLineNo());
		} else {
			if (not (*funcPtr == func)) {
				string msg = "Inconsistent declaration of function ";
				msg += "\"" + func.getName() + "\"";
				reportError(19, msg, funcPtr->getLineNo());
			}
			decFunc.insert(pair<string, Function>(func.getName(), func));
		}
	} else {
		decFunc.emplace(func.getName(), func);
	}
}

Type* SymbolTable::findStruct(const string &name) {
	auto it = decTypes.find(name);
	if (it != decTypes.end())
		return &it->second;
	else
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

void SymbolTable::checkFunc() {
	for (auto it = decFunc.begin(); it != decFunc.end(); ++it) {
		if (not it->second.isDef()) {
			string msg = "Undefined function ";
			msg += "\"" + it->first + "\"";
			reportError(18, msg, it->second.getLineNo());
		}
	}
}