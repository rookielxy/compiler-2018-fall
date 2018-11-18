#include "ast.h"

vector<Temp *> Temp::temps;
int Label::counter = 0;

void AstNode::translate() {
	assert(tag == TAG_PROGRAM);
}

list<InterCode> AstNode::translateProgram() {
	assert(tag == TAG_PROGRAM);
	AstNode *extDefList = first_child, 
			*extDef = extDefList->first_child;
	list<InterCode> ret;
	while (extDef->tag != TAG_EMPTY) {
		list<InterCode> toAdd = extDef->translateExtDef();
		ret.splice(ret.end(), toAdd);
		extDefList = extDef->first_sibling;
		extDef = extDefList->first_child;
	}
	return ret;
}

list<InterCode> AstNode::translateExtDef() {
	assert(tag == TAG_EXT_DEF);
	list<InterCode> ret;
	switch (attr) {
		case DEC_LIST: {
			AstNode *extDecList = first_child->first_sibling;
			list<InterCode> toAdd = extDecList->translateExtDecList();
			ret.splice(ret.end(), toAdd);
		}
		break;
		case VOID_DEC: break;
		case FUNC_DEF: {
			AstNode *compSt = first_child->first_sibling->first_sibling;
			list<InterCode> toAdd = compSt->translateCompSt();
			ret.splice(ret.end(), toAdd);
		}
		break;
		case FUNC_DEC: break;
		default: assert(false);
	}
	return ret;
}

list<InterCode> AstNode::translateCompSt() {
	assert(tag == TAG_COMPST);
}

list<InterCode> AstNode::translateExtDecList() {
	assert(tag == TAG_EXT_DEC_LIST);
	AstNode *extDecList = this, *varDec = first_child;
	list<InterCode> ret;
	while (varDec->first_sibling != nullptr) {
		
	}
}