#include "symbol.h"
#include "ast.h"

Symbol::Symbol(AstNode *param) {
    AstNode *specifier = param->first_child, *varDec = specifier->first_sibling,
            *id = varDec;
    *this = Symbol(varDec, specifier->parseSpecifier());
}

Symbol::Symbol(AstNode *varDec, const Type &type) {
	AstNode *child = varDec->first_child;
	assert(child->tag == TAG_ID or child->tag == TAG_VAR_DEC);
	
	if (child->tag == TAG_ID)
		this->type = type;
	else
		this->type = Type(varDec, type);
	while (child->tag == TAG_VAR_DEC)
		child = child->first_child;

	line_no = child->line_no;
	name = child->str;
}