#ifndef __AST_H__
#define __AST_H__

#include "common.h"

struct ast {
    int line_no;
    char* name;
	union {
		char id[20];
		int ival;
		float fval;
	};
    int error_type;
    struct ast* first_child;
    struct ast* first_sibling;

};

extern struct ast *astRoot;

struct ast *newAst(char* , int, ...);
void travesalAst(struct ast*, int);
void reportError(struct ast*, int);
#endif
