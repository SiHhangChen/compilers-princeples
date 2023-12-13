#include "block.c"
#include "interpret.c"

int main(void) {
    FILE *hbin;
    char file_name[80];
    int i;
    symset set, set1, set2;

    printf("Please input source file name: "); // get file name to be compiled
    scanf("%s", file_name);
    if ((infile = fopen(file_name, "r")) == NULL) {
        printf("File %s can't be opened.\n", file_name);
        exit(1);
    }

    phi = createset(SYMBOL_NULL); // 加入空集
    relate_set = createset(SYMBOL_EQU, SYMBOL_NEQ, SYMBOL_LES, SYMBOL_LEQ, SYMBOL_GTR, SYMBOL_GEQ, SYMBOL_NULL);
    // 加入关系运算符集合
    //  create begin symbol sets
    decl_begin_sys = createset(SYMBOL_CONST, SYMBOL_VAR, SYMBOL_PROCEDURE, SYMBOL_NULL);
    stat_begin_sys = createset(SYMBOL_BEGIN, SYMBOL_CALL, SYMBOL_IF, SYMBOL_WHILE, SYMBOL_NULL);
    fact_begin_sys = createset(SYMBOL_IDENTIFIER, SYMBOL_NUMBER, SYMBOL_LPAREN, SYMBOL_MINUS, SYMBOL_NULL);

    error_flag = char_count = curr_inst_index = line_length = 0; // initialize global variables
    last_char_readed = ' ';
    program_text_index = MAXIDLEN;

    get_one_symbol();

    set1 = createset(SYMBOL_PERIOD, SYMBOL_NULL);
    set2 = uniteset(decl_begin_sys, stat_begin_sys);
    set = uniteset(set1, set2); //set现在是一个集合，里面存放的是声明开始符号、语句开始符号和'.'这三个符号
    block(set);
    destroyset(set1);
    destroyset(set2);
    destroyset(set);
    destroyset(phi);
    destroyset(relate_set);
    destroyset(decl_begin_sys);
    destroyset(stat_begin_sys);
    destroyset(fact_begin_sys);

    if (last_symb_readed != SYMBOL_PERIOD)
        error(9); // '.' expected.
    if (error_flag == 0) {
        hbin = fopen("hbin.txt", "w");
        for (i = 0; i < curr_inst_index; i++)
            fwrite(&code[i], sizeof(instruction), 1, hbin);
        fclose(hbin);
    }
    if (error_flag == 0)
        interpret();
    else
        printf("There are %d error(s) in PL/0 program.\n", error_flag);
    listcode(0, curr_inst_index);

    return 0;
} // main