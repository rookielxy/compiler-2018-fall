#include "ast.h"

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
	AstNode *defList = first_child->first_sibling,
			*stmtList = defList->first_sibling;
	list<InterCode> code1 = defList->translateDefList();
	list<InterCode> code2 = stmtList->translateStmtList();
	code1.splice(code1.end(), code2);
	return code1;
}

list<InterCode> AstNode::translateExtDecList() {
	assert(tag == TAG_EXT_DEC_LIST);
	AstNode *extDecList = this, *varDec = first_child;
	list<InterCode> ret;
	while (true) {
		list<InterCode> code = varDec->translateVarDec();
		ret.splice(ret.end(), code);
		if (varDec->first_sibling == nullptr)
			break;
		extDecList = varDec->first_sibling->first_sibling;
		varDec = extDecList->first_child;	
	}
	return ret;
}

list<InterCode> AstNode::translateVarDec() {
	assert(tag == TAG_VAR_DEC);
	AstNode *id = first_child;
	Symbol *symbol = symTable.findGlobalSymbol(id->str);
	Type type = symbol->getType();
	list<InterCode> ret;
	if (not type.isBasic()) {
		assert(type.isArray() or type.isStruct());
		int size = type.getTypeSize();
		auto sz = new ConstOp(size);
		auto var = new SymbolOp(symbol);
		ret.emplace_back(InterCode(IR_DEC, var, sz));
	}
	return ret;
}

list<InterCode> AstNode::translateDefList() {
	assert(tag == TAG_DEF_LIST);
	AstNode *defList = this, *def = first_child;
	list<InterCode> ret;
	while (def->tag != TAG_EMPTY) {
		list<InterCode> code = def->translateDef();
		ret.splice(ret.end(), code);
		defList = def->first_sibling;
		def = defList->first_child;
	}
	return ret;
}

list<InterCode> AstNode::translateStmtList() {
	assert(tag == TAG_STMT_LIST);
}

list<InterCode> AstNode::translateDef() {
	assert(tag == TAG_DEF);
	AstNode *decList = first_child->first_sibling,
			*dec = decList->first_child;
	list<InterCode> ret;
	while (true) {
		list<InterCode> code = dec->translateDec();
		ret.splice(ret.end(), code);
		if (dec->first_sibling == nullptr)
			break;
		decList = dec->first_sibling->first_sibling;
		dec = decList->first_child;
	}
	return ret;
}

list<InterCode> AstNode::translateDec() {
	assert(tag == TAG_DEC);
	list<InterCode> ret;
	AstNode *varDec = first_child;
	switch(attr) {
		case EMPTY_DEC:
			ret = varDec->translateVarDec();
			break;
		case ASSIGN_DEC: {
			ret = varDec->translateVarDec();
			
		}
		break;
		default: assert(false);
	}

}