#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h> //处理可变参数的头文件
#include "set.h"

//合并集合
symset uniteset(symset source_set1, symset source_set2){
	symset set_head;
	snode* current_node;
	
	source_set1 = source_set1->next;
	source_set2 = source_set2->next;
	
	set_head = current_node = (snode*) malloc(sizeof(snode));
	while (source_set1 && source_set2) {//按序合并两个集合
		current_node->next = (snode*) malloc(sizeof(snode));
		current_node = current_node->next;
		if (source_set1->elem < source_set2->elem) {
			current_node->elem = source_set1->elem;
			source_set1 = source_set1->next;
		}
		else {
			current_node->elem = source_set2->elem;
			source_set2 = source_set2->next;
		}
	}

	while (source_set1) {
		current_node->next = (snode*) malloc(sizeof(snode));
		current_node = current_node->next;
		current_node->elem = source_set1->elem;
		source_set1 = source_set1->next;
	}

	while (source_set2)	{
		current_node->next = (snode*) malloc(sizeof(snode));
		current_node = current_node->next;
		current_node->elem = source_set2->elem;
		source_set2 = source_set2->next;
	}
	current_node->next = NULL;

	return set_head;
} // uniteset


//插入元素
void setinsert(symset source_set, int elem) {
	snode* current_node = source_set;
	snode* insert_node;

	while (current_node->next && current_node->next->elem < elem) {
		current_node = current_node->next;
	}
	
	insert_node = (snode*) malloc(sizeof(snode));
	insert_node->elem = elem;
	insert_node->next = current_node->next;
	current_node->next = insert_node;
} // setinsert


//创建集合,有一点问题在于：如果输入的参数本身是无序的，那么之后的insert操作毫无意义
symset createset(int elem, .../* SYM_NULL */) {
	va_list list; //va_list 是一个指针，指向参数列表中的第一个参数
    //va_list 宏实际上是一个指向参数列表的指针，通过这个指针可以访问传递给可变参数函数的所有参数。
	symset set;

	set = (snode*) malloc(sizeof(snode));
	set->next = NULL;

	va_start(list, elem);//这句代码的意思是从elem开始，依次取得各参数的值
	while (elem) {
		setinsert(set, elem); //递归处理，直到参数列表中的最后一个参数
		elem = va_arg(list, int);
	}
	va_end(list); //释放list
	return set;
} // createset


//销毁集合
void destroyset(symset source_set) {
	snode* current_node;

	while (source_set) {
		current_node = source_set;
		current_node->elem = -1000000;
		source_set = source_set->next;
		free(current_node);
	}
} // destroyset


//判断元素是否在集合中
int inset(int elem, symset source_set) {
	source_set = source_set->next;
	while (source_set && source_set->elem < elem)
		source_set = source_set->next;

	if (source_set && source_set->elem == elem)
		return 1;
	else
		return 0;
} // inset

// EOF set.c
