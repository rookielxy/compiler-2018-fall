#include "interCode.h"

Temp::Temp() {
	kind = OP_TEMP;
	tempIdx = temps.size();
	symbol = nullptr;
	temps.emplace_back(this);
}

Label::Label() {
	kind = OP_LABEL;
	labelIdx = counter;
	++counter;
	symbol = nullptr;
}