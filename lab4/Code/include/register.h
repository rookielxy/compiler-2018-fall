#ifndef __REGISTER__
#define __REGISTER__

#include "common.h"

class Operand;
class InterCode;

#define NR_REG 18

enum Register {
	t0, t1, t2, t3, t4,
	t5, t6, t7, t8, t9,
	s0, s1, s2, s3,
	s4, s5, s6, s7,
	nullReg
};

class RegScheduler {
	bool regs[NR_REG];
	map<string, int> index;
	vector<enum Register> map2Reg;
	vector<int> liveness;

	void addSymbol(Operand *op, int &idx);
	void noteLiveness(Operand *op, int line);
public:
	RegScheduler(list<InterCode>::iterator, list<InterCode>::iterator);
	enum Register ensure(string sym);
	static string displayReg(enum Register);
	void free(enum Register);
	enum Register allocate(string sym);
};

#endif