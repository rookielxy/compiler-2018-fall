%option yylineno

%{

#include "syntax.tab.h"

//#define LEX_DEBUG

void report(char *msg) {
#ifdef LEX_DEBUG
	if(strcmp(msg, "annotation") == 0)
		printf("%d: annotation\n", yylineno);
	else
		printf("%d: %s: %s\n", yylineno, msg, yytext);
#endif

}

%}

TYPE        int|float
STRUCT      struct
RETURN      return
IF          if
ELSE        else
WHILE       while

delim       [ \n\t\f\r\v]
ws          {delim}+
digit       [0-9]
digit_8     [0-7]
digit_16    [0-9A-Fa-f]
oct         0{digit_8}*
hex         0[Xx]{digit_16}+
dec         [+\-]?[1-9]{digit}*

flo         ({dec}\.{digit}*)|(\.{digit}+)
exp         ({dec}|{flo})[Ee]{dec}

INT         {oct}|{dec}|{hex}
FLOAT       {flo}|{exp}

letter      [_a-zA-Z]

single      \/\/[^\n]*\n
multi       \/\*[^*]*\*+([^*/][^*]*\*+)*\/
anno        {single}|{multi}
leftNoStar	\/\*[^*]*
leftStar    \/\*[^*]*\*+([^*/][^*]*\*+)*([^*/][^*]*)
left		{leftNoStar}|{leftStar}
right       \*\/

ID          {letter}({letter}|{digit})*
SEMI        ;
COMMA       ,
ASSIGNOP    =
RELOP       >|<|>=|<=|==|!=
PLUS        \+
MINUS       -
STAR        \*
DIV         \/
AND         &&
OR          \|\|
DOT         \.
NOT         !
LP          \(
RP          \)
LB          \[
RB          \]
LC          \{
RC          \}

exphd		{dec}|{flo}
wrflo		({exphd}[Ee]{flo})|({exphd}[Ee][^ \n\t;]*)|([Ee]{flo})

%%

{ws}        { /* take no action */ }

{TYPE}      { report("TYPE");
			  struct ast *leaf = yylval.type_ast = newAst("TYPE", 0, yylineno);
			  strcpy(leaf->id, yytext);
			  return TYPE;
			}

{STRUCT}    { report("STRUCT");  yylval.type_ast = newAst("STRUCT", 0, yylineno);     return STRUCT; }
{IF}        { report("IF");      yylval.type_ast = newAst("IF", 0, yylineno);         return IF; }
{ELSE}      { report("ELSE");    yylval.type_ast = newAst("ELSE", 0, yylineno);       return ELSE; }
{WHILE}     { report("WHILE");   yylval.type_ast = newAst("WHILE", 0, yylineno);      return WHILE; }
{RETURN}    { report("RETURN");  yylval.type_ast = newAst("RETURN", 0, yylineno);     return RETURN; }


{INT}       { report("INT");
			  struct ast *leaf = yylval.type_ast = newAst("INT", 0, yylineno);
			  leaf->ival = atoi(yytext);
			  return INT;
			}

{FLOAT}     { report("FLOAT");
			  struct ast *leaf = yylval.type_ast = newAst("FLOAT", 0, yylineno);
			  leaf->fval = atof(yytext);
			  return FLOAT; 
			}

{ID}        { report("ID");
			  struct ast *leaf = yylval.type_ast = newAst("ID", 0, yylineno);
			  strcpy(leaf->id, yytext);
			  return ID; }


{SEMI}      { report("SEMI");    yylval.type_ast = newAst("SEMI", 0, yylineno);       return SEMI; }
{COMMA}     { report("COMMA");   yylval.type_ast = newAst("COMMA", 0, yylineno);      return COMMA; }
{ASSIGNOP}  { report("ASSIGNOP");yylval.type_ast = newAst("ASSIGNOP", 0, yylineno);   return ASSIGNOP; }
{RELOP}     { report("RELOP");   yylval.type_ast = newAst("RELOP", 0, yylineno);      return RELOP; }
{PLUS}      { report("PLUS");    yylval.type_ast = newAst("PLUS", 0, yylineno);       return PLUS; }
{MINUS}     { report("MINUS");   yylval.type_ast = newAst("MINUS", 0, yylineno);      return MINUS; }
{STAR}      { report("STAR");    yylval.type_ast = newAst("STAR", 0, yylineno);       return STAR; }
{DIV}       { report("DIV");     yylval.type_ast = newAst("DIV", 0, yylineno);        return DIV; }
{AND}       { report("AND");     yylval.type_ast = newAst("AND", 0, yylineno);        return AND; }
{OR}        { report("OR");      yylval.type_ast = newAst("OR", 0, yylineno);         return OR; }
{DOT}       { report("DOT");     yylval.type_ast = newAst("DOT", 0, yylineno);        return DOT; }
{NOT}       { report("NOT");     yylval.type_ast = newAst("NOT", 0, yylineno);        return NOT; }
{LP}        { report("LP");      yylval.type_ast = newAst("LP", 0, yylineno);         return LP; }
{RP}        { report("RP");      yylval.type_ast = newAst("RP", 0, yylineno);         return RP; }
{LB}        { report("LB");      yylval.type_ast = newAst("LB", 0, yylineno);         return LB; }
{RB}        { report("RB");      yylval.type_ast = newAst("RB", 0, yylineno);         return RB; }
{LC}        { report("LC");      yylval.type_ast = newAst("LC", 0, yylineno);         return LC; }
{RC}        { report("RC");      yylval.type_ast = newAst("RC", 0, yylineno);         return RC; }

{anno}      { report("annotation"); }
{wrflo}		{ printf("Error type \033[31mA\033[0m at line \033[31m%d\033[0m: Illegal float number \'%s\'\n", yylineno, yytext); }
{left}      { printf("Error type \033[31mA\033[0m at Line \033[31m%d\033[0m: unterminated annotaion \'%s\'", yylineno, yytext); }
{right}     { printf("Error type \033[31mA\033[0m at Line \033[31m%d\033[0m: alone end of multiline annotation \'%s\'\n", yylineno, yytext); }
.           { printf("Error type \033[31mA\033[0m at Line \033[31m%d\033[0m: Myterious characters \'%s\'\n", yylineno, yytext); }

%%
