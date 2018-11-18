#ifndef __INTER_CODE__
#define __INTER_CODE__

#include "common.h"
#include "symbol.h"

enum operandType {
	OP_VARIABLE, OP_TEMP, OP_CONST,
	OP_LABEL, OP_FUNC
};

enum interCodeType {
	IR_EMPTY,

	IR_FUNC, IR_LABEL, IP_ASSIGN,
	IR_ADD, IR_SUB, IR_MUL, IR_DIV,

	IR_GOTO, IR_RETURN, 
	IR_RELOP_EQ, IR_RELOP_NEQ,
	IR_RELOP_GT, IR_RELOP_LT,
	IR_RELOP_GE, IR_RELOP_LE,

	IR_ADDR, IR_LSTAR, IR_RSTAR,
	
	IR_CALL, IR_PARAM, IR_DEC,

	IR_READ, IR_WRITE
};

class Operand {
protected:
	enum operandType kind;
	union {
		int value;
	};
	Symbol *symbol;
public:
	Operand() = default;
	Operand(int value);
	Operand(Symbol *symbol);
};

class Temp : public Operand {
	static vector<Temp *> temps;
	int tempIdx;
public:
	Temp() {
		kind = OP_TEMP;
		tempIdx = temps.size();
		symbol = nullptr;
		temps.emplace_back(this);
	}
};

class Label : public Operand {
	static int counter;
	int labelIdx;
public:
	Label() {
		kind = OP_LABEL;
		labelIdx = counter;
		++counter;
		symbol = nullptr;	
	}
};

class ConstOp : public Operand {
	int value;
public:
	explicit ConstOp(int value) {
		kind = OP_CONST;
		this->value = value;
	}	
};

class SymbolOp : public Operand {
	Symbol *symbol;
public:
	explicit SymbolOp(Symbol *symbol) {
		kind = OP_VARIABLE;
		this->symbol = symbol;
	}
};

class FuncOp : public Operand {
	Function *func;
public:
	explicit FuncOp(Function *func) {
		kind = OP_FUNC;
		this->func = func;
	}	
};

class InterCode {
	enum interCodeType kind;
	Operand *op1, *op2, *result;
public:
	InterCode() = default;
	InterCode(enum interCodeType kind, Operand *op): 
				kind(kind), op1(op) {}
	
	InterCode(enum interCodeType kind, Operand *op1, Operand *op2): 
				kind(kind), op1(op1), op2(op2) {}
	
};

#endif