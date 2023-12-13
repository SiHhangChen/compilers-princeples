#pragma warning(disable : 4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "PL0.h"
#include "set.c"

// print error message.
void error(int n) {
    int i;

    printf("      ");
    for (i = 1; i <= char_count - 1; i++)
        printf(" ");
    printf("^\n");
    printf("Error %3d: %s\n", n, err_msg[n]);
    error_flag++;
} // error

// tests if error occurs and skips all symbols that do not belongs to s1 or s2.
// 先判断上一个读取的符号是否在s1中，如果不在则报错，然后跳过所有不在s1和s2中的符号
void test(symset s1, symset s2, int n) {
    symset s;

    if (!inset(last_symb_readed, s1)) {
        error(n);
        s = uniteset(s1, s2);
        while (!inset(last_symb_readed, s))
            get_one_symbol();
        destroyset(s);
    }
} // test