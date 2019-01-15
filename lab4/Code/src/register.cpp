#include "register.h"
#include "interCode.h"


static string dict[] = {
	"$t0", "$t1", "$t2", "$t3", "$t4",
	"$t5", "$t6", "$t7", "$t8", "$t9",
	"$s0", "$s1", "$s2", "$s3",
	"$s4", "$s5", "$s6", "$s7", "null"
};

RegScheduler::RegScheduler(list<InterCode>::iterator begin, list<InterCode>::iterator end) {
	for (int i = 0; i < NR_REG; ++i)
		regs[i].name = dict[i];

	int line = 0;
	set<Operand*> s;
	for (auto it = begin; it != end; ++it) {
		addSymbol(it->result, s);
		addSymbol(it->op1, s);
		addSymbol(it->op2, s);
		++line;
	}
	for (auto it = s.begin(); it != s.end(); ++it) {
		RegSymbol regSym;
		regSym.op = *it;
		symbols.emplace_back(regSym);
	}
	auto it = --end;
	while (true) {
		noteLiveness(it->result, line);
		noteLiveness(it->op1, line);
		noteLiveness(it->op2, line);
		if (it == begin)
			break;
		--it;
		--line;
	}
}

string RegScheduler::displayReg(enum Reg reg) {
	return dict[reg];
}

void RegScheduler::addSymbol(Operand *op, set<Operand*> &s) {
	if (op == nullptr)
		return;
	if (op->getType() == OP_VARIABLE or op->getType() == OP_TEMP) {
		if (s.find(op) == s.end())
			s.insert(op);
	}
}

void RegScheduler::noteLiveness(Operand *op, int line) {
	if (op == nullptr)
		return;
	if (op->getType() == OP_VARIABLE or op->getType() == OP_TEMP) {
		for (auto it = symbols.begin(); it != symbols.end(); ++it) {
			if (it->op == op and it->liveness == -1) {
				it->liveness = line;
				return;
			}
		}
		assert(false);
	}
}