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

struct StackValue {
	int offset;
	int size;
};

struct RegSymbol {
	Operand *op;
	StackValue *onStack;
	int liveness;
	enum Reg reg;
	RegSymbol() {
		op = nullptr;
		onStack = nullptr;
		liveness = -1;
		reg = nullReg;
	}
};


struct Register {
	string name;
	RegSymbol *content;
	bool unused;
	Register() {
		content = nullptr;
		unused = true;
	}
};

extern vector<StackValue> stackValue;

class RegScheduler {
	Register regs[NR_REG];
	vector<RegSymbol> symbols;
	void addSymbol(Operand *op, set<Operand*> &s);
	void noteLiveness(Operand *op, int line);
	void spill(enum Reg, bool);
	void free(enum Reg);

public:
	RegScheduler(list<InterCode>::iterator, list<InterCode>::iterator);
	void addStackValue(Operand *op, int size);
	void addParamValue(Operand *op);

	enum Reg ensure(Operand *op, int line);
	enum Reg allocate(Operand *op, int line);
	void try_free(enum Reg, int line);
	int operandStackOffset(Operand *op);
};

string displayReg(enum Reg);

inline void printInstruction(string str) { cout << '\t' << str << endl; }
inline void printInstruction(string str1, string str2) { cout << '\t' << str1 << ' ' << str2 << endl; }
inline void printInstruction(string str1, string str2, string str3) {
	cout << '\t' << str1 << ' ' << str2 << ", " << str3 << endl; 
}
inline void printInstruction(string str1, string str2, string str3, string str4) {
	cout << '\t' << str1 << ' ' << str2 << ",  " << str3 << ", " << str4 << endl;
}
inline void printLabel(string str) { cout << str << ':' << endl; }


#endif