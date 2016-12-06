#ifndef _TREE_H_
#define _TREE_H_
typedef enum{
	C_FUNCTION=0,
	C_BLOCK,
	C_LABEL,
	C_TYPE,
	C_VARIABLE,
	C_POINTER,
	C_STATEMENT,
	C_IF,
	C_RETURN,
	C_GOTO,
	C_BIN_EXPR,
	C_EXPR,
	C_NEG,
	C_ASSIGN,
	C_WHILE,
	C_FUNCTION_CALL,
	C_IDENT,
	C_INTEGER,
	C_NOT,
	C_OPERATOR
} Categ;

typedef struct _treeNode{
	Categ categ;
	struct _treeNode *next; //list
	struct _treeNode *comps[8];
	char *str;   // IDENT, INTEGER or operator
} TreeNode, *TreeNodePtr;

void *getTree();  // returns any pointer
TreeNodePtr genNode2(Categ cat, char *str);
TreeNodePtr genNode3(Categ cat, char *str, TreeNodePtr p1);
TreeNodePtr genNode4(Categ cat, char *str, TreeNodePtr p1, TreeNodePtr p2);
TreeNodePtr genNode5(Categ cat, char *str, TreeNodePtr p1, TreeNodePtr p2, TreeNodePtr p3);
TreeNodePtr genNode6(Categ cat, char *str, TreeNodePtr p1, TreeNodePtr p2, TreeNodePtr p3,TreeNodePtr p4);
TreeNodePtr genNode7(Categ cat, char *str, TreeNodePtr p1, TreeNodePtr p2, TreeNodePtr p3,TreeNodePtr p4,TreeNodePtr p5);
TreeNodePtr inversao(TreeNodePtr tnp);
int stackHeight();
void counts(void *p, int *functions, int *funcalls, int *whiles, int *ifs, int *bin);
void dumpTree(TreeNode *p, int indent,char *tab);

#endif
