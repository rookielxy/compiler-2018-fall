#include "ast.h"

extern bool syntaxCorrect;

AstNode *astRoot = nullptr;
SymbolTable AstNode::symTable;

string DICT[] = {
    "Program", "ExtDefList", "ExtDef",
    "Error", "ExtDecList", "Specifier",
    "StructSpecifier", "OptTag", "Empty",
    "Tag", "VarDec", "FunDec", "VarList",
    "ParamDec", "CompSt", "StmtList",
    "Stmt", "DefList", "Def", "DecList",
    "Dec", "Exp", "Args",

    "TYPE", "STRUCT", "IF", "ELSE",
    "WHILE", "RETURN", 
    "INT", "INT", "INT", "FLOAT",
    "ID", "SEMI", "COMMA", "ASSIGNOP",
    "RELOP", "PLUS", "MINUS", "STAR",
    "DIV", "AND", "OR", "DOT", "NOT",
    "LP", "RP", "LB", "RB", "LC", "RC"
};

AstNode::AstNode(enum Tag tag, int n, ...) {
    va_list valist;
    va_start(valist, n);

    this->tag = tag;
    first_child = nullptr;
    first_sibling = nullptr;

    if(n > 0) {
        AstNode *child = va_arg(valist, AstNode*);
        line_no = child->line_no;
        first_child = child;
        
        for(int i = 1; i < n; ++i) {
            child->first_sibling = va_arg(valist, AstNode*);
            child = child->first_sibling;
        }
    } else {
        line_no = va_arg(valist, int);
        first_child = first_sibling = nullptr;
    }

    va_end(valist);
}

void AstNode::extraInfo(enum Tag tag, char *yytext) {
    switch (tag) {
        case TAG_ID: case TAG_TYPE:
            str = string(yytext);
            break;
        case TAG_HEX:
            sscanf(yytext, "%x", &ival);
            break;
        case TAG_INT:
            sscanf(yytext, "%d", &ival);
            break;
        case TAG_OCT:
            sscanf(yytext, "%o", &ival);
            break;
        case TAG_FLOAT:
            sscanf(yytext, "%f", &fval);
            break;
        default:
            assert(false);
    }
}

void AstNode::travesalAst(int indent) {
	if(tag == TAG_EMPTY)
		return;
    for(int i = 0; i < 2 * indent; ++i)
        cout << " ";
    cout << DICT[tag];
    if(first_child == nullptr) {
        if (tag == TAG_ID || tag == TAG_TYPE)
            cout << ": " << str;
        else if (tag == TAG_INT)
            cout << ": " << ival;
        else if (tag == TAG_FLOAT)
            cout << ": " << fval;
        cout << endl;
        return;
    }
    cout << " (" << line_no << ")" << endl;;
    AstNode *child = first_child;
    while(child != nullptr) {
        child->travesalAst(indent + 1);
        child = child->first_sibling;
    }
}

void AstNode::syntaxParse() {
    AstNode *extDefList = first_child, *extDef = extDefList->first_child;
    while (extDef->tag != TAG_EMPTY) {
        extDef->parseExtDef();
        extDefList = extDef->first_sibling;
        extDef = extDefList->first_child;
    }
}

void AstNode::parseExtDef() {
    AstNode *specifier = first_child;
    if (attr == FUNC_DEC) {                 // ExtDef -> Specifier FunDec SEMI
        auto func = Function(specifier->first_sibling, Type(specifier), false);

        return;
    } else if (attr == FUNC_DEF) {          // ExtDef -> Specifier FunDec Compst
        auto func = Function(specifier->first_sibling, Type(specifier), true);
        return;
    } else if (attr == VOID_DEC) {          // ExtDef -> Specifier SEMI
        if (specifier->tag == TAG_STRUCT_SPECIFIER)         // structure declaration
            symTable.defineStruct(Type(specifier));
        return;
    } else if (attr == DEC_LIST) {          // ExtDef -> Specifier ExtDecList SEMI
        AstNode *extDecList = specifier->first_sibling;
        extDecList->parseExtDecList(Type(specifier));
        return;
    }
    assert(false);
}

void AstNode::parseExtDecList(const Type &type) {
    AstNode *extDecList = this, *varDec = first_child;
    while (varDec->first_sibling != nullptr) {
        symTable.defineSymbol(Symbol(varDec, type));
        extDecList = varDec->first_sibling->first_sibling;
        varDec = extDecList->first_child;
    }
}

vector<Field> AstNode::parseVarList() {
    AstNode *param = first_child;
    vector<Field> result;
    while (param->first_sibling != nullptr) {
        result.emplace_back(Field(param));
        param = param->first_sibling->first_child;
    }
    return result;
}