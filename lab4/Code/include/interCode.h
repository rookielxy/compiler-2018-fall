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

	IR_FUNC, IR_LABEL, IR_ASSIGN,
	IR_ADD, IR_SUB, IR_MUL, IR_DIV,

	IR_GOTO, IR_RETURN, 
	IR_RELOP_EQ, IR_RELOP_NEQ,
	IR_RELOP_GT, IR_RELOP_LT,
	IR_RELOP_GE, IR_RELOP_LE,

	IR_ADDR, IR_LSTAR, IR_RSTAR,
	
	IR_CALL, IR_PARAM, IR_ARGS,
	IR_DEC, IR_BASIC_DEC,

	IR_READ, IR_WRITE
};


class Operand {
protected:
	enum operandType kind;
public:
	Operand() = default;
	enum operandType getType() { return kind; };
	virtual string display() = 0;
	virtual bool isPtr() { return false; }
	friend bool equal(Operand *op1, Operand *op2);
};

bool equal(Operand *op1, Operand *op2);

class Temp : public Operand {
	friend class CodeBlock;
	static vector<Temp*> temps;

	int tempIdx;
	bool ptr;
public:
	Temp(bool ptr): ptr(ptr) {
		kind = OP_TEMP;
		tempIdx = temps.size();
		temps.emplace_back(this);
	}

	string display() { return "t" + to_string(tempIdx); }

	static void reIndex() {
		for (int i = 0; i < temps.size(); ++i)
			temps[i]->tempIdx = i;
	}

	bool isPtr() { return ptr; }

	static void removeTemp(Temp *temp) {
		for (auto it = temps.begin(); it != temps.end(); ++it) {
			if (*it == temp) {
				temps.erase(it);
				return;
			}
		}
	}

	friend bool equal(Temp *t1, Temp *t2);
};

bool equal(Temp *t1, Temp *t2);

class Label : public Operand {
	static int counter;
	int labelIdx;
public:
	Label() {
		kind = OP_LABEL;
		labelIdx = counter;
		++counter;	
	}

	string display() { 
		return "label" + to_string(labelIdx); 
	}

};


class ConstOp : public Operand {
	int value;
public:
	explicit ConstOp(int value) {
		kind = OP_CONST;
		this->value = value;
	}

	string display() { return "#" + to_string(value); }

	int getValue() { return value; }
	friend bool equal(ConstOp *c1, ConstOp *c2);
};

bool equal(ConstOp *c1, ConstOp *c2);

class SymbolOp : public Operand {
	string name;
	bool basic;
public:
	explicit SymbolOp(string name, bool basic) {
		kind = OP_VARIABLE;
		this->name = name;
		this->basic = basic;
	}

	bool isPtr() { return not basic; }
	string display() { return name; }
	friend bool equal(SymbolOp *s1, SymbolOp *s2);
};

bool equal(SymbolOp *s1, SymbolOp *s2);

class FuncOp : public Operand {
	string name;
public:
	explicit FuncOp(string name) {
		kind = OP_FUNC;
		this->name = name;
	}

	string display() { return name; }
};

class RegScheduler;

class InterCode {
	friend class CodeBlock;
	friend class RegScheduler;

	enum interCodeType kind;
	Operand *op1, *op2, *result;
public:
	InterCode() = default;
	InterCode(enum interCodeType kind);
	InterCode(enum interCodeType kind, Operand *op);
	InterCode(enum interCodeType kind, Operand *op1, Operand *op2);
	InterCode(enum interCodeType kind, Operand *op1, Operand *op2, Operand *result);
	enum interCodeType getType() { return kind; }
	Operand* getResult();
	void display();
	void debug();
};


class CodeBlock {
	list<InterCode> code;
public:
	CodeBlock() = default;
	void append(CodeBlock toAdd);
	void append(const InterCode &toAdd);
	enum interCodeType getType() { return code.back().getType(); }
	Operand* getResult();
	void clearCompstDeadCode();
	bool endWithReturn() { return code.back().getType() == IR_RETURN; }
	void display();
	void debug();
	void optimize();
	bool optimizeOneRun();
	bool optimizeOneBlock(list<InterCode>::iterator, list<InterCode>::iterator);
	void redirectResult(Operand *redir);
	vector<list<InterCode>::iterator> splitIntoBlock();

	void assembleFunc();
	void assembleOneBlock(list<InterCode>::iterator, list<InterCode>::iterator);
};

#endif
