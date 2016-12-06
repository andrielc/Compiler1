#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tree.h"

const char *CATEGS[20]={
	"C_FUNCTION",
	"C_BLOCK",
	"C_LABEL",
	"C_TYPE",
	"C_VARIABLE",
	"C_POINTER",
	"C_STATEMENT",
	"C_IF",
	"C_RETURN",
	"C_GOTO",
	"C_BIN_EXPR",
	"C_EXPR",
	"C_NEG",
	"C_ASSIGN",
	"C_WHILE",
	"C_FUNCTION_CALL",
	"C_IDENT",
	"C_INTEGER",
	"C_NOT",
	"C_OPERATOR"
};

extern TreeNodePtr tree;
extern int stackheight;

int stackHeight() {
	return stackheight;
}

void *getTree() {
	return tree;
}

TreeNodePtr genNode2(Categ cat, char *str){

	TreeNodePtr np = malloc(sizeof(TreeNode));
	np->categ = cat;
	np->next = NULL;
	np->str = str;

	return np;
	
}

TreeNodePtr genNode3(Categ cat, char *str,TreeNodePtr p1){
	
	TreeNodePtr np = malloc(sizeof(TreeNode));
	np->categ = cat;
	np->next = NULL;
	np->str = str;

	np->comps[0] = p1;
	return np;
}

TreeNodePtr genNode4(Categ cat, char *str,TreeNodePtr p1, TreeNodePtr p2){
	
	TreeNodePtr np = malloc(sizeof(TreeNode));
	np->categ = cat;
	np->next = NULL;
	np->str = str;

	np->comps[0] = p1;
	np->comps[1] = p2;
	return np;
}

TreeNodePtr genNode5(Categ cat, char *str,TreeNodePtr p1, TreeNodePtr p2, TreeNodePtr p3){
	
	TreeNodePtr np = malloc(sizeof(TreeNode));
	np->categ = cat;
	np->next = NULL;
	np->str = str;
	
	np->comps[0] = p1;
	np->comps[1] = p2;
	np->comps[2] = p3;
	return np;
}

TreeNodePtr genNode6(Categ cat, char *str,TreeNodePtr p1, TreeNodePtr p2, TreeNodePtr p3,TreeNodePtr p4){

	TreeNodePtr np = malloc(sizeof(TreeNode));
	np->categ = cat;
	np->next = NULL;
	np->str = str;

	np->comps[0] = p1;
	np->comps[1] = p2;
	np->comps[2] = p3;
	np->comps[3] = p4;
	return np;
}

TreeNodePtr genNode7(Categ cat, char *str,TreeNodePtr p1, TreeNodePtr p2, TreeNodePtr p3,TreeNodePtr p4,TreeNodePtr p5){
	TreeNodePtr np = malloc(sizeof(TreeNode));
	np->categ = cat;
	np->next = NULL;
	np->str = str;

	np->comps[0] = p1;
	np->comps[1] = p2;
	np->comps[2] = p3;
	np->comps[3] = p4;
	np->comps[4] = p5;
	return np;
}


void dumpTree(TreeNodePtr p, int indent, char *tab){
	int i=0;
	
	while(i++<indent)
		 printf("%s",tab);
	
	if(p->str) 
		printf("Data: %s ",p->str);
	fflush(stdout);
	
	printf("Categ: %s\n",CATEGS[p->categ]);
	fflush(stdout);
	
	i=0;
	while(i<6 && p->comps[i])
		dumpTree(p->comps[i++],indent+1,tab);
	
	if(p->next)
		dumpTree(p->next,indent,tab);
}

TreeNodePtr inversao (TreeNodePtr tnp) {
	TreeNodePtr aux, np = NULL;
	while(tnp){
		aux = tnp->next;
		tnp->next = np;
		np = tnp;
		tnp = aux;
	}
	return np;
}
