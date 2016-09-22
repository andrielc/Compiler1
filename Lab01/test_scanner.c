/* 
   Test of the scanner for SL -- Simple Language.
   Prints one code and token per line.
   
   Exits on any lexical error or unfinished comment.
   
   Returns code execution 0 even in case of errors so as not to
   confuse SuSy.
*/

/* Last update: "test_scanner.c: 2015-08-19 (Wed)  09:39:51 BRT (tk)" */

#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

extern int yylex();

char *yytext;

extern int line_num;

int main() {

  char format[] = "%03d  %s\n";
  int code;
  
  do {
    code = yylex();
    
    switch (code) {

        case END_OF_FILE:        printf("%d lines read\n",line_num-1); break;
        case LEXICAL_ERROR:      printf("Lexical error in line %d\n",line_num); exit(0);
        case UNFINISHED_COMMENT: printf("Unfinished comment\n"); exit(0);
    
        default:                 printf(format,code,yytext); break;
        
    }
    
  }  while (code!=END_OF_FILE);
    
  return 0;
      
}