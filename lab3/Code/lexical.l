%option yylineno

%{

#include "syntax.tab.h"
extern bool syntaxCorrect;

void debug(string msg) {
#ifdef LEX_DEBUG
	if(msg == "annotation")
		cout << yylineno << ": annotation" << endl;
	else
		cout << yylineno << ": " << msg << ": " << yytext << endl;
#endif
}

static void reportError(string msg) {
	syntaxCorrect = false;
	printf("Error type A at line %d: %s %s\n", yylineno, msg.c_str(), yytext);
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
dec         [1-9]{digit}*

float       ({dec}|{hex}|{oct})\.{digit}*|\.{digit}+
base		[0-9]+\.[0-9]*|\.[0-9]+
exp         {base}[Ee][+-]?[0-9]+

FLOAT       {float}|{exp}

letter      [_a-zA-Z]

single      \/\/[^\n]*\n
multi       \/\*[^*]*\*+([^*/][^*]*\*+)*\/
annotation  {single}|{multi}
leftNoStar	\/\*[^*]*
leftStar    \/\*[^*]*\*+([^*/][^*]*\*+)*([^*/][^*]*)
leftAnno	{leftNoStar}|{leftStar}

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

wrong_float	{base}[Ee][^ \n\t;]*|[0-9]*[Ee][+-]?[0-9]+
wrong_hex	0[Xx][0-9A-Za-z]*
wrong_oct	0[0-9A-Za-z]*

%%

{ws}        { /* take no action */ }

{TYPE}      { debug("TYPE");	yylval.type_ast = new AstNode(TAG_TYPE, 0, yytext, yylineno);		return TYPE; }

{STRUCT}    { debug("STRUCT");  yylval.type_ast = new AstNode(TAG_STRUCT, 0, yytext, yylineno);     return STRUCT; }
{IF}        { debug("IF");      yylval.type_ast = new AstNode(TAG_IF, 0, yytext, yylineno);         return IF; }
{ELSE}      { debug("ELSE");    yylval.type_ast = new AstNode(TAG_ELSE, 0, yytext, yylineno);       return ELSE; }
{WHILE}     { debug("WHILE");   yylval.type_ast = new AstNode(TAG_WHILE, 0, yytext, yylineno);      return WHILE; }
{RETURN}    { debug("RETURN");  yylval.type_ast = new AstNode(TAG_RETURN, 0, yytext, yylineno);     return RETURN; }

{oct}       { debug("OCT");
			  yylval.type_ast = new AstNode(TAG_OCT, 0, yytext, yylineno);
			  yylval.type_ast->extraInfo(TAG_OCT, yytext);
			  return INT;
			}

{dec}       { debug("DEC");
			  yylval.type_ast = new AstNode(TAG_INT, 0, yytext, yylineno);
			  yylval.type_ast->extraInfo(TAG_INT, yytext);
			  return INT;
			}


{hex}       { debug("HEX");
			  yylval.type_ast = new AstNode(TAG_HEX, 0, yytext, yylineno);
			  yylval.type_ast->extraInfo(TAG_HEX, yytext);
			  return INT;
			}

{FLOAT}     { debug("FLOAT");
			  yylval.type_ast = new AstNode(TAG_FLOAT, 0, yytext, yylineno);
			  yylval.type_ast->extraInfo(TAG_FLOAT, yytext);
			  return FLOAT; 
			}

{ID}        { debug("ID");		yylval.type_ast = new AstNode(TAG_ID, 0, yytext, yylineno);			return ID; 
			}

{SEMI}      { debug("SEMI");    yylval.type_ast = new AstNode(TAG_SEMI, 0, yytext, yylineno);       return SEMI; }
{COMMA}     { debug("COMMA");   yylval.type_ast = new AstNode(TAG_COMMA, 0, yytext, yylineno);      return COMMA; }
{ASSIGNOP}  { debug("ASSIGNOP");yylval.type_ast = new AstNode(TAG_ASSIGNOP, 0, yytext, yylineno);   return ASSIGNOP; }
{RELOP}     { debug("RELOP");   yylval.type_ast = new AstNode(TAG_RELOP, 0, yytext, yylineno);      return RELOP; }
{PLUS}      { debug("PLUS");    yylval.type_ast = new AstNode(TAG_PLUS, 0, yytext, yylineno);       return PLUS; }
{MINUS}     { debug("MINUS");   yylval.type_ast = new AstNode(TAG_MINUS, 0, yytext, yylineno);      return MINUS; }
{STAR}      { debug("STAR");    yylval.type_ast = new AstNode(TAG_STAR, 0, yytext, yylineno);       return STAR; }
{DIV}       { debug("DIV");     yylval.type_ast = new AstNode(TAG_DIV, 0, yytext, yylineno);        return DIV; }
{AND}       { debug("AND");     yylval.type_ast = new AstNode(TAG_AND, 0, yytext, yylineno);        return AND; }
{OR}        { debug("OR");      yylval.type_ast = new AstNode(TAG_OR, 0, yytext, yylineno);         return OR; }
{DOT}       { debug("DOT");     yylval.type_ast = new AstNode(TAG_DOT, 0, yytext, yylineno);        return DOT; }
{NOT}       { debug("NOT");     yylval.type_ast = new AstNode(TAG_NOT, 0, yytext, yylineno);        return NOT; }
{LP}        { debug("LP");      yylval.type_ast = new AstNode(TAG_LP, 0, yytext, yylineno);         return LP; }
{RP}        { debug("RP");      yylval.type_ast = new AstNode(TAG_RP, 0, yytext, yylineno);         return RP; }
{LB}        { debug("LB");      yylval.type_ast = new AstNode(TAG_LB, 0, yytext, yylineno);         return LB; }
{RB}        { debug("RB");      yylval.type_ast = new AstNode(TAG_RB, 0, yytext, yylineno);         return RB; }
{LC}        { debug("LC");      yylval.type_ast = new AstNode(TAG_LC, 0, yytext, yylineno);         return LC; }
{RC}        { debug("RC");      yylval.type_ast = new AstNode(TAG_RC, 0, yytext, yylineno);         return RC; }

{annotation}   	{ debug("annotation"); }

{wrong_float}	{ reportError("Illegal float number: "); 
				  yylval.type_ast = new AstNode(TAG_ERROR, 0, yytext, yylineno);	
				  return FLOAT;
				}

{wrong_hex}		{ reportError("Illegal hexademical number: ");
				  yylval.type_ast = new AstNode(TAG_ERROR, 0, yytext, yylineno);
				  return INT;
				}

{wrong_oct}		{ reportError("Illegal octal number: ");
				  yylval.type_ast = new AstNode(TAG_ERROR, 0, yytext, yylineno);
				  return INT;
				}

{leftAnno}      { reportError("Unterminated annotation:\n"); }
.           	{ reportError("Mysterious characters: "); }

%%
