#ifndef __SYMBOL_TABLE__
#define __SYMBOL_TABLE__

#include "common.h"
#include "symbol.h"
#include "type.h"
#include "function.h"

class Scope {
    friend class SymbolTable;

    map<string, Symbol> symbols;
    map<string, Type> decTypes;
public:
    Type* findStruct(const string &name);
    Symbol* findSymbol(const string &name);
};

class SymbolTable {
    vector<Scope> scopes;
    map<string, Function> decFunc;
public:
    SymbolTable();
    void enterScope();
    void leaveScope();
    void defineStruct(const Type &type);
    void defineSymbol(const Symbol &symbol);
    void defineFunc(const Function &func);
    Type* findLocalStruct(const string &name);
    Type* findGlobalStruct(const string &name);
    Symbol* findLocalSymbol(const string &name);
    Symbol* findGlobalSymbol(const string &name);
    Function* findFunc(const string &name);
    bool typeDefined(const string &name);
    bool varDefined(const string &name);
};

#endif