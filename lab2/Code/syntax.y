%{

#include "ast.h"
#include "lex.yy.c"

extern int yylex();
extern void yyerror(char *msg);
extern int yylineno;
extern bool syntaxCorrect;

%}

%union {
    AstNode* type_ast;
}

/* Tokens */
%token <type_ast> INT FLOAT ID SEMI COMMA
%token <type_ast> ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR NOT
%token <type_ast> DOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE error

/* High-Level Definitions */
%type <type_ast> Program ExtDefList ExtDef ExtDecList
/* Specifiers */
%type <type_ast> Specifier StructSpecifier OptTag Tag
/* Declarators */
%type <type_ast> VarDec FunDec VarList ParamDec
/* Statements */
%type <type_ast> CompSt StmtList Stmt
/* Loacal Definitions */
%type <type_ast> DefList Def DecList Dec
/* Expressions */
%type <type_ast> Exp Args

/* Priority */
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB DOT

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

/* High-lever Definitions */
Program: ExtDefList                             { $$ = astRoot = new AstNode(TAG_PROGRAM, 1, $1); }
    ;
ExtDefList: ExtDef ExtDefList                   { $$ = new AstNode(TAG_EXT_DEF_LIST, 2, $1, $2); }
    | /* empty */                               { $$ = new AstNode(TAG_EMPTY, 0); }
    ;
ExtDef: Specifier ExtDecList SEMI               { $$ = new AstNode(TAG_EXT_DEF, 3, $1, $2, $3); $$->attr = DEC_LIST; }
    | Specifier SEMI                            { $$ = new AstNode(TAG_EXT_DEF, 2, $1, $2); $$->attr = VOID_DEC; }
    | Specifier FunDec CompSt                   { $$ = new AstNode(TAG_EXT_DEF, 3, $1, $2, $3); $$->attr = FUNC_DEF; }
    | Specifier FunDec SEMI                     { $$ = new AstNode(TAG_EXT_DEF, 3, $1, $2, $3); $$->attr = FUNC_DEC;}
    | Specifier error SEMI                      { $$ = new AstNode(TAG_ERROR, 3, $1, $2, $3); syntaxCorrect = false; }
    | error SEMI                                { $$ = new AstNode(TAG_ERROR, 2, $1, $2); syntaxCorrect = false; }
    ;

ExtDecList: VarDec                              { $$ = new AstNode(TAG_EXT_DEC_LIST, 1, $1); }
    | VarDec COMMA ExtDecList                   { $$ = new AstNode(TAG_EXT_DEC_LIST, 3, $1, $2, $3); }

/* Specifiers */
Specifier: TYPE                                 { $$ = new AstNode(TAG_SPECIFIER, 1, $1); }
    | StructSpecifier                           { $$ = new AstNode(TAG_SPECIFIER, 1, $1); }
    ;
StructSpecifier: STRUCT OptTag LC DefList RC    { $$ = new AstNode(TAG_STRUCT_SPECIFIER, 5, $1, $2, $3, $4, $5); $$->attr = STRUCT_DEF; }
    | STRUCT Tag                                { $$ = new AstNode(TAG_STRUCT_SPECIFIER, 2, $1, $2); $$->attr = STRUCT_DEC; }
    ;
OptTag: ID                                      { $$ = new AstNode(TAG_OPT_TAG, 1, $1); }
    | /*empty*/                                 { $$ = new AstNode(TAG_EMPTY, 0); }
    ;
Tag: ID                                         { $$ = new AstNode(TAG_TAG, 1, $1); }
    ;

/* Declarators */
VarDec: ID                                      { $$ = new AstNode(TAG_VAR_DEC, 1, $1); }
    | VarDec LB INT RB                          { $$ = new AstNode(TAG_VAR_DEC, 4, $1, $2, $3, $4); }
    ;
FunDec: ID LP VarList RP                        { $$ = new AstNode(TAG_FUN_DEC, 4, $1, $2, $3, $4); $$->attr = FUNC_VAR; }
    | ID LP RP                                  { $$ = new AstNode(TAG_FUN_DEC, 3, $1, $2, $3); $$->attr = FUNC_EMPTY; }
    ;
VarList: ParamDec COMMA VarList                 { $$ = new AstNode(TAG_VAR_LIST, 3, $1, $2, $3); }
    | ParamDec                                  { $$ = new AstNode(TAG_VAR_LIST, 1, $1); }
    ;
ParamDec: Specifier VarDec                      { $$ = new AstNode(TAG_PARAM_DEC, 2, $1, $2); }
    ;

/* Statements */
CompSt: LC DefList StmtList RC                  { $$ = new AstNode(TAG_COMPST, 4, $1, $2, $3, $4); }
    ;
StmtList: Stmt StmtList                         { $$ = new AstNode(TAG_STMT_LIST, 2, $1, $2); }
    | /* empty */                               { $$ = new AstNode(TAG_EMPTY, 0); }
    ;
Stmt: Exp SEMI                                  { $$ = new AstNode(TAG_STMT, 2, $1, $2); }
    | CompSt                                    { $$ = new AstNode(TAG_STMT, 1, $1); }
    | RETURN Exp SEMI                           { $$ = new AstNode(TAG_STMT, 3, $1, $2, $3); }
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE	{ $$ = new AstNode(TAG_STMT, 5, $1, $2, $3, $4, $5); }
    | IF LP Exp RP Stmt ELSE Stmt               { $$ = new AstNode(TAG_STMT, 7, $1, $2, $3, $4, $5, $6, $7); }
    | WHILE LP Exp RP Stmt                      { $$ = new AstNode(TAG_STMT, 5, $1, $2, $3, $4, $5); }
    | error SEMI                                { $$ = new AstNode(TAG_ERROR, 2, $1, $2); syntaxCorrect = false; }
    | WHILE LP error RP Stmt                    { $$ = new AstNode(TAG_ERROR, 5, $1, $2, $3, $4, $5); syntaxCorrect = false; }
    | RETURN error SEMI                         { $$ = new AstNode(TAG_ERROR, 3, $1, $2, $3); syntaxCorrect = false; }

    ;

/* Local Definitions */
DefList: Def DefList                            { $$ = new AstNode(TAG_DEF_LIST, 2, $1, $2); }
    | /* empty */                               { $$ = new AstNode(TAG_EMPTY, 0); }
    ;
Def:  Specifier DecList SEMI                    { $$ = new AstNode(TAG_DEF, 3, $1, $2, $3); }
    | Specifier error SEMI                      { $$ = new AstNode(TAG_ERROR, 3, $1, $2, $3); syntaxCorrect = false; }
    //| Specifier DecList error                   { $$ = new AstNode(TAG_ERROR, 3, $1, $2, $3); syntaxCorrect = false; }
    ;
DecList: Dec                                    { $$ = new AstNode(TAG_DEC_LIST, 1, $1); }
    | Dec COMMA DecList                         { $$ = new AstNode(TAG_DEC_LIST, 3, $1, $2, $3); }
    ;
Dec:  VarDec                                    { $$ = new AstNode(TAG_DEC, 1, $1); }
    | VarDec ASSIGNOP Exp                       { $$ = new AstNode(TAG_DEC, 3, $1, $2, $3); }
    ;

/* Expressions */
Exp:  Exp ASSIGNOP Exp                          { $$ = new AstNode(TAG_EXP, 3, $1, $2, $3); }
    | Exp AND Exp                               { $$ = new AstNode(TAG_EXP, 3, $1, $2, $3); }
    | Exp OR Exp                                { $$ = new AstNode(TAG_EXP, 3, $1, $2, $3); }
    | Exp RELOP Exp                             { $$ = new AstNode(TAG_EXP, 3, $1, $2, $3); }
    | Exp PLUS Exp                              { $$ = new AstNode(TAG_EXP, 3, $1, $2, $3); }
    | Exp MINUS Exp                             { $$ = new AstNode(TAG_EXP, 3, $1, $2, $3); }
    | Exp STAR Exp                              { $$ = new AstNode(TAG_EXP, 3, $1, $2, $3); }
    | Exp DIV Exp                               { $$ = new AstNode(TAG_EXP, 3, $1, $2, $3); }
    | LP Exp RP                                 { $$ = new AstNode(TAG_EXP, 3, $1, $2, $3); }
    | MINUS Exp                                 { $$ = new AstNode(TAG_EXP, 2, $1, $2); }
    | NOT Exp                                   { $$ = new AstNode(TAG_EXP, 2, $1, $2); }
    | ID LP Args RP                             { $$ = new AstNode(TAG_EXP, 4, $1, $2, $3, $4); }
    | ID LP RP                                  { $$ = new AstNode(TAG_EXP, 3, $1, $2, $3); }
    | Exp LB Exp RB                             { $$ = new AstNode(TAG_EXP, 4, $1, $2, $3, $4); }
    | Exp DOT ID                                { $$ = new AstNode(TAG_EXP, 3, $1, $2, $3); }
    | ID                                        { $$ = new AstNode(TAG_EXP, 1, $1); }
    | INT                                       { $$ = new AstNode(TAG_EXP, 1, $1); }
    | FLOAT                                     { $$ = new AstNode(TAG_EXP, 1, $1); }
    | ID LP error RP                            { $$ = new AstNode(TAG_ERROR, 4, $1, $2, $3, $4); syntaxCorrect = false; }
    | Exp LB error RB                           { $$ = new AstNode(TAG_ERROR, 4, $1, $2, $3, $4); syntaxCorrect = false; }
    ;
Args: Exp COMMA Args                            { $$ = new AstNode(TAG_ARGS, 3, $1, $2, $3); }
    | Exp                                       { $$ = new AstNode(TAG_ARGS, 1, $1); }
    ;

%%


#define reportError(type, lineno, format, ...) \
printf("Error type %s at Line %d: " format "\n", type, lineno, ## __VA_ARGS__); \

void yyerror(char *msg) {
  reportError("B", yylineno, "%s", msg);
}
