#ifndef __INTER_CODE__
#define __INTER_CODE__

#include "common.h"
#include "symbol.h"

enum operandType {
	OP_VARIABLE, OP_TEMP, OP_CONST,
	OP_LABEL, OP_FUNC
};

enum interCodeType {
	IR_FUNC, IR_LABEL, IP_ASSIGN,
	IR_ADD, IR_SUB, IR_MUL, IR_DIV,

	IR_GOTO, IR_RETURN, 
	IR_RELOP_EQ, IR_RELOP_NEQ,
	IR_RELOP_GT, IR_RELOP_LT,
	IR_RELOP_GE, IR_RELOP_LE,
	
	IR_CALL, IR_PARAM, IR_DEC,

	IR_READ, IR_WRITE
};

class Operand {
protected:
	enum operandType kind;
	union {
		int value;
		int tempIdx;
		int labelIdx;
	};
	Symbol *symbol;
public:
};

class Temp : public Operand {
	static vector<Temp *> temps;
public:
	Temp();
};

class Label : public Operand {
	static int counter;
public:
	Label();
};

class InterCode {
	enum interCodeType kind;
	Operand *op1, *op2, *result;
};

#endif