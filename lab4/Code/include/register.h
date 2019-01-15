#ifndef __REGISTER__
#define __REGISTER__

#include "common.h"

class Operand;
class InterCode;

#define NR_REG 18

enum Reg {
	t0, t1, t2, t3, t4,
	t5, t6, t7, t8, t9,
	s0, s1, s2, s3,
	s4, s5, s6, s7,
	nullReg
};

struct RegSymbol {
	Operand *op;
	int liveness;
	enum Reg reg;
	RegSymbol() {
		op = nullptr;
		liveness = -1;
		reg = nullReg;
	}
};

struct StackValue {
	RegSymbol *regSym;
	int offset;
};

struct Register {
	string name;
	StackValue *content;
	bool unused;
	Register() {
		content = nullptr;
		unused = true;
	}
};

class RegScheduler {
	Register regs[NR_REG];
	vector<RegSymbol> symbols;
	vector<StackValue> stackValue;

	void addSymbol(Operand *op, set<Operand*> &s);
	void noteLiveness(Operand *op, int line);
public:
	RegScheduler(list<InterCode>::iterator, list<InterCode>::iterator);
	enum Reg ensure(string sym);
	static string displayReg(enum Reg);
	void free(enum Reg);
	enum Reg allocate(string sym);
};

#endif