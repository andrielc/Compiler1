#include "tree.h"

typedef enum{
	T_PREDEF=1,
	T_ARRAY,
	T_FUNCTION
} TypeConstr;

typedef enum {
	P_VALUE = 1,
	P_VARIABLE,
	P_FUNCTION
} Passage;

typedef enum{
	S_CONST=1,
	S_VARIABLE,
	S_PARAMETER,
	S_FUNCTION,
	S_LABEL,
	S_TYPE
} SymbCateg;

typedef struct _typeDescr{
	TypeConstr constr;
	struct _typeDescr *type;
	int size;
	union{
		struct _array {
			int dimen;
		}array;
		struct _func {
			struct _symbEntry *params;
		}func;
	} descr;
} TypeDescr, *TypeDescrPtr;

typedef struct _symbEntry{
	SymbCateg categ;
	char *ident;
	int k;
	struct _symbEntry *next;
	TypeDescrPtr type;
	union{
		struct _constant {
			int value;
		} constant;
		struct _variable {
			int displ;
		}variable;
		struct _formParameter {
			int displ;
			Passage pass;
		}formParameter;
		struct _function {
			int displ;
			struct _symbEntry *params;
		}function;
		struct _label {
			char *label;
			int defined;
		}label;
	} descr;
} SymbEntry, *SymbEntryPtr;

TypeDescrPtr type(TypeConstr constr, TypeDescrPtr type, int size, int dim, SymbEntryPtr pt);
SymbEntryPtr symbol(SymbCateg categ, char *ident, int k, TypeDescrPtr type, int add, ... );
void genCode0(char *instr);
void genCode1(char *instr,int param);
void genCode2(char *instr,int param1,int param2);
void genCodeJump(char *instr, int toLabel, int param1, int param2 );
void genCodeLabel(char *instr, int label );
void genCodeLabel1(char *instr, int label, int param );
void processProgram(void *tree);
SymbEntryPtr findSymbol(char *c, int type);
