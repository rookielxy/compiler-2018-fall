#include "ast.h"

void AstNode::translate() {
	assert(tag == TAG_PROGRAM);
	CodeBlock code = translateProgram();
	code.display();
	//code.debug();
}

CodeBlock AstNode::translateProgram() {
	assert(tag == TAG_PROGRAM);
	AstNode *extDefList = first_child, 
			*extDef = extDefList->first_child;
	CodeBlock ret;
	while (extDefList->tag != TAG_EMPTY) {
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
			AstNode *funDec = first_child->first_sibling, 
				*compSt = funDec->first_sibling,
				*funId = funDec->first_child;
			ret.append(InterCode(IR_FUNC, new FuncOp(funId->str)));
			CodeBlock toAdd = compSt->translateCompSt();
			ret.append(toAdd);
		}
		break;
		case FUNC_DEC: break;
		default: assert(false);
	}
	return ret;
}

CodeBlock AstNode::translateFunDec() {
	assert(tag == TAG_FUN_DEC);
	CodeBlock code;
	AstNode *funId = first_child;
	switch (attr) {
		case FUNC_VAR: {
			code.append(InterCode(IR_FUNC, new FuncOp(funId->str)));
			AstNode *varList = first_child->first_sibling->first_sibling;
			code.append(varList->translateVarList());
			break;
		}
		case FUNC_EMPTY:
			code.append(InterCode(IR_FUNC, new FuncOp(funId->str)));
			break;
		default: assert(false);
	}
	return code;
}

CodeBlock AstNode::translateVarList() {
	assert(tag == TAG_VAR_LIST);
	AstNode *varList = this, *paramDec = first_child;
	CodeBlock code;
	while (true) {
		code.append(paramDec->translateParamDec());
		if (paramDec->first_sibling == nullptr)
			break;
		varList = paramDec->first_sibling;
		paramDec = varList->first_child;
	}
	return code;
}

CodeBlock AstNode::translateParamDec() {
	assert(tag == TAG_PARAM_DEC);
	CodeBlock code;
	AstNode *specifier = first_child,
			*varDec = specifier->first_sibling;
	auto symbol = new SymbolOp(varDec->str, varDec->type->isBasic());
	code.append(InterCode(IR_PARAM, symbol));
	return code;
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
		ret.append(varDec->translateVarDec());
		if (varDec->first_sibling == nullptr)
			break;
		extDecList = varDec->first_sibling->first_sibling;
		varDec = extDecList->first_child;	
	}
	return ret;
}

CodeBlock AstNode::translateVarDec() {
	assert(tag == TAG_VAR_DEC);
	CodeBlock code;
	if (not type->isBasic()) {
		assert(type->isArray() or type->isStruct());
		int size = type->getTypeSize();
		auto sz = new ConstOp(size);
		auto var = new SymbolOp(str, false);
		code.append(InterCode(IR_DEC, var, sz));
		code.append(InterCode(IR_ADDR, code.getResult()));
		code.append(InterCode(IR_ASSIGN, var, code.getResult()));
	} else {
		code.append(InterCode(IR_BASIC_DEC, new SymbolOp(str, true)));
	}
	return code;
}

CodeBlock AstNode::translateDefList() {
	assert(tag == TAG_DEF_LIST);
	AstNode *defList = this, *def = first_child;
	CodeBlock ret;
	while (defList->tag != TAG_EMPTY) {
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
	while (stmtList->tag != TAG_EMPTY) {
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
		case IF_STMT: case IF_ELSE_STMT:
			return translateCondStmt();
		case WHILE_STMT:
			return translateLoop();
		default: assert(false);
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
			CodeBlock code = varDec->translateVarDec();
			if (code.getType() == IR_BASIC_DEC) {
				CodeBlock expCode = expr->translateExp();
				InterCode assign = InterCode(IR_ASSIGN, code.getResult(), expCode.getResult());
				ret.append(expCode);
				ret.append(assign);
			} else {
				assert(false);						// assign structure or array not supported
			}
			break;
		}
		default: assert(false);
	}
	return ret;
}

CodeBlock AstNode::translateExp() {
	assert(tag = TAG_EXP);
	CodeBlock code;
	switch (attr) {
		case ASSIGN_EXP: {
			AstNode *expr1 = first_child,
					*expr2 = expr1->first_sibling->first_sibling;
			CodeBlock code1 = expr1->translateExp(),
					code2 = expr2->translateExp();
			Operand *x = code1.getResult(),
					*y = code2.getResult();
			code.append(code1);
			code.append(code2);
			if (y->isPtr()) {
				code.append(InterCode(IR_RSTAR, y));
				y = code.getResult();
			}

			if (x->isPtr())
				code.append(InterCode(IR_LSTAR, x, y));
			else
				code.append(InterCode(IR_ASSIGN, x, y));
				
			break;
		}
		case PLUS_EXP: case MINUS_EXP:
		case STAR_EXP: case DIV_EXP: {
			AstNode *expr1 = first_child,
					*expr2 = expr1->first_sibling->first_sibling;
			CodeBlock code1 = expr1->translateExp(),
					code2 = expr2->translateExp();
			Operand *x = code1.getResult(),
					*y = code2.getResult();
			code.append(code1);
			if (x->isPtr()) {
				code.append(InterCode(IR_RSTAR, x));
				x = code.getResult();
			}

			code.append(code2);
			if (y->isPtr()) {
				code.append(InterCode(IR_RSTAR, y));
				y = code.getResult();
			}

			enum interCodeType type;
			switch (attr) {
				case ASSIGN_EXP: type = IR_ASSIGN; break;
				case PLUS_EXP: type = IR_ADD; break;
				case MINUS_EXP: type = IR_SUB; break;
				case STAR_EXP: type = IR_MUL; break;
				case DIV_EXP: type = IR_DIV; break;
				default: assert(false);
			}
			code.append(InterCode(type, x, y));

			break;
		}
		case NEG_EXP: {
			AstNode *expr = first_child->first_sibling;
			CodeBlock code1 = expr->translateExp();
			Operand *x = code1.getResult();
			auto zero = new ConstOp(0);
			
			code.append(code1);
			code.append(InterCode(IR_SUB, zero, x));
			
			break;
		}
		case NEST_EXP: {
			AstNode *expr = first_child->first_sibling;
			CodeBlock code1 = expr->translateExp();
			code.append(code1);
			break;
		}
		case INT_EXP: {
			AstNode *intNode = first_child;
			InterCode line = InterCode(IR_EMPTY, new ConstOp(intNode->ival));
			code.append(line);
			break;
		}
		case ID_EXP: {
			AstNode *id = first_child;
			auto symOp = new SymbolOp(id->str, type->isBasic());
			InterCode line = InterCode(IR_EMPTY, symOp);
			code.append(line);
			break;
		}
		case STRUCT_EXP: {
			AstNode *expr = first_child, 
					*id = expr->first_sibling->first_sibling;
			CodeBlock code1 = expr->translateExp();
			Operand *x = code1.getResult();
			code.append(code1);

			auto offset = new ConstOp(expr->type->getFieldOffset(id->str));
			code.append(InterCode(IR_ADD, x, offset));

			break;
		}
		case ARRAY_EXP: {
			AstNode *expr1 = first_child,
					*expr2 = expr1->first_sibling->first_sibling;
			CodeBlock code1 = expr1->translateExp(),
					code2 = expr2->translateExp();
			Operand *x = code1.getResult(), *y = code2.getResult();
			code.append(code1);
			code.append(code2);

			auto eleSz = new ConstOp(expr1->type->arrayElemType().getTypeSize());
			InterCode offset = InterCode(IR_MUL, y, eleSz);
			code.append(offset);
			code.append(InterCode(IR_ADD, x, offset.getResult()));

			break;
		}
		case FUNC_EMPTY_EXP: {
			AstNode *funId = first_child;
			if (funId->str == "read")
				code.append(InterCode(IR_READ));
			else
				code.append(InterCode(IR_CALL, new FuncOp(str)));
			break;
		}
		case FUNC_ARGS_EXP: {
			AstNode *funId = first_child,
					*args = funId->first_sibling->first_sibling;
			if (funId->str == "write") {
				AstNode *expr = args->first_child;
				assert(expr->first_sibling == nullptr);
				code.append(expr->translateExp());
				code.append(InterCode(IR_WRITE, code.getResult()));
			} else {
				code.append(args->translateArgs());
				code.append(InterCode(IR_CALL, new FuncOp(str)));
			}
			break;
		}
		case REL_EXP: case NOT_EXP: 
		case AND_EXP: case OR_EXP:
		case FLOAT_EXP:
		default: assert(false);
	}
	return code;
}

CodeBlock AstNode::translateCondStmt() {
	assert(tag == TAG_STMT);
	assert(attr == IF_STMT or attr == IF_ELSE_STMT);
	CodeBlock code;
	AstNode *expr = first_child->first_sibling->first_sibling;
	assert(expr->attr == REL_EXP);

	AstNode *stmt = expr->first_sibling->first_sibling;
	CodeBlock stmtCode = stmt->translateStmt();

	switch(attr) {
		case IF_STMT: {
			auto label = new Label();
			code.append(expr->translateCondExp(nullptr, label));
			code.append(stmtCode);
			code.append(InterCode(IR_LABEL, label));
			break;
		}
		case IF_ELSE_STMT: {
			AstNode *elseStmt = stmt->first_sibling->first_sibling;
			CodeBlock elseCode = elseStmt->translateStmt();
			
			auto label1 = new Label();
			auto label2 = new Label();
			code.append(expr->translateCondExp(nullptr, label1));
			code.append(stmtCode);
			code.append(InterCode(IR_GOTO, label2));
			code.append(InterCode(IR_LABEL, label1));
			code.append(elseCode);
			code.append(InterCode(IR_LABEL, label2));

			break;
		}
		default: assert(false);
	}
	return code;
}

CodeBlock AstNode::translateCondExp(Label *labelTrue, Label *labelFalse) {
	assert(tag == TAG_EXP);
	assert(attr == REL_EXP or attr == NOT_EXP
			or attr == AND_EXP or attr == OR_EXP);
	assert(labelTrue != nullptr or labelFalse != nullptr);
			
	CodeBlock code;
	switch (attr) {
		case REL_EXP: {
			AstNode *expr1 = first_child,
					*relop = expr1->first_sibling,
					*expr2 = relop->first_sibling;

			CodeBlock code1 = expr1->translateExp(),
					code2 = expr2->translateExp();
			Operand *x = code1.getResult(), *y = code2.getResult();
			code.append(code1);
			code.append(code2);

			enum interCodeType relopType = relop->relopType();

			if (labelTrue == nullptr) {
				relopType = relopTypeReverse(relopType);
				code.append(InterCode(relopType, x, y, labelFalse));
			} else if (labelFalse == nullptr) {
				code.append(InterCode(relopType, x, y, labelTrue));
			} else {
				code.append(InterCode(relopType, x, y, labelTrue));
				code.append(InterCode(IR_GOTO, labelFalse));
			}

			break;
		}
		case AND_EXP: case OR_EXP: {
			AstNode *expr1 = first_child,
					*binop = expr1->first_sibling,
					*expr2 = binop->first_sibling;
			
			switch (attr) {
				case AND_EXP:
					if (labelFalse != nullptr) {
						code.append(expr1->translateCondExp(nullptr, labelFalse));
						code.append(expr2->translateCondExp(labelTrue, labelFalse));
					} else {
						Label *label = new Label();
						code.append(expr1->translateCondExp(nullptr, label));
						code.append(expr2->translateCondExp(labelTrue, nullptr));
						code.append(InterCode(IR_LABEL, label));
					}
					break;
				case OR_EXP:
					if (labelTrue != nullptr) {
						code.append(expr1->translateCondExp(labelTrue, nullptr));
						code.append(expr2->translateCondExp(labelTrue, labelFalse));
					} else {
						Label *label = new Label();
						code.append(expr1->translateCondExp(label, nullptr));
						code.append(expr2->translateCondExp(nullptr, labelFalse));
						code.append(InterCode(IR_LABEL, label));
					}
					break;
				default: assert(false);
			}

			break;
		}
		case NOT_EXP: {
			AstNode *expr = first_child->first_sibling;
			code.append(expr->translateCondExp(labelFalse, labelFalse));
			break;
		}
		default: assert(false);
	}
	return code;
}

CodeBlock AstNode::translateLoop() {
	assert(tag == TAG_STMT);
	assert(attr == WHILE_STMT);
	CodeBlock code;
	AstNode *expr = first_child->first_sibling->first_sibling;
	assert(expr->attr == REL_EXP);

	auto begin = new Label();
	auto end = new Label();

	AstNode *stmt = expr->first_sibling->first_sibling;
	CodeBlock stmtCode = stmt->translateStmt();

	code.append(InterCode(IR_LABEL, begin));
	code.append(expr->translateCondExp(nullptr, end));
	code.append(stmtCode);
	code.append(InterCode(IR_GOTO, begin));
	code.append(InterCode(IR_LABEL, end));

	return code;
}

CodeBlock AstNode::translateArgs() {
	assert(tag == TAG_ARGS);
	CodeBlock code;
	AstNode *args = this, *expr = first_child;
	while (true) {
		code.append(expr->translateExp());
		code.append(InterCode(IR_ARGS, code.getResult()));
		if (expr->first_sibling == nullptr)
			break;
		args = expr->first_sibling;
		expr = args->first_child;
	}
	return code;
}