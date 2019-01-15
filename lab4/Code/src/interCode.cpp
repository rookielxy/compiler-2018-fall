#include "interCode.h"

vector<Temp *> Temp::temps;
int Label::counter = 0;

/*
	format of 3-addr code:
	case READ:		read result
	case RSTAR:		result = *op1
	case CALL:		result = call op1
	case ASSIGN:	result = op1
	case ADDR:		result = &op1
	case WRITE:		write op1
	case ARGS:		args op1
	
	case LSTAR:		*op1 = op2
	case ADD:	case SUB:
	case MUL:	case DIV:
					result = op1 binop op2
	
 */
bool equal(Operand *op1, Operand *op2) {
	if (op1->kind != op2->kind)
		return false;
	switch (op1->kind) {
		case OP_VARIABLE:
			return  equal(dynamic_cast<SymbolOp*>(op1), dynamic_cast<SymbolOp*>(op2));
		case OP_TEMP:
			return equal(dynamic_cast<Temp*>(op1), dynamic_cast<Temp*>(op2));
		case OP_CONST:
			return equal(dynamic_cast<ConstOp*>(op1), dynamic_cast<ConstOp*>(op2));
		default: assert(false);
	}
}

bool equal(Temp *t1, Temp *t2) {
	return t1->tempIdx == t2->tempIdx;
}

bool equal(ConstOp *c1, ConstOp *c2) {
	return c1->value == c2->value;
}

bool equal(SymbolOp *s1, SymbolOp *s2) {
	return s1->name == s2->name;
}

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
		case IR_ASSIGN:
			result = new Temp(op->isPtr());
			op1 = op;
			break;
		case IR_ADDR:
			result = new Temp(true);
			op1 = op;
			break;
		case IR_EMPTY: 
		case IR_DEC: case IR_BASIC_DEC:
		case IR_PARAM: 
		case IR_GOTO: case IR_LABEL: case IR_FUNC: 
		case IR_RETURN:
			result = op;
			break;
		case IR_WRITE: case IR_ARGS:
			op1 = op;
			break;
		default: cout << kind << endl; assert(false);
	}
}

InterCode::InterCode(enum interCodeType kind, Operand *op1, Operand *op2):
			kind(kind), op1(nullptr), op2(nullptr), result(nullptr) {
	switch (kind) {
		case IR_DEC: case IR_ADDR:
		case IR_ASSIGN: 
			result = op1;
			this->op1 = op2;
			break;
		case IR_LSTAR:
			this->op1 = op1;
			this->op2 = op2;
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
		case IR_RSTAR:
			assert(not op1->isPtr() and op2->isPtr());
			result = op1;
			this->op1 = op2;
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
			cout << "*" << op1->display() << " := "
				<< op2->display() << endl;
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
			cout << "WRITE " << op1->display() << endl;
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
			cout << "ARG " << op1->display() << endl;
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

void CodeBlock::redirectResult(Operand *redir) {
	assert((code.back().getType() == IR_EMPTY) or (getResult()->getType() == OP_TEMP));
	if (code.back().getType() == IR_EMPTY) {
		append(InterCode(IR_ASSIGN, redir, getResult()));
	} else {
		Temp::removeTemp((Temp*)code.back().result);
		delete code.back().result;
		code.back().result = redir;
	}
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
	if (code.empty())
		return;
	while (optimizeOneRun());
	Temp::reIndex();
}

bool CodeBlock::optimizeOneRun() {
	bool optimized = false;

	// constant folding
	for (auto it = code.begin(); it != code.end(); ++it) {
		switch (it->kind) {
			case IR_ADD: case IR_SUB: case IR_MUL: case IR_DIV: {
				if (it->op1->getType() != OP_CONST or it->op2->getType() != OP_CONST)
					break;
				optimized = true;
				ConstOp *const1 = (ConstOp*)it->op1, *const2 = (ConstOp*)it->op2;
				int value;
				switch (it->kind) {
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

	/*
		applying algebraic identity
		y = x +/- 0 -> y = x
		y = x * 1   -> 
		y = x / 1	->
	*/
	for (auto it = code.begin(); it != code.end(); ++it) {
		switch (it->kind) {
			case IR_ADD: case IR_SUB: case IR_MUL: case IR_DIV: {
				if (it->op1->getType() != OP_CONST and it->op2->getType() != OP_CONST)
					break;
				ConstOp *constOp = (it->op1->getType() == OP_CONST)? 
									(ConstOp*)it->op1 : (ConstOp*)it->op2;
				switch (it->kind) {
					case IR_ADD: case IR_SUB:
						if (constOp->getValue() == 0) {
							optimized = true;
							it->kind = IR_ASSIGN;
							it->op1 = (it->op1->getType() == OP_CONST)? it->op2 : it->op1;
							it->op2 = nullptr;
							delete constOp;
						}
						break;
					case IR_MUL:
						if (constOp->getValue() == 1) {
							optimized = true;
							it->kind = IR_ASSIGN;
							it->op1 = (it->op1->getType() == OP_CONST)? it->op2 : it->op1;
							it->op2 = nullptr;
							delete constOp; 
						}
						break;
					case IR_DIV:
						if (it->op2->getType() == OP_CONST and ((ConstOp*)it->op2)->getValue() == 1) {
							optimized = true;
							it->kind = IR_ASSIGN;
							delete it->op2;
							it->op2 = nullptr;
						}
						break;
					default: assert(false);
				}
				break;
			}
		}
	}
	
	// seperate into several basic blocks
	vector<list<InterCode>::iterator> firstIts = splitIntoBlock();
	for (int i = 0; i < firstIts.size() - 1; ++i) {
		auto blockStart = firstIts[i], blockEnd = firstIts[i + 1];
		if (optimizeOneBlock(blockStart, blockEnd))
			optimized = true;
	}
	
	return optimized;
}

bool CodeBlock::optimizeOneBlock(list<InterCode>::iterator begin, list<InterCode>::iterator end) {
	bool optimized = false;
	for (auto it = begin; it != end; ) {
		switch (it->kind) {
			case IR_ASSIGN: {
				if (it->result->getType() != OP_TEMP) {
					++it;
					break;
				}
				bool findRef = false;
				auto nxt = it;
				++nxt;
				for (; nxt != end; ++nxt) {
					if (nxt->op1 == it->result or nxt->op2 == it->result) {
						findRef = true;
						nxt->op1 = (nxt->op1 == it->result)? it->op1 : nxt->op1;
						nxt->op2 = (nxt->op2 == it->result)? it->op1 : nxt->op2;
					}
				}
				if (findRef) {
					optimized = true;
					Temp::removeTemp((Temp*)it->result);
					delete it->result;
					it->result = nullptr;
					it = code.erase(it);
				} else {
					++it;
				}
				break;
			}
			default: ++it;
		}
	}
	return optimized;
}


vector<list<InterCode>::iterator> CodeBlock::splitIntoBlock() {
	vector<list<InterCode>::iterator> firstIts;
	assert(not code.empty());
	firstIts.emplace_back(code.begin());
	for (auto it = code.begin(); it != code.end(); ) {
		enum interCodeType type = it->kind;
		switch (type) {
			case IR_FUNC: case IR_LABEL:
				firstIts.emplace_back(it++);
				break;
			case IR_RELOP_EQ: case IR_RELOP_NEQ:
			case IR_RELOP_GE: case IR_RELOP_LE:
			case IR_RELOP_GT: case IR_RELOP_LT:
			case IR_GOTO:
				firstIts.emplace_back(++it);
				break;
			default: ++it; break;
		}
	}
	firstIts.emplace_back(code.end());
	return firstIts;
}