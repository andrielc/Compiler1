/*
Scanner in FLEX for the Simple Language (SL)
*/

%{
	/*
	#include <parser.h>
	int num_lines = 0;

}%

%%

%{
	/*Lines counter*/
}%
\n	++num_lines;

<<EOF>>	

%{
	/*Block comment scanner*/
}%
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
					break;
				}
			}
			/*Do something*/
		}

%{
	/*Line comment*/
}%
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
					break;
				}
			}
		}
%{
/* Symbols */
%}
"+"	
"-"	
"*"	
"/"	
"("	
")"	
"&&"	
"||"	
"=="
"!="
"<"
">"
"<="
">="
";"
":"

%{
/* Identifiers and integers */
%}
[a-z]
%%

main( argc, argv) {


}
