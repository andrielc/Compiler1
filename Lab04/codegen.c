#include "codegen.h"
#include "slc.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


int label=1,k=-1; //contar label
SymbEntryPtr symbTable = NULL;
TypeDescrPtr Bool, Int;

int newLabel () {
	return label++;
}

TypeDescrPtr type(TypeConstr constr, TypeDescrPtr type, int size, int dim, SymbEntryPtr pt) {
	TypeDescrPtr td = malloc(sizeof(TypeDescr));
	td->constr = constr;
	td->type = type;
	td->size = size;
	
	if (constr != T_PREDEF){
		if (constr == T_ARRAY) {
			td->descr.array.dimen = dim;
		}else{
			td->descr.func.params = pt;
		}

	}
	return td;
}

SymbEntryPtr symbol(SymbCateg categ, char *ident, int k, TypeDescrPtr type, int add, ... ){
	SymbEntryPtr se = malloc(sizeof(SymbEntry));
	se->categ = categ;
	se->ident = ident;
	se->k = k;
	se->type = type;
	
	if (add != 0) { //TODO: verificar
		se->next = symbTable;
		symbTable = se;
	}
	
	if (categ == S_TYPE) {
		return se;
	}
	va_list va;
	va_start (va,add);
	switch (categ) {
		case S_CONST:
			se->descr.constant.value = va_arg(va,int);
			break;
		case S_VARIABLE:
			se->descr.variable.displ = va_arg(va,int);
			break;
		case S_PARAMETER:
			se->descr.formParameter.displ = va_arg(va,int);
			se->descr.formParameter.pass = va_arg(va,Passage);
			break;
		case S_FUNCTION:
			se->descr.function.displ = va_arg(va,int);
			se->descr.function.params = va_arg(va,SymbEntryPtr);
			break;
		case S_LABEL:
			se->descr.label.label = va_arg(va,char *);
			se->descr.label.defined = va_arg(va,int);
			break;
		default:;
	}

	return se;
}

void processProgram(void *prog){
	//slide 196 "palavras reservadas"
	Bool = type (T_PREDEF,NULL,1,0,NULL);
	Int = type (T_PREDEF,NULL,1,0,NULL);

	symbol (S_TYPE,"integer",-1,Int,1);
	symbol (S_TYPE,"boolean",-1,Int,1);
	symbol (S_CONST,"true",-1,Bool,1);
	symbol (S_CONST,"false",-1,Bool,1);
	symbol (S_FUNCTION,"read",-1,NULL,1,-1,1);
	symbol (S_FUNCTION,"write",-1,NULL,1,-1,1);

	genCode0("MAIN");
	processFuncDecl(p,1);
	genCode0("END");
	
}

void genCode0(char *instr) {
	printf("\t%s\n",instr);
}

void genCode1(char *instr,int param) {
	printf("\t%s\t%d\n",instr,param);
}

void genCode2(char *instr, int param1, int param2){
	printf("\t%s\t%d, %d\n",instr,param1,param2);
}

void genCodeJump(char *instr, int toLabel, int param1, int param2 ) {
	printf("\t%s\tL",instr,toLabel);
	if (strcmp(instr,"JMPL")) {
		printf(", %d, %d");
	}
	printf("\n");
}

void genCodeLabel(char *instr, int label ) {
	printf("L%d:\t%s\n",label,instr);
}

void genCodeLabel1(char *instr, int label, int param ) {
	//entf
	printf("L%d:\t%s\t%d\n", label, instr, param);
}

SymbEntryPtr processFParams(TreeNodePtr np,int *lastDispl){
	TreeNodePtr tnp=NULL;
	SymbEntry sep;
	SymbEntryPtr next;
	TypeDescrPtr type;
	
	next = processFParams(np->next,lastDispl);

	if (np == NULL){
		return NULL;
	}
	SymbEntryPtr aux = getSymbol(np->comps[1]->str,1);
	if(np->categ == C_VARIABLE){
		type = aux->type;
		tn = np->comps[0];
		while(tnp != NULL){
			(*lastDispl) -= type->size;
			//add simbolo
			symbol(S_PARAMETER,tnp->str,k,aux->type,1,*lastDispl,P_VALUE);
			sep = symbol(S_PARAMETER,tnp->str,k,aux->type,0,*lastDispl,P_VALUE);
			sep->next = next;
			next = sep;
			tnp = tnp->next;
		}
	}else if (np->categ == C_POINTER) {
		tn = np->comps[0];
		while(tnp != NULL){
			(*lastDispl)--;
			symbol(S_PARAMETER,tnp->str,k,aux->type,1,*lastDispl,P_VARIABLE);
			sep = symbol(S_PARAMETER,tnp->str,k,aux->type,0,*lastDispl,P_VARIABLE);
			sep ->next=next;
			next= sep;
			tnp = tnp->next;
		}
	}else if (np->categ == C_FUNCTION) {
		//TODO: função como parametro
	}
	return sep;
}

SymbEntryPtr getSymbol(char *c, int type){
	SymbEntryPtr sym = symbTable;
	while(sym != NULL){
		if(!strcmp(c,sym->ident) && (!type || sym->categ==S_TYPE))
			break;
		sym=sym->next;
	}
	if(sym == NULL)
		SemanticError(NULL);
	return sym;
}


void processLabels(TreeNodePtr np){
	while (np != NULL) {
		symbol(S_LABEL, np->str, k, NULL, 1, newLabel(), 0);
		np = np->next;
	}
	return;
}

void processFuncDeclaration (TreeNodePtr p, int ismain) {	
	char *name = p->comps[1]->str;
	TypeDescrPtr restType = NULL, funType;
	int lastDispl = -4, entLabel,retLabel ;
	SymbolEntryPtr formals, func;
	
	if (!ismain) {
		
	}
}
