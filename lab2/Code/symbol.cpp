#include "symbol.h"
#include "ast.h"

Symbol::Symbol(AstNode *param) {
    AstNode *specifier = param->first_child, *varDec = specifier->first_sibling,
            *id = varDec;
    while (id->tag == TAG_VAR_DEC)
        id = id->first_child;
    name = id->str;
    type = specifier->parseSpecifier();
}

Symbol::Symbol(const Symbol &symbol) {
	name = symbol.name;
	line_no = symbol.line_no;
	type = new Type(*symbol.type);
}

Symbol::~Symbol() {
	name.~string(); 
	delete type; 
	type = nullptr;
}


Symbol::Symbol(AstNode *varDec, Type *type) {
	AstNode *child = varDec->first_child;
	assert(child->tag == TAG_ID or child->tag == TAG_VAR_DEC);
	
	if (child->tag == TAG_ID)
		this->type = new Type(*type);
	else
		this->type = new Type(varDec, type);
	while (child->tag == TAG_VAR_DEC)
		child = child->first_child;

	line_no = child->line_no;
	name = child->str;
}