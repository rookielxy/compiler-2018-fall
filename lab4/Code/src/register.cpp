#include "register.h"
#include "interCode.h"


static string dict[] = {
	"t0", "t1", "t2", "t3", "t4",
	"t5", "t6", "t7", "t8", "t9",
	"s0", "s1", "s2", "s3",
	"s4", "s5", "s6", "s7"
};

RegScheduler::RegScheduler(list<InterCode>::iterator begin, list<InterCode>::iterator end) {
	int idx = 0;
	for (auto it = begin; it != end; ++it) {
		addSymbol(it->result, idx);
		addSymbol(it->op1, idx);
		addSymbol(it->op2, idx);
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