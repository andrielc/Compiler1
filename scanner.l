/*
Scanner in FLEX for the Simple Language (SL)
*/

%{
	#include <parser.h>
	int num_lines = 0;
	extern char *yytext;
	DIG	[0-9];
%}

%%

%{
	/*Block comment scanner*/
%}
"/*"	{
			register int c;
			for ( ; ; )
			{
				while ( (c = input()) != '*' && c != EOF);
				if (c == '*') {
					while ( (c=input()) == '*');
					if (c == '/')
						break; /*found the end*/
				}
				if ( c == EOF ) {
					error("EOF in comment");
					return (UNFINISHED_COMMENT);
					break;
				}else{
					return (UNFINISHED_COMMENT);
				}
			}
		}

%{
	/*Line comment*/
%}
"//"	{
			register int c;
			for ( ; ; )
			{
				while ( (c = input() ) != '\n' && c != EOF );
				if (c == '\n') {
					break;
				}
				if (c == EOF) {
					error ("EOF in comment");
					return (UNFINISHED_COMMENT);
					break;
				}else{
					return (UNFINISHED_COMMENT);
				}
			}
		}
%{
/* Symbols */
%}
"+"	return (PLUS);
"*"	return (MULTIPLY)
"-"	return (MINUS);
"/"	return (DIV);

"("	return (OPEN_PAREN);
")"	return (CLOSE_PAREN);
"{"	return (OPEN_BRACE);
"}"	return (CLOSE_BRACE);
"["	return (OPEN_BRACKET);
"]"	return (CLOSE_BRACKET);

"&&"	return (AND);
"||"	return (OR);
"=="	return (EQUAL);
"!="	return (DIFFERENT);
'<'	return (LESS);
'>'	return (GREATER);
"<="	return (LESS_OR_EQUAL);
">="	return (GREATER_OR_EQUAL);

';'	return (SEMI_COLON);
':' return (COLON);
','	return (COMMA);

%{
/*Identiers and Integers*/
%}

DIG+	return (INTEGER);

%{
/* Reserved words */
%}

goto	return (GOTO);
void	return (VOID);
functions return (FUNCTIONS);
vars	return (VARS);
var	return(VAR);
while	return (WHILE);
if	return (IF);
else return (ELSE);
labels return (LABELS);
return	return (RETURN);
types return (TYPES);

%%

int yylex () {
	return 1;
}
