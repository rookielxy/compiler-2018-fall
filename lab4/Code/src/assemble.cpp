#include "ast.h"

void AstNode::assemble() {
	assert(tag == TAG_PROGRAM);
	AstNode *extDefList = first_child,
			*extDef = extDefList->first_child;
	while (extDefList->tag != TAG_EMPTY) {
		extDef->assembleExtDef();
		extDefList = extDef->first_sibling;
		extDef = extDefList->first_child;
	}
}

void AstNode::assembleExtDef() {
	assert(tag == TAG_EXT_DEF);
	switch (attr) {
		case FUNC_DEF: {
			AstNode *funDec = first_child->first_sibling,
					*compSt = funDec->first_sibling;
			CodeBlock code;
			code.append(funDec->translateFunDec());
			code.append(compSt->translateCompSt());
			code.assembleFunc();
			break;
		}
		default: assert(false);
	}
}

void CodeBlock::assembleFunc() {
	
}

void CodeBlock::assembleOneBlock(list<InterCode>::iterator begin, list<InterCode>::iterator end) {
	
}