#include "interCode.h"

vector<Temp *> Temp::temps;
int Label::counter = 0;

Operand* InterCode::getResult() {
	switch (kind) {
		IR_BASIC_DEC: IR_DEC:
			return op1;
		default: assert(false);
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