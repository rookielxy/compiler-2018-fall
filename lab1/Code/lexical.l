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

float       ({dec}\.{digit}*)|(\.{digit}+)
exp         ({dec}|{float})[Ee]{dec}

INT         {oct}|{dec}|{hex}
FLOAT       {float}|{exp}

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

exphead		{dec}|{float}
wrong_float	({exphead}[Ee]{float})|({exphead}[Ee][^ \n\t;]*)|([Ee]{float})

%%

{ws}        { /* take no action */ }

{TYPE}      { report("TYPE");
			  struct AstNode *leaf = yylval.type_ast = newAst(TAG_TYPE, 0, yylineno);
			  leaf->str = string(yytext);
			  return TYPE;
			}

{STRUCT}    { report("STRUCT");  yylval.type_ast = newAst(TAG_STRUCT, 0, yylineno);     return STRUCT; }
{IF}        { report("IF");      yylval.type_ast = newAst(TAG_IF, 0, yylineno);         return IF; }
{ELSE}      { report("ELSE");    yylval.type_ast = newAst(TAG_ELSE, 0, yylineno);       return ELSE; }
{WHILE}     { report("WHILE");   yylval.type_ast = newAst(TAG_WHILE, 0, yylineno);      return WHILE; }
{RETURN}    { report("RETURN");  yylval.type_ast = newAst(TAG_RETURN, 0, yylineno);     return RETURN; }


{INT}       { report("INT");
			  struct AstNode *leaf = yylval.type_ast = newAst(TAG_INT, 0, yylineno);
			  leaf->ival = atoi(yytext);
			  return INT;
			}

{FLOAT}     { report("FLOAT");
			  struct AstNode *leaf = yylval.type_ast = newAst(TAG_FLOAT, 0, yylineno);
			  leaf->fval = atof(yytext);
			  return FLOAT; 
			}

{ID}        { report("ID");
			  struct AstNode *leaf = yylval.type_ast = newAst(TAG_ID, 0, yylineno);
			  
			  leaf->str = string(yytext);
			  return ID; }


{SEMI}      { report("SEMI");    yylval.type_ast = newAst(TAG_SEMI, 0, yylineno);       return SEMI; }
{COMMA}     { report("COMMA");   yylval.type_ast = newAst(TAG_COMMA, 0, yylineno);      return COMMA; }
{ASSIGNOP}  { report("ASSIGNOP");yylval.type_ast = newAst(TAG_ASSIGNOP, 0, yylineno);   return ASSIGNOP; }
{RELOP}     { report("RELOP");   yylval.type_ast = newAst(TAG_RELOP, 0, yylineno);      return RELOP; }
{PLUS}      { report("PLUS");    yylval.type_ast = newAst(TAG_PLUS, 0, yylineno);       return PLUS; }
{MINUS}     { report("MINUS");   yylval.type_ast = newAst(TAG_MINUS, 0, yylineno);      return MINUS; }
{STAR}      { report("STAR");    yylval.type_ast = newAst(TAG_STAR, 0, yylineno);       return STAR; }
{DIV}       { report("DIV");     yylval.type_ast = newAst(TAG_DIV, 0, yylineno);        return DIV; }
{AND}       { report("AND");     yylval.type_ast = newAst(TAG_AND, 0, yylineno);        return AND; }
{OR}        { report("OR");      yylval.type_ast = newAst(TAG_OR, 0, yylineno);         return OR; }
{DOT}       { report("DOT");     yylval.type_ast = newAst(TAG_DOT, 0, yylineno);        return DOT; }
{NOT}       { report("NOT");     yylval.type_ast = newAst(TAG_NOT, 0, yylineno);        return NOT; }
{LP}        { report("LP");      yylval.type_ast = newAst(TAG_LP, 0, yylineno);         return LP; }
{RP}        { report("RP");      yylval.type_ast = newAst(TAG_RP, 0, yylineno);         return RP; }
{LB}        { report("LB");      yylval.type_ast = newAst(TAG_LB, 0, yylineno);         return LB; }
{RB}        { report("RB");      yylval.type_ast = newAst(TAG_RB, 0, yylineno);         return RB; }
{LC}        { report("LC");      yylval.type_ast = newAst(TAG_LC, 0, yylineno);         return LC; }
{RC}        { report("RC");      yylval.type_ast = newAst(TAG_RC, 0, yylineno);         return RC; }

{anno}      	{ report("annotation"); }
{wrong_float}	{ printf("Error type \033[31mA\033[0m at line \033[31m%d\033[0m: Illegal float number \'%s\'\n", yylineno, yytext); }
{left}      	{ printf("Error type \033[31mA\033[0m at Line \033[31m%d\033[0m: unterminated annotaion \'%s\'", yylineno, yytext); }
{right}     	{ printf("Error type \033[31mA\033[0m at Line \033[31m%d\033[0m: alone end of multiline annotation \'%s\'\n", yylineno, yytext); }
.           	{ printf("Error type \033[31mA\033[0m at Line \033[31m%d\033[0m: Myterious characters \'%s\'\n", yylineno, yytext); }

%%
