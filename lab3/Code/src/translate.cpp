#include "ast.h"

void AstNode::translate() {
	assert(tag == TAG_PROGRAM);
}

CodeBlock AstNode::translateProgram() {
	assert(tag == TAG_PROGRAM);
	AstNode *extDefList = first_child, 
			*extDef = extDefList->first_child;
	CodeBlock ret;
	while (extDef->tag != TAG_EMPTY) {
		CodeBlock toAdd = extDef->translateExtDef();
		ret.append(toAdd);
		extDefList = extDef->first_sibling;
		extDef = extDefList->first_child;
	}
	return ret;
}

CodeBlock AstNode::translateExtDef() {
	assert(tag == TAG_EXT_DEF);
	CodeBlock ret;
	switch (attr) {
		case DEC_LIST: {
			AstNode *extDecList = first_child->first_sibling;
			CodeBlock toAdd = extDecList->translateExtDecList();
			ret.append(toAdd);
		}
		break;
		case VOID_DEC: break;
		case FUNC_DEF: {
			AstNode *compSt = first_child->first_sibling->first_sibling;
			CodeBlock toAdd = compSt->translateCompSt();
			ret.append(toAdd);
		}
		break;
		case FUNC_DEC: break;
		default: assert(false);
	}
	return ret;
}

CodeBlock AstNode::translateCompSt() {
	assert(tag == TAG_COMPST);
	AstNode *defList = first_child->first_sibling,
			*stmtList = defList->first_sibling;
	CodeBlock code1 = defList->translateDefList();
	CodeBlock code2 = stmtList->translateStmtList();
	code1.append(code2);
	return code1;
}

CodeBlock AstNode::translateExtDecList() {
	assert(tag == TAG_EXT_DEC_LIST);
	AstNode *extDecList = this, *varDec = first_child;
	CodeBlock ret;
	while (true) {
		InterCode code = varDec->translateVarDec();
		if (code.getType() != IR_BASIC_DEC)
			ret.append(code);
		if (varDec->first_sibling == nullptr)
			break;
		extDecList = varDec->first_sibling->first_sibling;
		varDec = extDecList->first_child;	
	}
	return ret;
}

InterCode AstNode::translateVarDec() {
	assert(tag == TAG_VAR_DEC);
	AstNode *id = first_child;
	Symbol *symbol = symTable.findGlobalSymbol(id->str);
	Type type = symbol->getType();
	if (not type.isBasic()) {
		assert(type.isArray() or type.isStruct());
		int size = type.getTypeSize();
		auto sz = new ConstOp(size);
		auto var = new SymbolOp(symbol);
		return InterCode(IR_DEC, var, sz);
	}
	return InterCode(IR_BASIC_DEC, new SymbolOp(symbol));
}

CodeBlock AstNode::translateDefList() {
	assert(tag == TAG_DEF_LIST);
	AstNode *defList = this, *def = first_child;
	CodeBlock ret;
	while (def->tag != TAG_EMPTY) {
		CodeBlock code = def->translateDef();
		ret.append(code);
		defList = def->first_sibling;
		def = defList->first_child;
	}
	return ret;
}

CodeBlock AstNode::translateStmtList() {
	assert(tag == TAG_STMT_LIST);
	AstNode *stmtList = this, *stmt = first_child;
	CodeBlock ret;
	while (stmt->tag != TAG_EMPTY) {
		ret.append(stmt->translateStmt());
		stmtList = stmt->first_sibling;
		stmt = stmtList->first_child;
	}
	return ret;
}

CodeBlock AstNode::translateStmt() {
	assert(tag == TAG_STMT);
	switch (attr) {
		case EXP_STMT:
			return first_child->translateExp();
		case COMPST_STMT:
			return first_child->translateCompSt();
		case RETURN_STMT: {
			AstNode *expr = first_child->first_sibling;
			CodeBlock expCode = expr->translateExp();
			Operand *expRes = expCode.getResult();
			InterCode retCode = InterCode(IR_RETURN, expRes);
			expCode.append(retCode);
			return expCode;
		}
		case IF_STMT: {
			CodeBlock ret;
			AstNode *expr = first_child->first_sibling->first_sibling,
					*stmt = expr->first_sibling->first_sibling;
			assert(expr->attr == REL_EXP);
			AstNode *exp1 = expr->first_child, 
					*relop = exp1->first_sibling,
					*exp2 = relop->first_sibling;
			CodeBlock code1 = exp1->translateExp(),
					code2 = exp2->translateExp();
			
			enum interCodeType relopType = relop->relopType();
			relopType = relopTypeReverse(relopType);

			ret.append(code1);
			ret.append(code2);
			Operand *x = code1.getResult(), *y = code2.getResult();
			Label *label = new Label();
			InterCode ifCode = InterCode(relopType, x, y, label);
			ret.append(ifCode);
			ret.append(stmt->translateStmt());
			InterCode labelCode = InterCode(IR_LABEL, label);
			ret.append(labelCode);
			return ret;
		}
	}
}

CodeBlock AstNode::translateDef() {
	assert(tag == TAG_DEF);
	AstNode *decList = first_child->first_sibling,
			*dec = decList->first_child;
	CodeBlock ret;
	while (true) {
		CodeBlock code = dec->translateDec();
		ret.append(code);
		if (dec->first_sibling == nullptr)
			break;
		decList = dec->first_sibling->first_sibling;
		dec = decList->first_child;
	}
	return ret;
}

CodeBlock AstNode::translateDec() {
	assert(tag == TAG_DEC);
	CodeBlock ret;
	AstNode *varDec = first_child;
	switch (attr) {
		case EMPTY_DEC:
			ret.append(varDec->translateVarDec());
			break;
		case ASSIGN_DEC: {
			AstNode *expr = varDec->first_sibling->first_sibling;
			InterCode code = varDec->translateVarDec();
			if (code.getType() == IR_BASIC_DEC) {
				CodeBlock expCode = expr->translateExp();
				InterCode assign = InterCode(IR_ASSIGN, code.getResult(), expCode.getResult());
				ret.append(expCode);
				ret.append(assign);
			} else {
				assert(false);						// assign structure or array not supported
			}
		}
		break;
		default: assert(false);
	}
	return ret;
}

CodeBlock AstNode::translateExp() {
	assert(tag = TAG_EXP);
}

