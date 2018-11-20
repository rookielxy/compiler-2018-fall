#include "interCode.h"

vector<Temp *> Temp::temps;
int Label::counter = 0;

InterCode::InterCode(enum interCodeType kind, Operand *op): 
			kind(kind), op1(op), op2(nullptr), result(nullptr) {
	switch (kind) {
		case IR_ADD: case IR_SUB: case IR_MUL: case IR_DIV:
		case IR_RSTAR:
			result = new Temp();
			break;
		case IR_RELOP_EQ: case IR_RELOP_NEQ:
		case IR_RELOP_GE: case IR_RELOP_LE:
		case IR_RELOP_GT: case IR_RELOP_LT:
		case IR_GOTO: case IR_LABEL: case IR_FUNC: 
		case IR_RETURN:					// these kinds of intermediate codes have no result
		default: assert(false);
	}
}

InterCode::InterCode(enum interCodeType kind, Operand *op1, Operand *op2):
			kind(kind), op1(op1), op2(op2), result(nullptr) {
	switch (kind) {
		case IR_DEC: case IR_BASIC_DEC: case IR_ASSIGN:
			result = op1;
			break;
		default: assert(false);
	}
}

InterCode::InterCode(enum interCodeType kind, Operand *op1, Operand *op2, Operand *result):
			kind(kind), op1(op1), op2(op2), result(result) {
	switch(kind) {
		case IR_RELOP_EQ: case IR_RELOP_NEQ:
		case IR_RELOP_GE: case IR_RELOP_LE:
		case IR_RELOP_GT: case IR_RELOP_LT:
			break;
		default: assert(false);
	}
}

Operand* InterCode::getResult() {
	
}

void CodeBlock::append(CodeBlock toAdd) {
	code.splice(code.end(), toAdd.code);
}

void CodeBlock::append(const InterCode &toAdd) {
	code.emplace_back(toAdd);
}

Operand* CodeBlock::getResult() {
	return code.back().getResult();
}