#ifndef __AST_H__
#define __AST_H__

#include "common.h"

enum Tag {
    TAG_PROGRAM, TAG_EXT_DEF_LIST, TAG_EXT_DEF,
    TAG_ERROR, TAG_EXT_DEC_LIST, TAG_SPECIFIER,
    TAG_STRUCT_SPECIFIER, TAG_OPT_TAG, TAG_EMPTY,
    TAG_TAG, TAG_VAR_DEC, TAG_FUN_DEC, TAG_VAR_LIST,
    TAG_PARAM_DEC, TAG_COMPST, TAG_STMT_LIST,
    TAG_STMT, TAG_DEF_LIST, TAG_DEF, TAG_DEC_LIST,
    TAG_DEC, TAG_EXP, TAG_ARGS,

    TAG_TYPE, TAG_STRUCT, TAG_IF, TAG_ELSE,
    TAG_WHILE, TAG_RETURN, TAG_INT, TAG_FLOAT,
    TAG_ID, TAG_SEMI, TAG_COMMA, TAG_ASSIGNOP,
    TAG_RELOP, TAG_PLUS, TAG_MINUS, TAG_STAR,
    TAG_DIV, TAG_AND, TAG_OR, TAG_DOT, TAG_NOT,
    TAG_LP, TAG_RP, TAG_LB, TAG_RB, TAG_LC, TAG_RC,

};

struct AstNode {
    int line_no;
    enum Tag tag;
	union {
		string str;
		int ival;
		float fval;
	};
    int error_type;
    AstNode* first_child;
    AstNode* first_sibling;

    AstNode() {
        first_child = nullptr;
        first_sibling = nullptr;
    }

};

extern AstNode *astRoot;
extern string DICT[];

AstNode *newAst(enum Tag, int, ...);
void travesalAst(AstNode*, int);
void reportError(AstNode*, int);
#endif
