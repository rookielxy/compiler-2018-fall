#include "ast.h"

extern FILE* yyin;
extern int yylval;
bool syntaxCorrect = true;
bool semanticCorrect = true;

#ifdef YY_DEBUG
extern int yydebug;
#endif

extern int yylex();
extern int yyparse();
extern int yyrestart(FILE *);


int main(int argc , char **argv) {
	if (argc <= 1)
		return 1;
	FILE *f = fopen(argv[1], "r");
	if (f == nullptr) {
		perror(argv[1]);
		return 1;
	}

	yyrestart(f);
#ifdef YY_DEBUG
	yydebug = 1;
#endif
	yyparse();
	if (syntaxCorrect)
		astRoot->travesalAst(0);
	if (syntaxCorrect)
		astRoot->syntaxParse();

	fclose(f);
	return 0;
}
