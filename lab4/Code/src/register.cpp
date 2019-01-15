#include "register.h"
#include "interCode.h"

vector<StackValue> stackValue;

static string dict[] = {
	"$t0", "$t1", "$t2", "$t3", "$t4",
	"$t5", "$t6", "$t7", "$t8", "$t9",
	"$s0", "$s1", "$s2", "$s3",
	"$s4", "$s5", "$s6", "$s7", "null"
};

string displayReg(enum Reg reg) {
	return dict[reg];
}

void RegScheduler::addSymbol(Operand *op, set<Operand*> &s) {
	if (op == nullptr)
		return;
	if (op->getType() == OP_VARIABLE or op->getType() == OP_TEMP or op->getType() == OP_CONST) {
		if (s.find(op) == s.end())
			s.insert(op);
	}
}

void RegScheduler::noteLiveness(Operand *op, int line) {
	if (op == nullptr)
		return;
	if (op->getType() == OP_VARIABLE or op->getType() == OP_TEMP or op->getType() == OP_CONST) {
		for (auto it = symbols.begin(); it != symbols.end(); ++it) {
			if (it->op == op) {
				if (it->liveness == -1)
					it->liveness = line;
				return;
			}
		}
		assert(false);
	}
}

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

void RegScheduler::addStackValue(Operand *op, int size) {
	assert(op != nullptr);
	StackValue s;
	s.size = size;
	if (stackValue.empty())
		s.offset = 0;
	else
		s.offset = stackValue.back().offset + stackValue.back().size;
	stackValue.emplace_back(s);
	printInstruction("addi", "$sp", "$sp", to_string(-size));
}

void RegScheduler::addParamValue(Operand *op) {
	assert(op != nullptr);
	StackValue s;
	s.size = 4;
	if (stackValue.empty())
		s.offset = -4;
	else 
		s.offset = stackValue[0].offset - 4;
	stackValue.insert(stackValue.begin(), s);
}

enum Reg RegScheduler::ensure(Operand *op, int line) {
	assert(op != nullptr);
	auto it = symbols.begin();
	for (; it != symbols.end(); ++it) {
		if (it->op == op)
			break;
	}
	assert(it != symbols.end());
	if (it->reg != nullReg)
		return it->reg;
	it->reg = allocate(op, line);

	if (op->getType() == OP_CONST) {
		ConstOp *con = dynamic_cast<ConstOp*>(op);
		assert(con != nullptr);
		printInstruction("li", dict[it->reg], to_string(con->getValue()));
	} else if (op->getType() == OP_VARIABLE) {
		assert(it->onStack != nullptr);
		int offset = it->onStack->offset;
		printInstruction("lw", dict[it->reg], to_string(offset) + "($fp)");
	} else if (op->getType() == OP_TEMP){
		if (it->onStack != nullptr)
			printInstruction("lw", dict[it->reg], to_string(it->onStack->offset) + "($fp)");
	} else {
		assert(false);
	}
	return it->reg;
}

void RegScheduler::free(enum Reg reg) {
	regs[reg].unused = true;
	RegSymbol *regSym = regs[reg].content;
	regSym->reg = nullReg;
	regs[reg].content = nullptr;
}

enum Reg RegScheduler::allocate(Operand *op, int line) {
	assert (op != nullptr);
	auto it = symbols.begin();
	for (; it != symbols.end(); ++it) {
		if (it->op == op)
			break;
	}
	assert(it != symbols.end());
	for (int i = 0; i < NR_REG; ++i) {
		if (regs[i].unused) {
			regs[i].unused = false;
			regs[i].content = &(*it);
			return (enum Reg)(t0 + i);
		}
	}
	for (int i = 0; i < NR_REG; ++i) {
		if (regs[i].content->liveness < line) {
			spill((enum Reg)(t0 + i), true);
			regs[i].content = &(*it);
			return (enum Reg)(t0 + i);
		}
	}
	int max = -1, idx = -1;
	for (int i = 0; i < NR_REG; ++i) {
		if (regs[i].content->liveness > max) {
			max = regs[i].content->liveness;
			idx = i;
		}
	}
	assert(idx != -1);
	spill((enum Reg)(t0 + idx), false);
	regs[idx].content = &(*it);
	return (enum Reg)(t0 + idx);
}

void RegScheduler::spill(enum Reg reg, bool dead) {
	assert(not regs[reg].unused);
	RegSymbol *content = regs[reg].content;
	if (content->op->getType() == OP_VARIABLE) {
		assert(content->onStack != nullptr);
		int offset = content->onStack->offset;
		printInstruction("sw", dict[reg], to_string(offset) + "($fp)");
	} else if (content->op->getType() == OP_TEMP) {
		if (not dead and content->onStack == nullptr) {
			addStackValue(content->op, 4);
			content->onStack = &stackValue.back();
		}
		if (not dead and content->onStack != nullptr) {
			int offset = content->onStack->offset;
			printInstruction("sw", dict[reg], to_string(offset) + "($fp)");
		}
	} else {
		assert(false);
	}
}

void RegScheduler::try_free(enum Reg reg, int line) {
	assert(reg != nullReg);
	int active = regs[reg].content->liveness;
	if (active <= line)
		free(reg);
}

int RegScheduler::operandStackOffset(Operand *op) {
	assert(op != nullptr);
	auto it = symbols.begin();
	for (; it != symbols.end(); ++it) {
		if (it->op == op)
			break;
	}
	assert(it != symbols.end());
	assert(it->onStack != nullptr);
	return it->onStack->offset;
}