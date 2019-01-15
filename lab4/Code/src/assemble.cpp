#include "ast.h"
#include "register.h"

static inline void printInstruction(string str) { cout << '\t' << str << endl; }
static inline void printInstruction(string str1, string str2) { cout << '\t' << str1 << ' ' << str2 << endl; }
static inline void printInstruction(string str1, string str2, string str3) {
	cout << '\t' << str1 << ' ' << str2 << ", " << str3 << endl; 
}
static inline void printInstruction(string str1, string str2, string str3, string str4) {
	cout << '\t' << str1 << ' ' << str2 << ",  " << str3 << ", " << str4 << endl;
}
static inline void printLabel(string str) { cout << str << ':' << endl; }

void AstNode::assemble() {
	assert(tag == TAG_PROGRAM);

	cout << ".data" << endl
		<< "_prompt: .asciiz \"Entering an integer:\""
		<< "_ret: .asciiz \"\n\""
		<< ".globl main"
		<< ".text";
	printLabel("read");
	printInstruction("li", "$v0", "4");
	printInstruction("la", "$a0", "_prompt");
	printInstruction("syscall");
	printInstruction("li", "$v0", "5");
	printInstruction("syscall");
	printInstruction("jr", "$ra");
	printLabel("write");
	printInstruction("li", "$v0", "1");
	printInstruction("syscall");
	printInstruction("li", "$v0", "4");
	printInstruction("la", "$a0", "ret");
	printInstruction("syscall");
	printInstruction("move", "$v0", "$0");
	printInstruction("jr", "$ra");

	AstNode *extDefList = first_child,
			*extDef = extDefList->first_child;
	while (extDefList->tag != TAG_EMPTY) {
		extDef->assembleExtDef();
		extDefList = extDef->first_sibling;
		extDef = extDefList->first_child;
	}
}

void AstNode::assembleExtDef() {
	assert(tag == TAG_EXT_DEF);
	switch (attr) {
		case FUNC_DEF: {
			AstNode *funDec = first_child->first_sibling,
					*compSt = funDec->first_sibling;
			CodeBlock code;
			code.append(funDec->translateFunDec());
			code.append(compSt->translateCompSt());
			code.assembleFunc();
			break;
		}
		default: assert(false);
	}
}

void CodeBlock::assembleFunc() {
	assert(not code.empty());
	assert(code.front().getType() == IR_FUNC);
	printLabel(code.front().result->display());
	code.pop_front();

	vector<list<InterCode>::iterator> firstIts = splitIntoBlock();
	for (int i = 0; i < firstIts.size() - 1; ++i)
		assembleOneBlock(firstIts[i], firstIts[i + 1]);
}

void CodeBlock::assembleOneBlock(list<InterCode>::iterator begin, list<InterCode>::iterator end) {
	int line = 0;
	RegScheduler scheduler(begin, end);
	for (auto it = begin; it != end; ++it) {
		switch (it->kind) {
			case IR_LABEL: {
				Label *label = dynamic_cast<Label*>(it->getResult());
				assert(label != nullptr);
				printLabel(label->display());
				break;
			}
			case IR_FUNC: assert(false);
			case IR_GOTO: {
				Label *label = dynamic_cast<Label*>(it->getResult());
				assert(label != nullptr);
				printInstruction("j", label->display());
				break;
			}
			case IR_READ: {
				printInstruction("addi", "$sp", "$sp", "-4");
				printInstruction("sw", "$ra", "0($sp)");
				printInstruction("jal", "read");
				printInstruction("lw", "$ra", "0($sp)");
				printInstruction("addi", "$sp", "$sp", "4");
				// TODO
				//enum Register reg =  scheduler.allocate()
				enum Reg reg = nullReg;
				printInstruction("move", RegScheduler::displayReg(reg), "$v0");
				// TODO:
				// spill to memory
				break;
			}
			case IR_WRITE: {
				// TODO:
				enum Reg reg = nullReg;
				printInstruction("move", "$a0", "%s", RegScheduler::displayReg(reg));
				// TODO: free register
				printInstruction("addi", "$sp", "$sp", "-4");
				printInstruction("sw", "$ra", "0($sp)");
				printInstruction("jal", "write");
				printInstruction("lw", "$ra", "0($sp)");
				printInstruction("addi", "$sp", "$sp", "4");
				break;
			}
			default: assert(false);
		}
	} 
}