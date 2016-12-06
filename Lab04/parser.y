/*
Scanner in Yacc for grammar production in Simple Language (SL)
*/

%{
#include "tree.h"
#include<stdlib.h>
#include<stdio.h>

TreeNodePtr tree;
int stackheight;
char *empty="empty";

int yylex();
int yyerror(char *s);
%}

%union{
	char			*string;
	struct _treeNode	*node;
}

%left		PLUS MINUS
%left		UNARY
%left		MULTIPLY DIV

%token		EQUAL
%token		DIFFERENT
%token		LESS
%token		LESS_OR_EQUAL
%token		GREATER
%token		GREATER_OR_EQUAL
%token		PLUS
%token		MINUS
%token		OR
%token		MULTIPLY
%token		DIV
%token		AND
%token		UNARY
%token		CLOSE_BRACE
%token		CLOSE_BRACKET
%token		CLOSE_PAREN
%token		COLON
%token		COMMA
%token		ELSE
%token		END_OF_FILE
%token		FUNCTIONS
%token		GOTO
%token	<string>	IDENTIFIER
%token		ASSIGN
%token		IF
%token	<string>	INTEGER
%token		LABELS
%token		NOT
%token		OPEN_BRACE
%token		OPEN_BRACKET
%token		OPEN_PAREN
%token		RETURN
%token		SEMI_COLON
%token		TYPES
%token		VAR
%token		VARS
%token	<string>	VOID
%token		WHILE
%token		UNFINISHED_COMMENT
%token		LEXICAL_ERROR

%type	<node>	function block labels types types_list variables variables_list functions functions_list body body_list type type_list formal_parameters formal_parameters_list formal_parameter expression_parameter function_parameter statement variable variable_list unlabeled_statement assignment function_call_statement goto return compound compound_list conditional repetitive expression relational_operator simple_expression simple_expression_list additive_operator term term_list multiplicative_operator factor function_call identifier_list expression_list identifier integer return_function end_of_file
%%

program : 
		function {stackheight = yylen; tree = $1; /*dumpTree(tree,0,"\t")*/;} end_of_file
	    ;

end_of_file :
		END_OF_FILE {return 0;}
		|
		{}
		;

function :
		return_function identifier formal_parameters block {$$ = genNode6(C_FUNCTION,NULL,$1,$2,$3,$4);}
		;

block :
		labels types variables functions body {$$ = genNode7(C_BLOCK,NULL,$1,$2,$3,$4,$5);}
		;

labels : 
		LABELS identifier_list SEMI_COLON { $$ = $2; }
		|
		{$$ = genNode2(C_LABEL,empty);}
		;

types :
		TYPES types_list { $$ = inversao($2); }
		|
		{$$ = genNode2(C_TYPE,empty);}
		;

types_list :
		identifier ASSIGN type SEMI_COLON {$$ = genNode4(C_TYPE,NULL,$1,$3);}
		|
		types_list identifier ASSIGN type SEMI_COLON {$$ = genNode4(C_TYPE,NULL,$2,$4); $$->next = $1;}
		;
		
variables :
		VARS variables_list { $$ = inversao($2); }
		|
		{$$ = genNode2(C_VARIABLE,empty);}
		;

variables_list :
		identifier_list COLON type SEMI_COLON  {$$ = genNode4(C_VARIABLE,NULL,$1,$3);}
		|
		variables_list identifier_list COLON type SEMI_COLON {$$ = genNode4(C_VARIABLE,NULL,$2,$4); $$->next = $1;}
		;

functions :
		FUNCTIONS functions_list  {$$ = inversao($2);}
		|
		{}
		;

functions_list : 
		function
		|
		functions_list function {$2->next = $1; $$ = inversao($2);}
		;

body : 
		OPEN_BRACE body_list CLOSE_BRACE {$$ = inversao($2);}
		;

body_list :
		body_list statement {$2->next = $1; $$ = $2;}
		|
		{$$ = genNode2(C_STATEMENT,empty);}
		;

type : 
		identifier type_list {$$ = genNode4(C_TYPE,NULL,$1,inversao($2));}
		;

type_list :
		type_list OPEN_BRACKET integer CLOSE_BRACKET {$$ = $3; $3->next = $1;}
		|
		{$$ = genNode2(C_INTEGER,NULL);}
		;

formal_parameters :
		OPEN_PAREN formal_parameters_list CLOSE_PAREN {$$ = inversao($2);}
		;

formal_parameters_list :
		formal_parameter 
		|
		formal_parameters_list SEMI_COLON formal_parameter {$$ = $3; $3->next = $1;}
		|
		{$$=genNode2(C_TYPE,empty);}
		;

formal_parameter : 
		expression_parameter
		|
		function_parameter
		;

expression_parameter :
		VAR identifier_list COLON identifier {$$ = genNode4(C_POINTER,NULL,inversao($2),$4);}
		|
		identifier_list COLON identifier {$$ = genNode4(C_VARIABLE,NULL,inversao($1),$3);}
		;

function_parameter : 
		return_function identifier formal_parameters { $$ = genNode5(C_VARIABLE,NULL,$1,$2,$3);}
		;

statement : 
		identifier COLON unlabeled_statement {$$=genNode4(C_LABEL,NULL,$1,$3);}
		|
		unlabeled_statement
		|
		compound
		;

variable : 
		identifier variable_list {$$ = genNode4(C_VARIABLE,NULL,$1,inversao($2));}
		;

variable_list :
		variable_list OPEN_BRACKET expression CLOSE_BRACKET {$$ = $3; $3->next = $1;}
		|
		{$$ = genNode2(C_INTEGER,NULL);}
		;

unlabeled_statement :
		assignment
		|
		function_call_statement
		|
		goto
		|
		return
		|
		conditional
		|
		repetitive
		| 
		empty_statement {$$ = genNode2(C_STATEMENT,empty);}
		;

assignment :
		variable ASSIGN expression SEMI_COLON {$$ = genNode4(C_ASSIGN,NULL,$1,$3);}
		;

function_call_statement : 
		function_call SEMI_COLON
		;

goto :
		GOTO identifier SEMI_COLON {$$ = genNode3(C_GOTO,NULL,$2);}
		;

return :
		RETURN SEMI_COLON {$$ = genNode2(C_RETURN,empty);}
		|
		RETURN expression SEMI_COLON {$$ = genNode3(C_RETURN,NULL,$2);}
		;

compound : 
		OPEN_BRACE compound_list CLOSE_BRACE {$$ = inversao($2);}
		;

compound_list : 
		unlabeled_statement
		|
		compound_list unlabeled_statement {$$ = $2; $2->next = $1;}
		;

conditional :
		IF OPEN_PAREN expression CLOSE_PAREN compound { $$ = genNode4(C_IF,NULL,$3,$5);}
		|
		IF OPEN_PAREN expression CLOSE_PAREN compound ELSE compound  {$$ = genNode5(C_IF,NULL,$3,$5,$7);}
		;

repetitive : 
		WHILE OPEN_PAREN expression CLOSE_PAREN compound { $$ = genNode4(C_WHILE,NULL,$3,$5); }
		;

empty_statement :
		SEMI_COLON
		;

expression : 
		simple_expression
		|
		simple_expression relational_operator simple_expression {$$ = genNode5(C_BIN_EXPR,NULL,$1,$2,$3);}
		;

relational_operator :
		EQUAL {$$ = genNode2(C_OPERATOR,"==");}
		|
		DIFFERENT {$$ = genNode2(C_OPERATOR,"!=");}
		|
		LESS {$$ = genNode2(C_OPERATOR,"<");}
		|
		LESS_OR_EQUAL {$$ = genNode2(C_OPERATOR,"<=");}
		|
		GREATER {$$ = genNode2(C_OPERATOR,">");}
		|
		GREATER_OR_EQUAL {$$ = genNode2(C_OPERATOR,">=");}
		;

simple_expression :
		term simple_expression_list{$$ = genNode4(C_EXPR,NULL,$1,inversao($2));}
		|
		PLUS term simple_expression_list {$$ = genNode4(C_EXPR,NULL,$2,inversao($3));}
		|
		MINUS term simple_expression_list %prec UNARY {$$ = genNode4(C_NEG,NULL,$2,inversao($3));}
		;

simple_expression_list : 
		simple_expression_list additive_operator term {$$ = genNode4(C_EXPR,NULL,$2,$3); $$->next = $1;}
		|
		{$$ = genNode2(C_EXPR,empty);}
		;

additive_operator : 
		PLUS {$$ = genNode2(C_OPERATOR,"+");}
		|
		MINUS {$$ = genNode2(C_OPERATOR,"-");}
		|
		OR {$$ = genNode2(C_OPERATOR,"||");}
		;

term :
		factor term_list {$$ = genNode4(C_EXPR,NULL,$1,inversao($2)); }
		;

term_list : 
		term_list multiplicative_operator factor {$$ = genNode4(C_EXPR,NULL,$2,$3); $$->next = $1;}
		|
		{$$ = genNode2(C_EXPR,empty);}
		;

multiplicative_operator : 
		MULTIPLY {$$ = genNode2(C_OPERATOR,"*");}
		|
		DIV {$$ = genNode2(C_OPERATOR,"/");}
		|
		AND {$$ = genNode2(C_OPERATOR,"&&");}
		;

factor :
		variable
		|
		integer
		|
		function_call
		|
		OPEN_PAREN expression CLOSE_PAREN {$$ = $2;}
		|
		NOT factor {$$ = genNode3(C_NOT,NULL,$2);}
		;

function_call : 
		identifier OPEN_PAREN expression_list CLOSE_PAREN { $$ = genNode4(C_FUNCTION_CALL,NULL,$1,inversao($3));}
		;

identifier_list :
		identifier
		|
		identifier_list COMMA identifier {$3->next = $1; $$ = $3;}
		;

expression_list :
		expression 
		|
		expression_list COMMA expression {$$ = $3; $3->next = $1;}
		|
		{$$ = genNode2(C_EXPR,empty);}
		;

identifier : 
		IDENTIFIER {$$ = genNode2(C_IDENT,$1);}
		;

integer :
		INTEGER { $$ = genNode2(C_INTEGER,$1);}
		;

return_function :
		VOID {$$ = genNode2(C_TYPE,NULL);}
		|
		identifier
		;
%%
