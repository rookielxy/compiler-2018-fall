#include "symbol.h"

ScopeList scopeHead = NULL;

void insertStruct(Type type) {
	if(scopeHead->typeList == NULL) 
		scopeHead->typeList = type;
	else {
		type->next = scopeHead->typeList;
		scopeHead->typeList = type;
	}
}

Type searchStruct(char *name) {
	ScopeList scope = scopeHead;
	Type type = NULL;
	bool flag = false;
	while(scope != NULL) {
		type = scope->typeList;
		while(type != NULL) {
			if(strcmp(name, type->u.structure.name) == 0) {
				flag = true;
				break;
			}
			type = type->next; 
		}
		if(flag)
			break;
		scope = scope->next;
	}
	return type;
}

void insertSymbol(SymbolList symbol) {
	if(scopeHead->symList == NULL)
		scopeHead->symList = symbol;
	else {
		symbol->next = scopeHead->symList;
		scopeHead->symList = symbol;
	}
}

SymbolList searchSymbol(char *name) {
	ScopeList scope = scopeHead;
	SymbolList symbol = NULL;
	bool flag = false;
	while(scope != NULL) {
		symbol = scope->symList;
		while(symbol != NULL) {
			if(strcmp(name, symbol->str) == 0) {
				flag = true;
				break;
			}
			symbol = symbol->next;
		}
		if(flag)
			break;
		scope = scope->next;
	}
	return symbol;
}

void initSymTable() {
	ScopeList global = (ScopeList)malloc(sizeof(struct Scope));
	global->next = NULL;
	global->symList = NULL;
	global->typeList = NULL;
	scopeHead = global;
}

void pushLocalScope() {
	ScopeList local = (ScopeList)malloc(sizeof(struct Scope));
	local->symList = NULL;
	local->next = scopeHead;
	local->typeList = NULL;
	scopeHead = local;
}

void popLocalScope() {
	ScopeList p = scopeHead;
	scopeHead = p->next;
	deleteScope(p);
}

void deleteSymbol(SymbolList symbol) {
	if(symbol->kind == VAR) {
		deleteType(symbol->u.type);
		free(symbol);
		return;
	}

	assert(symbol->kind == DEC_FUNC || symbol->kind == DEF_FUNC);
	Func func = symbol->u.func;
	deleteType(func->ret);
	FieldList f = func->args;
	while(f != NULL) {
		FieldList q = f;
		f = f->next;
		deleteType(q->type);
		free(q);
	}
	free(func);
	free(symbol);
}

void deleteType(Type type) {
	if(type->kind == BASIC) {
		free(type);
		return;
	}
	
	if(type->kind == ARRAY) {
		Type p = type->u.array.elem;
		while(p->kind == ARRAY) {
			Type q = p;
			p = p->u.array.elem;
			free(q);
		}
		deleteType(p);
		free(type);
		return;
	}

	assert(type->kind == STRUCTURE);
	FieldList f = type->u.structure.field;
	while(f != NULL) {
		FieldList q = f;
		f = f->next;
		deleteType(q->type);
		free(q);
	}
	free(type);
}

void deleteScope(ScopeList p) {
	// TODO: delete variable definitions in local scope
	// 		since there is no function and 
	//		structure definitions in local scope in C--
	SymbolList symbol = p->symList;
	while(symbol != NULL) {
		SymbolList q = symbol;
		symbol = symbol->next;
		deleteSymbol(q);
	}
	Type type = p->typeList;
	while(type != NULL) {
		Type q = type;
		type = type->next;
		deleteType(q);
	}
}


