# include "error.c"

// locates identifier in symbol table.
// 这个函数的作用是：在符号表中查找标识符的位置
int position(char *id) {
    int i;
    strcpy(table[0].name, id); // strcpy把id的内容复制到table[0].name中
    i = curr_table_index + 1;
    while (strcmp(table[--i].name, id) != 0) ;
    return i;
} // position

// 这个函数的作用是：把常量、变量、过程的信息写入符号表
void enter(int kind) {
    mask *mk;

    curr_table_index++;
    strcpy(table[curr_table_index].name, last_id_readed);
    table[curr_table_index].kind = kind;
    switch (kind) {
        case ID_CONSTANT:
            if (last_num_readed > MAXADDRESS) {
                error(25); // The number is too great.
                last_num_readed = 0;
            }
            table[curr_table_index].value = last_num_readed;
            break;
        case ID_VARIABLE:
            mk = (mask *)&table[curr_table_index];
            mk->level = curr_block_level;
            mk->address = data_alloc_idx++;
            break;
        case ID_PROCEDURE:
            mk = (mask *)&table[curr_table_index];
            mk->level = curr_block_level;
            break;
    } // switch
} // enter

void gen(int x, int y, int z) {
    if (curr_inst_index > CXMAX) {
        printf("Fatal Error: Program too long.\n");
        exit(1);
    }
    code[curr_inst_index].func_code = x;
    code[curr_inst_index].level = y;
    code[curr_inst_index++].addr = z;
} // gen

void listcode(int from, int to) {
    int i;

    printf("\n");
    for (i = from; i < to; i++){
        printf("%5d %s\t%d\t%d\n", i, mnemonic[code[i].func_code], code[i].level, code[i].addr);
    }
    printf("\n");
} // listcode