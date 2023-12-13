#ifndef SET_H 
#define SET_H

typedef struct snode
{
	int elem; 
	struct snode* next;
} snode, *symset;

//分别是空集
symset phi;
// 声明开始符号集: 常量、变量、过程
symset decl_begin_sys;
//语句开始符号集: 标识符、begin、call、if、while
symset stat_begin_sys;
//因子开始符号集: 标识符、数字、左括号、减号
symset fact_begin_sys;
//关系运算符集
symset relate_set;

//集合操作
symset createset(int data, .../* SYM_NULL */);
void destroyset(symset s);
symset uniteset(symset s1, symset s2);
int inset(int elem, symset s);

#endif
// EOF set.h
