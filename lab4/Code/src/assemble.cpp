#include "ast.h"
#include "register.h"

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
	printInstruction("addi", "$sp", "$sp", "-4");
	printInstruction("sw", "$fp", "0($sp)");
	printInstruction("move", "$fp", "$sp");

	vector<list<InterCode>::iterator> firstIts = splitIntoBlock();
	for (int i = 0; i < firstIts.size() - 1; ++i)
		assembleOneBlock(firstIts[i], firstIts[i + 1]);
	stackValue.clear();
}

void CodeBlock::assembleOneBlock(list<InterCode>::iterator begin, list<InterCode>::iterator end) {
	int line = 0;
	RegScheduler scheduler(begin, end);
	for (auto it = begin; it != end; ++it, ++line) {
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
				enum Reg reg = scheduler.ensure(it->getResult(), line);
				printInstruction("move", displayReg(reg), "$v0");
				break;
			}
			case IR_WRITE: {
				enum Reg reg = scheduler.ensure(it->op1, line);
				printInstruction("move", "$a0", "%s", displayReg(reg));
				printInstruction("addi", "$sp", "$sp", "-4");
				printInstruction("sw", "$ra", "0($sp)");
				printInstruction("jal", "write");
				printInstruction("lw", "$ra", "0($sp)");
				printInstruction("addi", "$sp", "$sp", "4");
				break;
			}
			case IR_ASSIGN: {
				enum Reg reg0 = scheduler.ensure(it->op1, line);
				enum Reg reg1 = scheduler.ensure(it->result, line);
				printInstruction("move", displayReg(reg0), displayReg(reg1));
				scheduler.try_free(reg1, line);
				break;
			}
			case IR_ADD: case IR_SUB: case IR_MUL: case IR_DIV: {
				enum Reg reg0 = scheduler.ensure(it->result, line);
				enum Reg reg1 = scheduler.ensure(it->op1, line);
				enum Reg reg2 = scheduler.ensure(it->op2, line);
				switch (it->kind) {
					case IR_ADD:
						printInstruction("add", displayReg(reg0), displayReg(reg1), displayReg(reg2));
						break;
					case IR_SUB:
						printInstruction("sub", displayReg(reg0), displayReg(reg1), displayReg(reg2));
						break;
					case IR_MUL:
						printInstruction("mul", displayReg(reg0), displayReg(reg1), displayReg(reg2));
						break;
					case IR_DIV:
						printInstruction("div", displayReg(reg1), displayReg(reg2));
						printInstruction("mflo", displayReg(reg0));
						break;
					default: assert(false);
				}
				scheduler.try_free(reg1, line);
				scheduler.try_free(reg2, line);
				break;
			}
			case IR_BASIC_DEC:
				addStackValue(it->result, 4);
				break;
			case IR_DEC: {
				ConstOp *con = dynamic_cast<ConstOp*>(it->op1);
				assert(con != nullptr);
				addStackValue(it->result, con->getValue());
				break;
			}
			case IR_ADDR: {
				enum Reg reg0 = scheduler.ensure(it->result, line);
				printInstruction("la", displayReg(reg0), 
							to_string(scheduler.operandStackOffset(it->op1)) + "($fp)");
				break;
			}
			case IR_RSTAR: {
				enum Reg reg0 = scheduler.ensure(it->result, line);
				enum Reg reg1 = scheduler.ensure(it->op1, line);
				printInstruction("lw", displayReg(reg0), "0(" + displayReg(reg1) + ")");
				scheduler.try_free(reg1, line);
				break;
			}
			case IR_LSTAR: {
				enum Reg reg0 = scheduler.ensure(it->result, line);
				enum Reg reg1 = scheduler.ensure(it->op1, line);
				printInstruction("sw", displayReg(reg1), "0(" + displayReg(reg0) + ")" );
				break;
			}
			case IR_RETURN: {
				enum Reg reg0 = scheduler.ensure(it->result, line);
				printInstruction("move", "$v0", displayReg(reg0));
				printInstruction("move", "$sp", "$fp");
				printInstruction("lw", "$fp", "0($sp)");
				printInstruction("addi", "$sp", "$sp", "4");
				printInstruction("jr", "$ra");
				scheduler.try_free(reg0, line);
				break;
			}
			case IR_ARGS: {
				enum Reg reg0 = scheduler.ensure(it->result, line);
				printInstruction("addi", "$sp", "$sp", "-4");
				printInstruction("sw", displayReg(reg0), "0($sp)");
				scheduler.try_free(reg0, line);
				break;
			}
			case IR_CALL: {
				printInstruction("addi", "$sp", "$sp", "-4");
				printInstruction("sw", "$ra", "0($sp)");
				printInstruction("jal", it->op1->display());
				printInstruction("lw", "$ra", "0($sp)");
				printInstruction("addi", "$sp", "$sp", "4");
				enum Reg reg0 = scheduler.ensure(it->result, line);
				printInstruction("move", displayReg(reg0), "$v0");
				scheduler.try_free(reg0, line);
				break;
			}
			case IR_PARAM: {
				addParamValue(it->result);
				break;
			}
			case IR_RELOP_EQ: case IR_RELOP_NEQ:
			case IR_RELOP_GE: case IR_RELOP_LE:
			case IR_RELOP_LT: case IR_RELOP_GT: {
				string ins;
				switch (it->kind) {
					case IR_RELOP_EQ: ins = "beq"; break;
					case IR_RELOP_NEQ: ins = "bne"; break;
					case IR_RELOP_GT: ins = "bgt"; break;
					case IR_RELOP_LT: ins = "blt"; break;
					case IR_RELOP_GE: ins = "bge"; break;
					case IR_RELOP_LE: ins = "ble"; break;
					default: assert(false);
				}
				enum Reg reg0 = scheduler.ensure(it->op1, line);
				enum Reg reg1 = scheduler.ensure(it->op2, line);
				printInstruction(displayReg(reg0), displayReg(reg1), it->result->display());
				break;
			}
			default: assert(false);
		}
	}
}