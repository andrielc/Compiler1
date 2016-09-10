/*
Scanner in FLEX for some rules of the Simple Language (SL)
*/

%{
	#include "parser.h"
	int line_num = 1;
%}

%x IN_COMMENT
%x quote

%%
[ \t]	;
"\n"	line_num++;

<quote><<EOF>>	return (UNFINISHED_COMMENT);

%{
	/* Reserved words */
%}
"goto"	return (GOTO);
"void"	return (VOID);
"functions" return (FUNCTIONS);
"vars"	return (VARS);
"var"	return(VAR);
"while"	return (WHILE);
"if"	return (IF);
"else" return (ELSE);
"labels" return (LABELS);
"return"	return (RETURN);
"types" return (TYPES);

%{
	/*Line comment*/
%}
"//".*	;

%{
	/*Block comment*/
%}
<INITIAL>{
     "/*"              BEGIN(IN_COMMENT);
     }
     <IN_COMMENT>{
     "*/"      BEGIN(INITIAL);
     [^*\n]+   // eat comment in chunks
     "*"       // eat the lone star
     <<EOF>>	BEGIN(quote);
     "\n"	line_num++;
     }


<<EOF>>	return (END_OF_FILE);
%{
/* Symbols */
%}
"+"	{return PLUS;}
"*"	{return MULTIPLY;}
"-"	{return MINUS;}
"/"	{return DIV;}

"("	{return OPEN_PAREN;}
")"	{return	CLOSE_PAREN;}
"["	{return OPEN_BRACKET;}
"]"	{return CLOSE_BRACKET;}
"{"	{return OPEN_BRACE;}
"}"	{return CLOSE_BRACE;}

"&&"	{return AND;}
"||"	{return OR;}
"=="	{return EQUAL;}
"!="	{return DIFFERENT;}
"<"	{return LESS;}
">"	{return GREATER;}
"<="	{return LESS_OR_EQUAL;}
">="	{return GREATER_OR_EQUAL;}
"="	{return ASSIGN;}

";"	{return SEMI_COLON;}
":"	{return COLON;}
","	{return COMMA;}

%{
	/*Identifiers and Integer*/
%}
[0-9]+	return(INTEGER);
[A-Za-z][A-Za-z0-9]*	return(IDENTIFIER);

%{
	/* Error repport */
%}
.	{return(LEXICAL_ERROR);}

%%

int yywrap(){
	return 1;
}
