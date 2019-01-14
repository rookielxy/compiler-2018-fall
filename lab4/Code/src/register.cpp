#include "register.h"
#include "interCode.h"


static string dict[] = {
	"t0", "t1", "t2", "t3", "t4",
	"t5", "t6", "t7", "t8", "t9",
	"s0", "s1", "s2", "s3",
	"s4", "s5", "s6", "s7"
};

RegScheduler::RegScheduler(list<InterCode>::iterator begin, list<InterCode>::iterator end) {
	memset(regs, false, NR_REG*sizeof(bool));
	int idx = 0, line = 0;
	for (auto it = begin; it != end; ++it) {
		addSymbol(it->result, idx);
		addSymbol(it->op1, idx);
		addSymbol(it->op2, idx);
		++line;
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

string RegScheduler::displayReg(enum Register reg) {
	return dict[reg];
}

void RegScheduler::addSymbol(Operand *op, int &idx) {
	if (op->isSym()) {
		string symName = op->display();
		if (index.find(symName) == index.end()) {
			index.insert(pair<string, int>(symName, idx));
			++idx;
			map2Reg.emplace_back(nullReg);
			liveness.emplace_back(-1);
		}
	}
}

void RegScheduler::noteLiveness(Operand *op, int line) {
	if (op->isSym()) {
		string symName = op->display();
		auto it = index.find(symName);
		assert(it != index.end());
		if (liveness[it->second] == - 1)
			liveness[it->second] = line;
	}
}