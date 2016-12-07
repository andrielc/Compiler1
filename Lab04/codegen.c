#include "codegen.h"
#include "slc.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


int label=1,k=-1; //contar label
SymbEntryPtr symbTable = NULL;
TypeDescrPtr predefBool, predefInt;

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
	
	if (add != 0) {
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
	predefBool = type (T_PREDEF,NULL,1,0,NULL);
	predefInt = type (T_PREDEF,NULL,1,0,NULL);

	symbol (S_TYPE,"integer",-1,predefInt,1);
	symbol (S_TYPE,"boolean",-1,predefInt,1);
	symbol (S_CONST,"true",-1,predefBool,1);
	symbol (S_CONST,"false",-1,predefBool,1);
	symbol (S_FUNCTION,"read",-1,NULL,1,-1,1);
	symbol (S_FUNCTION,"write",-1,NULL,1,-1,1);

	genCode0("MAIN");
	//processFuncDecl(prog,1);
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


SymbEntryPtr findSymbol(char *c, int type){
	SymbEntryPtr sym = symbTable;
	while(sym != NULL){
		if(!strcmp(c,sym->ident) && (!type || sym->categ == S_TYPE))
			break;
		sym=sym->next;
	}
	if(sym == NULL)
		SemanticError(NULL);
	return sym;
}

void processTypes (TreeNodePtr tnp) {
	SymbEntryPtr aux = NULL;
	TypeDescrPtr t = NULL;
	while (tnp != NULL) {
		aux = findSymbol (tnp->comps[1]->comps[0]->str,1); //identifier
		t = aux->type;
		if (tnp->comps[1]->comps[1] != NULL) { //type -> array
			TreeNodePtr np = tnp->comps[1]->comps[1];
			int mult = 1, dim = 0;
			while (np != NULL) {
				dim++;
				mult *= atoi(np->str);
				np = np->next;
			}
			t = type(T_ARRAY,t,mult*(t->size),dim,NULL);
		}
		symbol(S_TYPE, tnp->comps[0]->str,k,t,1); //identifier
		tnp = tnp->next;
	}

}


void processLabels(TreeNodePtr np){
	while (np != NULL) {
		symbol(S_LABEL, np->str, k, NULL, 1, newLabel(), 0);
		np = np->next;
	}
	return;
}

SymbEntryPtr processFParams(TreeNodePtr np,int *lastDispl){
	TreeNodePtr tnp=NULL;
	SymbEntryPtr sep, next;
	next = processFParams(np->next,lastDispl);

	if (np == NULL){
		return NULL;
	}
	
	if(np->categ == C_VARIABLE){
		SymbEntryPtr aux = findSymbol(np->comps[1]->str,1);
		tnp = np->comps[0];
		while(tnp != NULL){
			(*lastDispl) -= aux->type->size; //qtd de argumentos reservados pra pilha
			//add simbolo
			symbol(S_PARAMETER,tnp->str,k,aux->type,1,*lastDispl,P_VALUE);
			sep = symbol(S_PARAMETER,tnp->str,k,aux->type,0,*lastDispl,P_VALUE);
			sep->next = next;
			next = sep;
			tnp = tnp->next;
		}
	}else if (np->categ == C_POINTER) {
		SymbEntryPtr aux = findSymbol(np->comps[1]->str,1);
		tnp = np->comps[0];
		while(tnp != NULL){
			(*lastDispl)--; //pointer.. precisa apenas do LADR ou LGAD
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

void processIteration(TreeNodePtr p) {
	int label1 = newLabel();
	int label2 = newLabel();
	TypeDescrPtr t;
	genCodeLabel(label1);
	t = processExpr(p->comps[0]);
	if (t!=predefBool)
	    SemanticError();
	genCodeJump("JMPF",label2);
	processStatement(p->comps[1]);
	genCodeJump("JUMP",label1);
	genCodeLabel(label2);
}

void processAssign(TreeNodePtr p) { //verificar
	TreeNodePtr pvar  = p->comps[0];
	TreeNodePtr pexpr = p->comps[1];
	char *id = pvar.str;
	SymbEntryPtr ste = searchSte(id);
	TypeDescrPtr tvar,texpr;
	if (ste==NULL) SemanticError(); // undefined variable
	if ((ste->categ!=S_VARIABLE)&&(ste->categ!=S_PARAMETER))
	   SemanticError();
	tvar = ste->descr->type;
	texpr = processExpr(pexpr);
	if (!compatibleTypes(tvar,texpr))
	    SemanticError();
	if ( (ste->categ == S_PARAMETER) && (ste->descr->pass == P_VARIABLE) ){
	    genCode2("STVI",ste->level,ste->descr->displ);
	}else{
	    genCode2("STVL",ste->level,ste->descr->displ);
	}
}

void processVariables(TreeNodePtr p) {
    for ( ; (p!=NULL); p=p->next )
        processVarDecl(p);
}

void processVarDecl(TreeNodePtr p) {
    TreeNodePtr pvars = p->comps[0];
    TypeDescrPtr ptype = p->comps[1]->type;
    SymbEntryPtr ste;
    char *id;
    for ( ; (pvars!=NULL); pvars=pvars->next ) { //gerar symbol
    	id = getIdent(pvars);
    	ste = newSymbEntry(S_VARIABLE,id);
    	ste->level = currentLevel; ste->descr->displ = currentDispl;
    	ste->descr->type = ptype;
    	insertSymbolTable(ste);  currentDispl+= ptype->size;
	}
}

TypeDescrPtr processExpr(TreeNodePtr p) {
    switch(p->categ) {
        case C_IDENT:    return processExprIdent(p);
        case C_UN_EXPR:  return processUnExpr(p);
        case C_BIN_EXPR: return processBinExpr(p);
        ...
	}
}

TypeDescrPtr processUnExpr(TreeNodePtr p) {
    char *op = p->comps[0]->str;
    TypeDescrPtr t = processExpr(p->comps[1]);
    if (!compatibleTypesUnOp(op,t))
        SemanticError();
    genCode0(mepaUnInstr(op));
    return t;
 }

 TypeDescrPtr processBinExpr(TreeNodePtr p) {
    TypeDescrPtr t0 = processExpr(p->comps[0]);
    TypeDescrPtr t1 = processExpr(p->comps[2]);
    char *op = getOp(p->comps[1]);
    if (!compatibleTypesBinOp(op,t0,t1))
        SemanticError();
    genCode0(mepaBinInstr(op));
    if (relational(op))
        return predefBool;
    else
        return predefInt;
}


/*
void processFuncDeclaration (TreeNodePtr np, int ismain) {	
	char *name = np->comps[1]->str;
	TypeDescrPtr retType = NULL, funType;
	int lastDispl = -4, entLabel,bdLabel,retLabel ;
	SymbEntryPtr formals, func;

	entLabel = newlabel();

	// 
	if (np->comps[0]->str != NULL) {
		SymbEntryPtr aux = getSymbol(np->comps[0]->str, 1);
		retType = aux->type;
	}

	if (!ismain){
		genCodeLabel1("ENFN", label,k);
		t = type(T_FUNCTION,retType,0);
	}
}*/
