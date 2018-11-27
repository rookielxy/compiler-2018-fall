#include "interCode.h"

vector<Temp *> Temp::temps;
int Label::counter = 0;

InterCode::InterCode(enum interCodeType kind):
			kind(kind), op1(nullptr), op2(nullptr), result(nullptr) {
	switch (kind) {
		case IR_READ:
			result = new Temp(false);
			break;
		default: cout << kind << endl; assert(false);
	}
}

InterCode::InterCode(enum interCodeType kind, Operand *op): 
			kind(kind), op1(nullptr), op2(nullptr), result(nullptr) {
	switch (kind) {
		case IR_RSTAR: case IR_CALL:
			result = new Temp(false);
			op1 = op;
			break;
		case IR_ADDR:
			result = new Temp(true);
			op1 = op;
			break;
		case IR_EMPTY: 
		case IR_DEC: case IR_BASIC_DEC:
		case IR_PARAM: case IR_WRITE:
		case IR_GOTO: case IR_LABEL: case IR_FUNC: 
		case IR_RETURN: case IR_ARGS:
			result = op;
			break;
		default: cout << kind << endl; assert(false);
	}
}

InterCode::InterCode(enum interCodeType kind, Operand *op1, Operand *op2):
			kind(kind), op1(nullptr), op2(nullptr), result(nullptr) {
	switch (kind) {
		case IR_DEC: case IR_ADDR:
		case IR_ASSIGN: case IR_LSTAR:
			result = op1;
			this->op1 = op2;
			break;
		case IR_ADD: case IR_SUB: 
			result = new Temp(op1->isPtr() or op2->isPtr());
			this->op1 = op1;
			this->op2 = op2;
			break;
		case IR_MUL: case IR_DIV:
			assert(not (op1->isPtr() and op2->isPtr()));
			result = new Temp(false);
			this->op1 = op1;
			this->op2 = op2;
			break;
		default: cout << kind << endl; assert(false);
	}
}

InterCode::InterCode(enum interCodeType kind, Operand *op1, Operand *op2, Operand *result):
			kind(kind), op1(op1), op2(op2), result(result) {
	switch(kind) {
		case IR_RELOP_EQ: case IR_RELOP_NEQ:
		case IR_RELOP_GE: case IR_RELOP_LE:
		case IR_RELOP_GT: case IR_RELOP_LT:
			break;
		default: assert(false);
	}
}

Operand* InterCode::getResult() {
	if (result == nullptr) {
		cout << kind << endl;
		assert(false);
	}
	return result;
}

static string dict[] = {
	"IR_EMPTY",

	"IR_FUNC", "IR_LABEL", "IR_ASSIGN",
	"IR_ADD", "IR_SUB", "IR_MUL", "IR_DIV",

	"IR_GOTO", "IR_RETURN", 
	"IR_RELOP_EQ", "IR_RELOP_NEQ",
	"IR_RELOP_GT", "IR_RELOP_LT",
	"IR_RELOP_GE", "IR_RELOP_LE",

	"IR_ADDR", "IR_LSTAR", "IR_RSTAR",
	
	"IR_CALL", "IR_PARAM", "IR_ARGS",
	"IR_DEC", "IR_BASIC_DEC",

	"IR_READ", "IR_WRITE"
};

void InterCode::debug() {
	cout << dict[kind] << " ";
	if (result != nullptr)
		cout << result->display() << " ";
	if (op1 != nullptr)
		cout << op1->display() << " ";
	if (op2 != nullptr)
		cout << op2->display();
	cout << endl;
}

void InterCode::display() {
	switch (kind) {
		case IR_EMPTY: case IR_BASIC_DEC: break;
		case IR_FUNC: 
			cout << "FUNCTION " << result->display() << " :" << endl;
			break;
		case IR_LABEL:
			cout << "LABEL " << result->display() << " :" << endl;
			break;
		case IR_ASSIGN:
			cout << result->display() << " := "
				<< op1->display() << endl;
			break;
		case IR_ADD: case IR_SUB: case IR_MUL: case IR_DIV:
			cout << result->display() << " := "
				<< op1->display();
			switch (kind) {
				case IR_ADD: cout << " + "; break;
				case IR_SUB: cout << " - "; break;
				case IR_MUL: cout << " * "; break;
				case IR_DIV: cout << " / "; break;
				default: assert(false);
			}
			cout << op2->display() << endl;
			break;
		case IR_GOTO:
			cout << "GOTO " << result->display() << endl;
			break;
		case IR_ADDR:
			cout << result->display() << " := &"
				<< op1->display() << endl;
			break;
		case IR_LSTAR:
			cout << "*" << result->display() << " := "
				<< op1->display() << endl;
			break;
		case IR_RSTAR:
			cout << result->display() << " := "
				<< "*" << op1->display() << endl;
			break;
		case IR_DEC:
			cout << "DEC " << result->display() << " "
				<< to_string(((ConstOp *)op1)->getValue()) << endl;
			break;
		case IR_READ:
			cout << "READ " << result->display() << endl;
			break;
		case IR_WRITE:
			cout << "WRITE " << result->display() << endl;
			break;
		case IR_RELOP_EQ: case IR_RELOP_NEQ:
		case IR_RELOP_GE: case IR_RELOP_LE:
		case IR_RELOP_GT: case IR_RELOP_LT:
			cout << "IF " << op1->display() << " ";
			switch (kind) {
				case IR_RELOP_EQ: cout << "== "; break;
				case IR_RELOP_NEQ: cout << "!= "; break;
				case IR_RELOP_GT: cout << "> "; break;
				case IR_RELOP_LT: cout << "< "; break;
				case IR_RELOP_GE: cout << ">= "; break;
				case IR_RELOP_LE: cout << "<= "; break;
				default: assert(false);
			}
			cout << op2->display() << " GOTO " << result->display() << endl; 
			break;
		case IR_RETURN:
			cout << "RETURN " << result->display() << endl;
			break;
		case IR_ARGS:
			cout << "ARG " << result->display() << endl;
			break;
		case IR_PARAM:
			cout << "PARAM " << result->display() << endl;
			break;
		case IR_CALL:
			cout << result->display() << " := CALL " << op1->display() << endl;
			break;
		default: cout << kind << endl; assert(false);
	}
}

void CodeBlock::clearCompstDeadCode() {
	bool retStmt = false;
	for (auto it = code.begin(); it != code.end(); ++it) {
		if (it->getType() == IR_RETURN)
			retStmt = true;
	}
	if (retStmt) {
		while (code.back().getType() != IR_RETURN)
			code.pop_back();
	}
}

void CodeBlock::append(CodeBlock toAdd) {
	code.splice(code.end(), toAdd.code);
}

void CodeBlock::append(const InterCode &toAdd) {
	code.emplace_back(toAdd);
}

Operand* CodeBlock::getResult() {
	return code.back().getResult();
}

void CodeBlock::display() {
	for (auto &ele : code)
		ele.display();
}

void CodeBlock::debug() {
	for (auto &ele : code)
		ele.debug();
}

void CodeBlock::optimize() {
	for (auto it = code.begin(); it != code.end();) {
		if (it->kind == IR_EMPTY or it->kind == IR_BASIC_DEC)
			it = code.erase(it);
		else
			++it;
	}
	while (optimizeOneRun());
}

bool CodeBlock::optimizeOneRun() {
	bool optimized = false;

	// precompute constant calculation
	for (auto it = code.begin(); it != code.end(); ++it) {
		switch (it->getType()) {
			case IR_ADD: case IR_SUB: case IR_MUL: case IR_DIV: {
				if (it->op1->getType() != OP_CONST or it->op2->getType() != OP_CONST)
					break;
				optimized = true;
				ConstOp *const1 = (ConstOp*)it->op1, *const2 = (ConstOp*)it->op2;
				int value;
				switch (it->getType()) {
					case IR_ADD: value = const1->getValue() + const2->getValue(); break;
					case IR_SUB: value = const1->getValue() - const2->getValue(); break;
					case IR_MUL: value = const1->getValue() * const2->getValue(); break;
					case IR_DIV: value = const1->getValue() / const2->getValue(); break;
					default: assert(false);
				}
				ConstOp *result = new ConstOp(value);
				delete it->op1;
				delete it->op2;
				it->kind = IR_ASSIGN;
				it->op1 = result;
				it->op2 = nullptr;
				break;
			}
		}
	}

	// seperate into several basic blocks
	auto blockStart = code.begin(), blockEnd = code.begin();
	while (true) {
		blockStart = blockEnd;
		while (blockStart != code.end()) {
			enum interCodeType type = blockStart->kind;
			if (type == IR_FUNC or type == IR_LABEL)
				++blockStart;
			else
				break;
		}
		if (blockStart == code.end())
			break;

		blockEnd = blockStart;
		bool endWithRelop = false;
		while (blockEnd != code.end()) {
			enum interCodeType type = blockEnd->kind;
			if (type == IR_FUNC or type == IR_LABEL)
				break;
			if ((type >= IR_RELOP_LE and type <= IR_RELOP_LE) or
				type == IR_GOTO)
				endWithRelop = true;
			else
				if(endWithRelop) break;
			++blockEnd;
		}
		if (blockEnd == code.end())
				break;

		optimized = optimized or optimizeOneBlock(blockStart, blockEnd);
	}
	return optimized;
}

bool CodeBlock::optimizeOneBlock(list<InterCode>::iterator begin, list<InterCode>::iterator end) {
	auto it = begin;
	vector<Temp*> temps;
	while (it != end) {
		if (it->op1 != nullptr and it->op1->getType() == OP_TEMP)
			temps.emplace_back((Temp*)it->op1);
		if (it->op2 != nullptr and it->op2->getType() == OP_TEMP)
			temps.emplace_back((Temp*)it->op2);
		if (it->result != nullptr and it->result->getType() == OP_TEMP)
			temps.emplace_back((Temp*)it->result);

		switch (it->kind) {
			case IR_ASSIGN: case IR_ADDR:
			case IR_RSTAR:
				it->result->assign();
				it->op1->ref();
				break;
			case IR_CALL: case IR_READ:
				it->result->assign();
				break;
			case IR_LSTAR:
				it->result->ref();
				it->op1->ref();
				break;
			case IR_ADD: case IR_SUB:
			case IR_MUL: case IR_DIV:
				it->result->assign();
				it->op1->ref();
				it->op2->ref();
				break;
			case IR_RETURN: case IR_ARGS:
			case IR_WRITE:
				it->result->ref();
				break;
			case IR_RELOP_EQ: case IR_RELOP_NEQ:
			case IR_RELOP_LT: case IR_RELOP_GT:
			case IR_RELOP_LE: case IR_RELOP_GE:
				it->op1->ref();
				it->op2->ref();
				break;
			case IR_GOTO: case IR_DEC:
			case IR_PARAM:
				break;
			case IR_LABEL: case IR_FUNC:
			case IR_EMPTY: case IR_BASIC_DEC:
			default: cout << it->kind << endl; assert(false);
		}
		++it;
	}

}