#ifndef __SYMBOL_TABLE__
#define __SYMBOL_TABLE__

#include "common.h"
#include "symbol.h"
#include "type.h"
#include "function.h"

class Scope {
    friend class SymbolTable;
    map<string, Symbol> symbols;
public:
    Symbol* findSymbol(const string &name);
};

class SymbolTable {
    vector<Scope> scopes;
    map<string, Type> decTypes;
    map<string, Function> decFunc;
public:
    SymbolTable();
    void enterScope();
    void leaveScope();
    void defineStruct(const Type &type);
    void defineSymbol(const Symbol &symbol);
    void defineFunc(const Function &func);
    Type* findStruct(const string &name);
    Symbol* findLocalSymbol(const string &name);
    Symbol* findGlobalSymbol(const string &name);
    Function* findFunc(const string &name);
};


void reportError(int type, string msg, int line_no);
#endif