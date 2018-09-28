#include "ast.h"


extern FILE* yyin;
extern int yylval;
int out;
#ifdef YY_DEBUG
extern int yydebug;
#endif

extern int yylex();
extern int yyparse();
extern int yyrestart(FILE *);


int main(int argc , char **argv) {
    out = 1;
	if (argc <= 1)
		return 1;
	FILE *f = fopen(argv[1], "r");
	if (f == NULL) {
		perror(argv[1]);
		return 1;
	}

	yyrestart(f);
#ifdef YY_DEBUG
	yydebug = 1;
#endif
	yyparse();
    if (out == 1)
	    travesalAst(astRoot, 0);
	return 0;
}
