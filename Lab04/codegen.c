#include "codegen.h"
#include "slc.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


int label=1,k=-1,returnDispl,returnLabel; //contar label
SymbEntryPtr symbTable = NULL;
TypeDescrPtr predefBool; 
TypeDescrPtr predefInt;


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

SymbEntryPtr symbol(SymbCateg categ, char *ident, int k, TypeDescrPtr type, int add , ... ){
    SymbEntryPtr se = malloc(sizeof(SymbEntry));
    se->categ = categ;
    se->ident = ident;
    se->k = k;
    se->type = type;
    
    if (add == 1) {
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
            se->descr.label.label = va_arg(va,int);
            se->descr.label.defined = va_arg(va,int);
            break;
        default:;
    }

    return se;
}


int newLabel () {
    return label++;
}

void initializeTypes(){
    predefBool = type (T_PREDEF,NULL,1,0,NULL);
    predefInt = type (T_PREDEF,NULL,1,0,NULL);
}

void initializeSymbolTable() {
    symbol (S_TYPE,"integer",-1,predefInt,1);
    symbol (S_TYPE,"boolean",-1,predefBool,1);
    symbol (S_CONST,"true",-1,predefBool,1,1);
    symbol (S_CONST,"false",-1,predefBool,1,0);
    symbol (S_FUNCTION,"read",-1,NULL,1,-1,1);
    symbol (S_FUNCTION,"write",-1,NULL,1,-1,1);
}

void processProgram(void *prog){
    //slide 196 "palavras reservadas"
    initializeTypes();
    initializeSymbolTable();
    genCode0("MAIN");
    processFuncDeclaration(prog,1);
    genCode0("END");
}

void genCode0(char *instr) {
    printf("\t%s\n",instr);
}

void genCode1(char *instr,int param) {
    printf("\t%s\t%d\n",instr,param);
}

void genCode2(char *instr, int param1, int param2){
    if (strcmp("CFUN",instr) == 0) {
        printf("\t%s\tL%d,%d\n",instr,param1,param2);
    }else{
        printf("\t%s\t%d,%d\n",instr,param1,param2);
    }
}

void genCodeJump(char *instr, int toLabel, int param1, int param2 ) {
    printf("\t%s\tL%d",instr,toLabel);
    if (strcmp(instr,"JMPL") == 0) {
        printf(",%d,%d",param1,param2);
    }
    printf("\n");
}

void genCodeLabel(char *instr, int label ) {
    printf("L%d:\t%s\n",label,instr);
}

void genCodeLabel1(char *instr, int label, int param1, int param2 ) {
    printf("L%d:\t%s\t%d", label, instr, param1);
    if (strcmp("ENLB",instr) == 0) {
        printf(",%d",param2);
    }
    printf("\n");
}


SymbEntryPtr findSymbol(char *c, int type){
    SymbEntryPtr sym = symbTable;
    //printf("Entrada: findSymbol: sym->ident %s\n",sym->ident);
    while(sym != NULL){
        if( strcmp(c,sym->ident)== 0 && (!type || sym->categ == S_TYPE)) {
            break;
        }
        //printf("findSymbol: sym->ident %s\n",sym->ident);
        sym=sym->next;
    }
    if(sym == NULL){
        SemanticError(NULL);
    }
    return sym;
}

void processTypes (TreeNodePtr tnp) {
    SymbEntryPtr aux = NULL;
    TypeDescrPtr t = NULL;
    while (tnp != NULL && strcmp(tnp->str,"empty")) {
        
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
    while (np != NULL && strcmp(np->str,"empty") != 0) {
        symbol(S_LABEL, np->str, k, NULL, 1, newLabel(), 0);
        np = np->next;
    }
    return;
}

SymbEntryPtr processFParams(TreeNodePtr np, int *lastDispl){
    TreeNodePtr tnp=NULL;
    SymbEntryPtr sep, next;
    if (np == NULL || (np->str != NULL && strcmp(np->str,"empty") == 0)){
	return NULL;
    }
    if (np->next)
    	next = processFParams(np->next,lastDispl);
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
    genCodeLabel("NOOP",label1);
    t = processExpr(p->comps[0]);
    if (t!=predefBool)
        SemanticError(NULL);
    genCodeJump("JMPF",label2,0,0);
    processStatements(p->comps[1]);
    //printf("agora eu acho que cheguei aqui\n");
    genCodeJump("JUMP",label1,0,0);
    //printf("opa\n");
    genCodeLabel("NOOP",label2);
}

void processStatements(TreeNodePtr np){
    SymbEntryPtr sym, syn;
    int alloc;
    while(np != NULL){
        //printf("np->categ: %d",np->categ);
        if(np->categ == C_LABEL){
            sym = findSymbol(np->comps[0]->str, 0);
            syn = symbTable;
            alloc = 0;
            while(syn && syn->k >= sym->k){
                if(syn->k == sym->k && syn->categ == S_VARIABLE)
                    alloc += syn->type->size;
                syn = syn->next;
            }
            genCodeLabel1("ENLB", sym->descr.label.label,sym->k, alloc);
            processStatement(np->comps[1]);
        }else{
            processStatement(np);
        }
        np=np->next;
    }
}

void processStatement(TreeNodePtr np){
    SymbEntryPtr sym;
 	printf("nois\n");
    switch(np->categ){
        case C_FUNCTION_CALL:
		processCall(np);
            break;
        case C_ASSIGN:
            processAssign(np);
            break;
        case C_IF:
            processConditional(np);
            break;
        case C_WHILE:
            processIteration(np);
            break;
        case C_GOTO:
            sym=findSymbol(np->comps[0]->str, 0);
            genCodeJump("JUMP",sym->descr.label.label,0,0);
            break;
        case C_RETURN:
            if(np->comps[0] != NULL && np->str != NULL){
                processExpr(np->comps[0]);
                genCode2("STVL",k,returnDispl);
            }
	    //printf("alo rics2\n");
            genCodeJump("JUMP", returnLabel,0,0);
            break;

        default:;
    }
}

void processCall(TreeNodePtr p){
    TreeNodePtr tn;
    SymbEntryPtr sym=findSymbol(p->comps[0]->str, 0);
	printf("lols\n");
    if(sym->k < 0){//nivel primitivo
        if(strcmp(p->comps[0]->str,"read") == 0){
            p=p->comps[1];
            while(p!=NULL){
                genCode0("READ");
                tn=p->comps[0]->comps[0]->comps[0];
                sym = findSymbol(tn->str, 0);
                if(sym->categ==S_PARAMETER && sym->descr.formParameter.pass==P_VARIABLE){
                    genCode2("STVI", sym->k, sym->descr.formParameter.displ);
                }else{
                    if(sym->categ==S_PARAMETER){
                        genCode2("STVL", sym->k, sym->descr.formParameter.displ);
                    }else{
                        genCode2("STVL", sym->k, sym->descr.variable.displ);
                    }
                //printf("%i ", findSymbol(tn->d,0)->descr.formalParam.pass);
                }
                p=p->next;
            }
        }else{
            p=p->comps[1];
            while(p!= NULL){
                processExpr(p); // TODO tem que verificar tipos dos parametros oh embaixo TODO TODO
                genCode0("PRNT");
                p=p->next;
            }
        }
    }else{
        //printf("oi\n");
        if(sym->type->type)
            genCode1("ALOC",sym->type->type->size);
        p=p->comps[1];
       while(p != NULL){
            processExpr(p); // TODO tem que verificar tipos dos parametros para passar por parametro
            p=p->next;
        }
        genCode2("CFUN", sym->descr.label.label, k);
    }
}

void processAssign(TreeNodePtr p) { 
	printf("aooooo\n");
	TreeNodePtr pvar  = p->comps[0];
	TreeNodePtr pexpr = p->comps[1];

	SymbEntryPtr ste = findSymbol(pvar->comps[0]->str,0);
    	
    	TypeDescrPtr tvar,texpr;
    	if (ste==NULL){
     	   SemanticError(NULL);
    	}

    	if ((ste->categ!=S_VARIABLE)&&(ste->categ!=S_PARAMETER))
       		SemanticError(NULL);
    
    	tvar = ste->type;
    	texpr = processExpr(pexpr);
    	if (texpr != NULL && (tvar -> constr != texpr-> constr) )
        	SemanticError(NULL);
    	if ( (ste->categ == S_PARAMETER) && (ste->descr.formParameter.pass == P_VARIABLE) ){
        	genCode2("STVI",ste->k,ste->descr.formParameter.displ);
    	}else{
		if(ste->categ == S_PARAMETER ){
        	genCode2("STVL",ste->k,ste->descr.formParameter.displ);
        }else{
            	genCode2("STVL",ste->k,ste->descr.variable.displ);
        }
    }


}

int processVariables(TreeNodePtr np){
	printf("oi\n");
    if(np == NULL || (np->str != NULL && strcmp(np->str,"empty") == 0)){
        return 0;
    }
    int display = processVariables(np->next);

    TreeNodePtr tn = NULL;
    TypeDescrPtr type;
    SymbEntryPtr aux = findSymbol(np->comps[1]->comps[0]->str, 1);
    type = aux->type;
    int i = 1;
    tn = np->comps[1]->comps[1];
    
    while(tn != NULL && strcmp(tn->str,"empty") != 0){
        i *= atoi(tn->str);
        tn = tn->next;
    }
    
    tn =  np->comps[0];
    while(tn != NULL){
        symbol(S_VARIABLE,tn->str,k,type,1, display);
        display += type->size*i;
        tn = tn->next;
    }
    return display;
}

/*void printTree (TreeNodePtr p){
    printf("====================\n");   
    dumpTree(p,0,"\t");
    printf("====================\n");
}*/
void processConditional(TreeNodePtr p){
    int label1 = newLabel();
    int label2 = newLabel();
    TypeDescrPtr aux = processExpr(p->comps[0]);
    if(aux != predefBool)
        SemanticError(NULL);
    genCodeJump("JMPF",label1,0,0);
    processStatements(p->comps[1]);
    if (p->comps[2] != NULL){
        genCodeJump("JUMP",label2,0,0);
    }   
    genCodeLabel("NOOP",label1);
    if(p->comps[2] != NULL) {
        processStatements(p->comps[2]);
        genCodeLabel("NOOP", label2);
    }
}

void restoreSymbTable() {
    while (symbTable->k > k) {
        symbTable = symbTable->next;
    }
}


TypeDescrPtr processExpr(TreeNodePtr np){ //MODIFICARRR
    TypeDescrPtr t1, t2, t3, t4;
    SymbEntryPtr sym;
	//printf("cheguei aqui\n");
    if(np->comps[0] != NULL && np->categ == C_EXPR){
	if(np->comps[1] != NULL && np->comps[1]->comps[0]->categ == C_OPERATOR){
            //printf("oi");
            t1=processExpr(np->comps[0]);
            np = np->comps[1];
            while(np != NULL){
                t2 = processExpr(np->comps[1]);
                t3 = processOperator(np->comps[0],&t4);
                checkTypes(t1,t2,t4);
                t1 = t3;
                np = np->next;
            }
        }else{
                return processExpr(np->comps[0]);
        }
    }else{
        switch(np->categ){
            case C_INTEGER: 
                genCode1("LDCT", atoi(np->str));
                t1 = predefInt;
                break;
            case C_VARIABLE:
                sym = findSymbol(np->comps[0]->str, 0); //identifier
                t1=sym->type;
                
                if( sym->categ == S_CONST){
                    genCode1("LDCT", sym->descr.constant.value);
                }else{
                    if(np->comps[1] != NULL && strcmp(np->comps[1]->str,"empty") != 0){
                        printf("lols\n");
                    }else{
                        if(sym->categ == S_PARAMETER && sym->descr.formParameter.pass == P_VARIABLE){
                            genCode2("LDVI", sym->k, sym->descr.formParameter.displ);
                        }else{
                            if(sym->categ==S_PARAMETER){
                                genCode2("LDVL",sym->k, sym->descr.formParameter.displ);
                            }else{
                                genCode2("LDVL",sym->k, sym->descr.variable.displ);
                                }
                            }
                        }
                    }
                break;
            case C_NEG:
                if(np->comps[1] && np->comps[1]->comps[0]->categ == C_OPERATOR){
                    t1 = processExpr(np->comps[0]);
                    genCode0("NEGT");
                    np = np->comps[1];
                    while(np != NULL){
                        t2 = processExpr(np->comps[1]);
                        t3 = processOperator(np->comps[0],&t4);
                        checkTypes(t1,t2,t4);
                        t1 = t3;
                        np = np->next;
                    }
                }
                else{
                    t1 = processExpr(np->comps[0]);
                    genCode0("NEGT");
                }
                break;
            case C_NOT:
                t1=processExpr(np->comps[0]);
                genCode0("LNOT");
                break;
            case C_BIN_EXPR:
                t1 = processExpr(np->comps[0]);
                t2 = processExpr(np->comps[2]);
                t3 = processOperator(np->comps[1], &t4);
                checkTypes(t1,t2,t4);
                if(t3 != predefBool){
                    SemanticError(NULL);
                }
                t1 = predefBool;
                break;
            case C_FUNCTION_CALL:
		//printf("fcal\n");
                processCall(np);
                break;
            default:
                //printf("Case default para: %d\n",np->categ);                
                ;
        }
    }
    return t1;
}

void checkTypes(TypeDescr *t1, TypeDescr *t2, TypeDescr *work){
    if(t1!=work || t2!=work)
        SemanticError(NULL);
}

TypeDescrPtr processOperator(TreeNodePtr p, TypeDescrPtr *t){ //MODIFICARRRR
    switch(p->str[0]){
        case '=':
            genCode0("EQUA");
            *t = predefInt;
            return predefBool;
        case '!':
            genCode0("DIFF");
            *t = predefInt;
            return predefBool;
        case '<':
            if(p->str[1]){
                genCode0("LEQU");
            }else{
                genCode0("LESS");
            }
            *t=predefInt;
            return predefBool;
        case '>':
            if(p->str[1]) {
                genCode0("GEQU");
            }else{
                genCode0("GRTR");
            }
            *t=predefInt;
            return predefBool;

        case '+':
            genCode0("ADDD");
            *t = predefInt;
            return predefInt;
        case '-':
            genCode0("SUBT");
            *t = predefInt;
            return predefInt;
        case '|':
            genCode0("LORR");
            *t = predefBool;
            return predefBool;
        case '*':
            genCode0("MULT");
            *t = predefInt;
            return predefInt;
        case '/':
            genCode0("DIVI");
            *t = predefInt;
            return predefInt;
        case '&':
            genCode0("LAND");
            *t = predefBool;
            return predefBool;
        default:
            SemanticError(NULL);
    }
    return NULL;
}



void processFuncDeclaration(TreeNodePtr np, int ismain) {   
    
    if(np == NULL || (np->str != NULL && strcmp(np->str,"empty") == 0) ){
        return;
    }
    char *name;
    TypeDescrPtr resType = NULL, funType = NULL;
    int lastDispl = -4,entLabel,bdLabel = 10000,retLabel;
    SymbEntryPtr formals, funSymbol;

    k++;
    name = np->comps[1]->str;
    

    if (np->comps[0]->str != NULL) { //tipo da funcao
        SymbEntryPtr aux = findSymbol(np->comps[0]->str, 1);
        resType = aux->type;
    }
    
    if (resType != NULL) { //alocar na stack retorno da funcao
        lastDispl -= resType->size;
    }
    if (ismain == 0) { //nao eh main
       
        entLabel = newLabel();
         
        if(np->comps[3] !=NULL && np->comps[3]->comps[3] != NULL && (np->comps[3]->comps[3]->str == NULL || (np->comps[3]->comps[3]->str && strcmp(np->comps[3]->comps[3]->str,"empty")!=0)))
        {
            bdLabel = newLabel();
        }
        
        retLabel = newLabel();
        


        
        genCodeLabel1("ENFN", entLabel,k,0);
        funType = type(T_FUNCTION,resType,0,0,NULL);
        funSymbol = symbol(S_FUNCTION,name,k-1,funType,1,entLabel,1);
    }
    else{
    
        retLabel = newLabel();
        
    
    }
    returnLabel = retLabel;
    //printf("ao\n");
    formals = processFParams(np->comps[2],&lastDispl);
    if(funType != NULL){
		//printf("funType diff null\n");
		funType->descr.func.params=formals;
    }
    np = np->comps[3]; //block
    
    processLabels(np->comps[0]);
    processTypes(np->comps[1]);

    int display = processVariables( np->comps[2] );
    if (display != 0){
        genCode1("ALOC",display);
    }
   
    if(np->comps[3] != NULL && np->comps[3]->str == NULL ){
	   genCodeJump("JUMP",retLabel,0,0);
    }
    
    processFuncDeclaration(np->comps[3],0);
    processFuncDeclaration(np->next,0);
    if(np->next == NULL ){
            if(bdLabel == 10000)
                genCodeLabel("NOOP",retLabel);
            else
                genCodeLabel("NOOP",bdLabel);
   
    }
        
	
    processStatements(np->comps[4]);
    
    if (display != 0){
        genCode1("DLOC",display);
    }
    if(ismain == 1) {
        genCode0("STOP");
    }else{
        if (resType != NULL){
            genCode1("RTRN", -lastDispl-4-1);
        }else{
            genCode1("RTRN", -lastDispl-4);
        }
    }
    k--;
    restoreSymbTable();
}
