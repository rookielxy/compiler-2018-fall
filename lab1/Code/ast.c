#include "ast.h"

extern int out;
struct ast *astRoot = NULL;

void printError(char* msg, char type, int lineno) {
    fprintf(stderr, "Error type \033[31m%c\033[0m at Line \033[31m%d\033[0m: %s\033[0m\n", type, lineno, msg);
}

void reportError(struct ast* root, int lineno) {
    struct ast* child = root->first_child;
    while (child != NULL) {
        if (child->error_type > 0)
            return;
        child = child->first_sibling;
    }
    switch(root->error_type) {
        case 2: printError("Missing \";\"", 'B', lineno); break;
        default: printError("Syntax error", 'B', lineno); printf("%d\n", root->error_type); break;
    }

}

struct ast *newAst(char* name, int n, ...) {
	va_list valist;
    va_start(valist, n);

    struct ast *root = (struct ast*)malloc(sizeof(struct ast));
    root->name = name;
    root->error_type = 0;
    root->first_child = NULL;
    root->first_sibling = NULL;

    if (n > 0) {
        struct ast *child = va_arg(valist, struct ast*);
        root->line_no = child->line_no;
        root->first_child = child;
        int i = 1;
        //printf("%s %s\n", root->name, child->name);
		for (; i < n; i++) {
            child->first_sibling = va_arg(valist, struct ast*);
            child = child->first_sibling;
      		//printf("%s %s\n", root->name, child->name);
		}
    }
    else {
        root->line_no = va_arg(valist, int);
		root->first_child = root->first_sibling = NULL;
	}

	va_end(valist);
	return root;
}

void travesalAst(struct ast *root, int indent) {
	if (root == NULL)
		return;
	if (strcmp(root->name, "EMPTY") == 0)
		return;
    for (int i = 0; i < 2 * indent; ++i)
        printf(" ");
    printf("%s", root->name);
    if (root->first_child == NULL) {
        if (strcmp(root->name, "ID") == 0 || strcmp(root->name, "TYPE") == 0)
            printf(": %s", root->id);
        else if (strcmp(root->name, "INT") == 0)
            printf(": %d", root->ival);
        else if (strcmp(root->name, "FLOAT") == 0)
            printf(": %f", root->fval);
        printf("\n");
        return;
    }
    printf(" (%d)\n", root->line_no);
    struct ast *child = root->first_child;
    while (child != NULL) {
        travesalAst(child, indent + 1);
        child = child->first_sibling;
    }
}
