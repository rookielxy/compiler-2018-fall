%{

#include <stdio.h>
#include "ast.h"
#include "lex.yy.c"

extern int yylex();
extern void yyerror(char *msg);
extern int yylineno;
extern int out;

%}

%union {
    struct ast* type_ast;
    double type_double;
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
%left LP RP LB RB DOT
%left STAR DIV PLUS MINUS RELOP AND OR
%right NOT ASSIGNOP

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

/* High-lever Definitions */
Program: ExtDefList                             { $$ = astRoot = newAst("Program", 1, $1); }
    ;
ExtDefList: ExtDef ExtDefList                   { $$ = newAst("ExtDefList", 2, $1, $2); }
/*    | ExtDef error                              { $$ = newAst("error", 2, $1, $2); $$->error_type = 9; reportError($$->error_type, yylineno); out = 0; }*/
    | /* empty */                               { $$ = newAst("EMPTY", 0); }
    ;
ExtDef: Specifier ExtDecList SEMI               { $$ = newAst("ExtDef", 3, $1, $2, $3); }
    | Specifier SEMI                            { $$ = newAst("ExtDef", 2, $1, $2); }
    | Specifier FunDec CompSt                   { $$ = newAst("ExtDef", 3, $1, $2, $3); }
/*    | Specifier FunDec error                    { $$ = newAst("error", 3, $1, $2, $3); $$->error_type = 5; reportError($$, yylineno); out = 0; }*/
    | Specifier ExtDecList error                { $$ = newAst("error", 3, $1, $2, $3); $$->error_type = 8; /*reportError($$, yylineno);*/ out = 0; }
    | error SEMI                                { $$ = newAst("error", 2, $1, $2); $$->error_type = 10; /*reportError($$, yylineno);*/ out = 0; }
    ;
ExtDecList: VarDec                              { $$ = newAst("ExtDecList", 1, $1); }
    | VarDec COMMA ExtDecList                   { $$ = newAst("ExtDecList", 3, $1, $2, $3); }

/* Specifiers */
Specifier: TYPE                                 { $$ = newAst("Specifier", 1, $1); }
    | StructSpecifier                           { $$ = newAst("Specifier", 1, $1); }
    ;
StructSpecifier: STRUCT OptTag LC DefList RC    { $$ = newAst("StructSpecifier", 5, $1, $2, $3, $4, $5); }
    | STRUCT Tag                                { $$ = newAst("StructSpecifier", 2, $1, $2); }
    ;
OptTag: ID                                      { $$ = newAst("OptTag", 1, $1); }
    | /*empty*/                                 { $$ = newAst("EMPTY", 0); }
    ;
Tag: ID                                         { $$ = newAst("Tag", 1, $1); }
    ;

/* Declarators */
VarDec: ID                                      { $$ = newAst("VarDec", 1, $1); }
    | VarDec LB INT RB                          { $$ = newAst("VarDec", 4, $1, $2, $3, $4); }
    ;
FunDec: ID LP VarList RP                        { $$ = newAst("FunDec", 4, $1, $2, $3, $4); }
    | ID LP RP                                  { $$ = newAst("FunDec", 3, $1, $2, $3); }
    ;
VarList: ParamDec COMMA VarList                 { $$ = newAst("VarList", 3, $1, $2, $3); }
    | ParamDec                                  { $$ = newAst("VarList", 1, $1); }
    ;
ParamDec: Specifier VarDec                      { $$ = newAst("ParamDec", 2, $1, $2); }
    ;

/* Statements */
CompSt: LC DefList StmtList RC                  { $$ = newAst("Comst", 4, $1, $2, $3, $4); }
    | LC DefList StmtList error                 { $$ = newAst("error", 4, $1, $2, $3, $4); $$->error_type = 7; /*reportError($$, yylineno);*/ out = 0; }
    ;
StmtList: Stmt StmtList                         { $$ = newAst("StmtList", 2, $1, $2); }
/*    | Stmt error                                { $$ = newAst("error", 2, $1, $2); $$->error_type = 122; reportError($$, yylineno); out = 0; }*/
    | /* empty */                               { $$ = newAst("EMPTY", 0); }
    ;
Stmt: Exp SEMI                                  { $$ = newAst("Stmt", 2, $1, $2); }
    | CompSt                                    { $$ = newAst("Stmt", 1, $1); }
    | RETURN Exp SEMI                           { $$ = newAst("Stmt", 3, $1, $2, $3); }
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE	{ $$ = newAst("Stmt", 5, $1, $2, $3, $4, $5); }
    | IF LP Exp RP Stmt ELSE Stmt               { $$ = newAst("Stmt", 7, $1, $2, $3, $4, $5, $6, $7); }
    | WHILE LP Exp RP Stmt                      { $$ = newAst("Stmt", 5, $1, $2, $3, $4, $5); }
/*    | Exp error                                 { $$ = newAst("error", 2, $1, $2); $$->error_type = 111; reportError($$, yylineno); out = 0; } */
    | error SEMI                                { $$ = newAst("error", 2, $1, $2); $$->error_type = 1; /*reportError($$, yylineno)*/; out = 0; }
    | WHILE LP error RP Stmt                     { $$ = newAst("error", 5, $1, $2, $3, $4, $5); $$->error_type = 3; /*reportError($$, yylineno);*/ out = 0; }
    | RETURN error SEMI                          { $$ = newAst("error", 3, $1, $2, $3); $$->error_type = 14; /*reportError($$, yylineno);*/ out = 0; }

    ;

/* Local Definitions */
DefList: Def DefList                            { $$ = newAst("DefList", 2, $1, $2); }
    | /* empty */                               { $$ = newAst("EMPTY", 0); }
    ;
Def: Specifier DecList SEMI                     { $$ = newAst("Def", 3, $1, $2, $3); }
    | Specifier error SEMI                      { $$ = newAst("error", 3, $1, $2, $3); $$->error_type = 4; /*reportError($$, yylineno);*/ out = 0; }
    | Specifier DecList error                   { $$ = newAst("error", 3, $1, $2, $3); $$->error_type = 2; /*reportError($$, yylineno);*/ out = 0; }
    | Specifier Dec error                       { $$ = newAst("error", 3, $1, $2, $3); $$->error_type = 11; /*reportError($$, yylineno);*/ out = 0; }
    ;
DecList: Dec                                    { $$ = newAst("DecList", 1, $1); }
    | Dec COMMA DecList                         { $$ = newAst("DecList", 3, $1, $2, $3); }
    ;
Dec:  VarDec                                    { $$ = newAst("Dec", 1, $1); }
    | VarDec ASSIGNOP Exp                       { $$ = newAst("Dec", 3, $1, $2, $3); }
    ;

/* Expressions */
Exp:  Exp ASSIGNOP Exp                          { $$ = newAst("Exp", 3, $1, $2, $3); }
    | Exp AND Exp                               { $$ = newAst("Exp", 3, $1, $2, $3); }
    | Exp OR Exp                                { $$ = newAst("Exp", 3, $1, $2, $3); }
    | Exp RELOP Exp                             { $$ = newAst("Exp", 3, $1, $2, $3); }
    | Exp PLUS Exp                              { $$ = newAst("Exp", 3, $1, $2, $3); }
    | Exp MINUS Exp                             { $$ = newAst("Exp", 3, $1, $2, $3); }
    | Exp STAR Exp                              { $$ = newAst("Exp", 3, $1, $2, $3); }
    | Exp DIV Exp                               { $$ = newAst("Exp", 3, $1, $2, $3); }
    | LP Exp RP                                 { $$ = newAst("Exp", 3, $1, $2, $3); }
    | MINUS Exp                                 { $$ = newAst("Exp", 2, $1, $2); }
    | NOT Exp                                   { $$ = newAst("Exp", 2, $1, $2); }
    | ID LP Args RP                             { $$ = newAst("Exp", 4, $1, $2, $3, $4); }
    | ID LP RP                                  { $$ = newAst("Exp", 3, $1, $2, $3); }
    | Exp LB Exp RB                             { $$ = newAst("Exp", 4, $1, $2, $3, $4); }
    | Exp DOT ID                                { $$ = newAst("Exp", 3, $1, $2, $3); }
    | ID                                        { $$ = newAst("Exp", 1, $1); }
    | INT                                       { $$ = newAst("Exp", 1, $1); }
    | FLOAT                                     { $$ = newAst("Exp", 1, $1); }
    | ID LP error RP                            { $$ = newAst("error", 4, $1, $2, $3, $4); $$->error_type = 10; /*reportError($$, yylineno);*/ out = 0; }
    | Exp LB error RB                           { $$ = newAst("error", 4, $1, $2, $3, $4); $$->error_type = 6; /*reportError($$, yylineno);*/ out = 0; }
    ;
Args: Exp COMMA Args                            { $$ = newAst("Args", 3, $1, $2, $3); }
    | Exp                                       { $$ = newAst("Args", 1, $1); }
    ;

%%


#define reportError(type, lineno, format, ...) \
printf("Error type %s at Line %d: " format "\n", type, lineno, ## __VA_ARGS__); \

void yyerror(char *msg) {
  reportError("B", yylineno, "%s", msg);
}
