%{
int yylex();
int yyerror(char *s);
%}

%union{
	int	number;
	char *string;
}

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
%token		CLOSE_BRACE
%token		CLOSE_BRACKET
%token		CLOSE_PAREN
%token		COLON
%token		COMMA
%token		ELSE
%token		END_OF_FILE
%token		FUNCTIONS
%token		GOTO
%token	<str>	IDENTIFIER
%token		ASSIGN
%token		IF
%token	<num>	INTEGER
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
%token		VOID
%token		WHILE
%token		UNFINISHED_COMMENT
%token		LEXICAL_ERROR

%%

program : 
		function END_OF_FILE { return 0; }
	    ;


function :
		IDENTIFIER IDENTIFIER formal_parameters block
		|
		VOID IDENTIFIER formal_parameters block
		;

block :
		labels types variables functions body
		;

labels :
		|
		LABELS identifier_list SEMI_COLON
		;

types :
		|
		TYPES types_list
		;

types_list :
		IDENTIFIER ASSIGN type SEMI_COLON
		|
		types_list IDENTIFIER ASSIGN type SEMI_COLON
		;
		
variables :
		|
		VARS variables_list
		;

variables_list :
		identifier_list COLON type SEMI_COLON
		|
		variables_list identifier_list COLON type SEMI_COLON
		;

functions :
		|
		FUNCTIONS functions_list
		;

functions_list : 
		function
		|
		functions_list function
		;

body : 
		OPEN_BRACE body_list CLOSE_BRACE
		;

body_list :
		|
		statement
		|
		body_list statement
		;

type : 
		IDENTIFIER type_list
		;

type_list :
		|
		OPEN_BRACKET INTEGER CLOSE_BRACKET
		|
		type_list OPEN_BRACKET INTEGER CLOSE_BRACKET
		;

formal_parameters :
		OPEN_PAREN formal_parameters_list CLOSE_PAREN
		;

formal_parameters_list :
		|
		formal_parameter
		|
		formal_parameters_list SEMI_COLON formal_parameter
		;

formal_parameter : 
		expression_parameter
		|
		function_parameter
		;

expression_parameter :
		VAR identifier_list COLON IDENTIFIER
		|
		identifier_list COLON IDENTIFIER
		;

function_parameter : 
		IDENTIFIER IDENTIFIER formal_parameters
		|
		VOID IDENTIFIER formal_parameters
		;

statement : 
		IDENTIFIER COLON unlabeled_statement
		|
		unlabeled_statement
		|
		compound
		;

variable : 
		IDENTIFIER variable_list
		;

variable_list :
		|
		OPEN_BRACKET expression CLOSE_BRACKET
		|
		variable_list OPEN_BRACKET expression CLOSE_BRACKET
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
		empty_statement
		;

assignment :
		variable ASSIGN expression SEMI_COLON
		;

function_call_statement : 
		function_call SEMI_COLON
		;

goto :
		GOTO IDENTIFIER SEMI_COLON
		;

return :
		RETURN SEMI_COLON
		|
		RETURN expression SEMI_COLON
		;

compound : 
		OPEN_BRACE compound_list CLOSE_BRACE
		;

compound_list : 
		unlabeled_statement
		|
		compound_list unlabeled_statement
		;

conditional :
		IF OPEN_PAREN expression CLOSE_PAREN compound
		|
		IF OPEN_PAREN expression CLOSE_PAREN compound ELSE compound
		;

repetitive : 
		WHILE OPEN_PAREN expression CLOSE_PAREN compound
		;

empty_statement :
		SEMI_COLON
		;

expression : 
		simple_expression
		|
		simple_expression relational_operator simple_expression
		;

relational_operator :
		EQUAL
		|
		DIFFERENT
		|
		LESS
		|
		LESS_OR_EQUAL
		|
		GREATER
		|
		GREATER_OR_EQUAL
		;

simple_expression :
		term simple_expression_list
		|
		PLUS term simple_expression_list
		|
		MINUS term simple_expression_list
		;

simple_expression_list : 
		|
		additive_operator term
		|
		simple_expression_list additive_operator term
		;

additive_operator : 
		PLUS
		|
		MINUS
		|
		OR
		;

term :
		factor term_list
		;

term_list : 
		|
		multiplicative_operator factor
		|
		term_list multiplicative_operator factor
		;

multiplicative_operator : 
		MULTIPLY
		|
		DIV
		|
		AND
		;

factor :
		variable
		|
		INTEGER
		|
		function_call
		|
		OPEN_PAREN expression CLOSE_PAREN
		|
		NOT factor
		;

function_call : 
		IDENTIFIER OPEN_PAREN expression_list CLOSE_PAREN
		;

identifier_list :
		IDENTIFIER
		|
		identifier_list COMMA IDENTIFIER
		;

expression_list :
		|
		expression
		|
		expression_list COMMA expression
		;
%%
