#ifndef __FUNCTION__
#define __FUNCTION__

#include "type.h"
#include "common.h"

class Function {
	string name;
    Type ret;
    vector<Field> args;
public:
    string getName() const;
};

#endif