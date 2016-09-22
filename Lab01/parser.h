/* Avoids warnings */
int yylex();
int fileno(FILE *);

/* These definitions might have been specified as %token for Bison */


#define EQUAL 258
#define DIFFERENT 259
#define LESS 260
#define LESS_OR_EQUAL 261
#define GREATER 262
#define GREATER_OR_EQUAL 263
#define PLUS 264
#define MINUS 265
#define OR 266
#define MULTIPLY 267
#define DIV 268
#define AND 269
#define UNARY 270
#define CLOSE_BRACE 271
#define CLOSE_BRACKET 272
#define CLOSE_PAREN 273
#define COLON 274
#define COMMA 275
#define ELSE 276
#define END_OF_FILE 277
#define FUNCTIONS 278
#define GOTO 279
#define IDENTIFIER 280
#define ASSIGN 297
#define IF 281
#define INTEGER 282
#define LABELS 283
#define NOT 284
#define OPEN_BRACE 285
#define OPEN_BRACKET 286
#define OPEN_PAREN 287
#define RETURN 288
#define SEMI_COLON 289
#define TYPES 290
#define VAR 291
#define VARS 292
#define VOID 293
#define WHILE 294
#define UNFINISHED_COMMENT 295
#define LEXICAL_ERROR 296
