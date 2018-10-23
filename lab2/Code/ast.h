#ifndef __AST__
#define __AST__

#include "common.h"
#include "symbolTable.h"

enum Tag {
    TAG_PROGRAM, TAG_EXT_DEF_LIST, TAG_EXT_DEF,
    TAG_ERROR, TAG_EXT_DEC_LIST, TAG_SPECIFIER,
    TAG_STRUCT_SPECIFIER, TAG_OPT_TAG, TAG_EMPTY,
    TAG_TAG, TAG_VAR_DEC, TAG_FUN_DEC, TAG_VAR_LIST,
    TAG_PARAM_DEC, TAG_COMPST, TAG_STMT_LIST,
    TAG_STMT, TAG_DEF_LIST, TAG_DEF, TAG_DEC_LIST,
    TAG_DEC, TAG_EXP, TAG_ARGS,

    TAG_TYPE, TAG_STRUCT, TAG_IF, TAG_ELSE,
    TAG_WHILE, TAG_RETURN, 
    TAG_INT, TAG_OCT, TAG_HEX, TAG_FLOAT,
    TAG_ID, TAG_SEMI, TAG_COMMA, TAG_ASSIGNOP,
    TAG_RELOP, TAG_PLUS, TAG_MINUS, TAG_STAR,
    TAG_DIV, TAG_AND, TAG_OR, TAG_DOT, TAG_NOT,
    TAG_LP, TAG_RP, TAG_LB, TAG_RB, TAG_LC, TAG_RC,
};

enum Attr {
    DUMMY, DEC_LIST, VOID_DEC, FUNC_DEF, FUNC_DEC,
    STRUCT_DEF, STRUCT_DEC,
    FUNC_VAR, FUNC_EMPTY,
    EMPTY_DEC, ASSIGN_DEC
};

class AstNode {
    static SymbolTable symTable;

public:
    enum Tag tag;
    AstNode* first_child;
    AstNode* first_sibling;
    union {
        string str;
        int ival;
        float fval;
    };
    int line_no;
    enum Attr attr;
    
    AstNode(enum Tag, int, ...);
    void extraInfo(enum Tag, char *yytext);
    void travesalAst(int);
    void syntaxParse();
    void parseExtDef();
    Type* parseSpecifier();
    void parseExtDecList(Type *type);
    vector<Symbol> parseDefList(bool assign);
    vector<Symbol> parseVarList();
    void parseDecList(vector<Symbol> &symbol, Type *type, bool assign);
};

extern string DICT[];
extern AstNode *astRoot;

#endif
