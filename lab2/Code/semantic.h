#ifndef __SEMANTIC__
#define __SEMANTIC__

#include "common.h"
#include "ast.h"
#include "symbol.h"

void parseExtDef(struct ast *root);
void parseCompSt(struct ast *compSt);
SymbolList parseDefList(struct ast *defList);
SymbolList parseDef(struct ast *def);
SymbolList parseDec(struct ast *dec, Type type);
SymbolList parseVarDec(struct ast *varDec, Type type);
FieldList buildVarList(struct ast *varList);
SymbolList buildFunc(struct ast *func, Type type);
Type buildType(struct ast *spec);
SymbolList buildArray(struct ast *varDec, Type type);
Type buildStruct(struct ast *struct_);
FieldList buildFieldList(struct ast *defList);

bool typeEqual(Type s1, Type s2);
bool structEqual(Type s1, Type s2);
bool arrayEqual(Type s1, Type s2);

#endif
