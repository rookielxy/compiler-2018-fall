#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include "common.h"
#include "ast.h"

typedef struct Type_ *Type;
typedef struct Field *FieldList;
typedef struct Symbol *SymbolList;
typedef struct Function *Func;
typedef struct Scope *ScopeList;

struct Type_ {
	enum { BASIC, ARRAY, STRUCTURE } kind;
	union {
		enum { TYPE_INT, TYPE_FLOAT } basic;
		struct {
			Type elem;
			int size;
		} array;
		struct {
			char name[STRING_LENGTH];
			FieldList field;
		} structure;
	} u;
	Type next;
};

struct Field {
	char str[STRING_LENGTH];
	Type type;
	FieldList next;
};

struct Function {
	Type ret;
	FieldList args;
};

struct Symbol {
	enum {
		VAR,
		DEF_FUNC,
		DEC_FUNC
	} kind;
	char str[STRING_LENGTH];
	union {
		Type type;
		Func func;
	} u;
	SymbolList next;
};

struct Scope {
	SymbolList symList;
	Type typeList;
	ScopeList next;
};

extern ScopeList scopeHead;

void insertStruct(Type type);
Type searchStruct(char *name);
void insertSymbol(SymbolList symbol);
SymbolList searchSymbol(char *name);

void initSymTable();
void pushLocalScope();
void popLocalScope();
void deleteSymbol(SymbolList symList);
void deleteType(Type typeList);
void deleteScope(ScopeList p);

#endif
