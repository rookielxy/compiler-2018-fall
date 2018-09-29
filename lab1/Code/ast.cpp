    #include "ast.h"

extern int out;
AstNode *astRoot = nullptr;

string DICT[] = {
    "Program", "ExtDefList", "ExtDef",
    "Error", "ExtDecList", "Specifier",
    "StructSpecifier", "OptTag", "Empty",
    "Tag", "VarDec", "FunDec", "VarList",
    "ParamDec", "CompSt", "StmtList",
    "Stmt", "DefList", "Def", "DecList",
    "Dec", "Exp", "Args",

    "TYPE", "STRUCT", "IF", "ELSE",
    "WHILE", "RETURN", "INT", "FLOAT",
    "ID", "SEMI", "COMMA", "ASSIGNOP",
    "RELOP", "PLUS", "MINUS", "STAR",
    "DIV", "AND", "OR", "DOT", "NOT",
    "LP", "RP", "LB", "RB", "LC", "RC"
};

void printError(char* msg, char type, int lineno) {
    fprintf(stderr, "Error type \033[31m%c\033[0m at Line \033[31m%d\033[0m: %s\033[0m\n", type, lineno, msg);
}

void reportError(AstNode* root, int lineno) {
    AstNode* child = root->first_child;
    while (child != nullptr) {
        if (child->error_type > 0)
            return;
        child = child->first_sibling;
    }
    switch(root->error_type) {
        case 2: printError("Missing \";\"", 'B', lineno); break;
        default: printError("Syntax error", 'B', lineno); printf("%d\n", root->error_type); break;
    }

}

AstNode *newAst(enum Tag tag, int n, ...) {
	va_list valist;
    va_start(valist, n);

    auto root = new AstNode();
    root->tag = tag;
    root->error_type = 0;
    root->first_child = nullptr;
    root->first_sibling = nullptr;

    if(n > 0) {
        AstNode *child = va_arg(valist, AstNode*);
        root->line_no = child->line_no;
        root->first_child = child;
        
		for(int i = 1; i < n; ++i) {
            child->first_sibling = va_arg(valist, AstNode*);
            child = child->first_sibling;
      		//printf("%s %s\n", root->name, child->name);
		}
    }
    else {
        root->line_no = va_arg(valist, int);
		root->first_child = root->first_sibling = nullptr;
	}

	va_end(valist);
	return root;
}

void travesalAst(AstNode *root, int indent) {
	if (root == nullptr)
		return;
	if (root->tag == TAG_EMPTY)
		return;
    for (int i = 0; i < 2 * indent; ++i)
        cout << " ";
    cout << DICT[root->tag];
    if (root->first_child == nullptr) {
        if (root->tag == TAG_ID || root->tag == TAG_TYPE)
            cout << ": " << root->str;
        else if (root->tag == TAG_INT)
            cout << ": " << root->ival;
        else if (root->tag == TAG_FLOAT)
            cout << ": " << root->fval;
        cout << endl;
        return;
    }
    cout << " (" << root->line_no << ")" << endl;;
    AstNode *child = root->first_child;
    while (child != nullptr) {
        travesalAst(child, indent + 1);
        child = child->first_sibling;
    }
}
