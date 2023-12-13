# include "statement.c"
// 这个函数的作用是：处理常量声明
void constdeclaration() {
    if (last_symb_readed == SYMBOL_IDENTIFIER) {
        get_one_symbol();
        if (last_symb_readed == SYMBOL_EQU || last_symb_readed == SYMBOL_BECOMES) {
            if (last_symb_readed == SYMBOL_BECOMES)
                error(1); // Found ':=' when expecting '='., 说明应该是等号而不是赋值号
            get_one_symbol();
            if (last_symb_readed == SYMBOL_NUMBER) {
                enter(ID_CONSTANT);
                get_one_symbol();
            }
            else {
                error(2); // There must be a number to follow '='.
            }
        }
        else {
            error(3); // There must be an '=' to follow the identifier.
        }
    }
    else
        error(4);
    // There must be an identifier to follow 'const', 'var', or 'procedure'.
} // constdeclaration

//////////////////////////////////////////////////////////////////////
void vardeclaration(void) {
    if (last_symb_readed == SYMBOL_IDENTIFIER) {
        enter(ID_VARIABLE);
        get_one_symbol();
    }
    else {
        error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
    }
} // vardeclaration

//block的作用是：处理常量、变量、过程的声明，处理语句
void block(symset fsys) {//fsys是一个集合，里面存放的是声明开始符号、语句开始符号和'.'这三个符号
    int cx0; // initial code index
    mask *mk; // 这是一个结构体，用来存储符号表的信息
    int block_dx; // 记录当前层的数据段的大小
    int savedTx; // 保存当前符号表的位置
    symset set1, set; 

    data_alloc_idx = 3;
    block_dx = data_alloc_idx; 
    mk = (mask *)&table[curr_table_index]; // mk指向符号表的最后一个元素
    mk->address = curr_inst_index;
    gen(JMP, 0, 0);
    if (curr_block_level > MAXLEVEL) {
        error(32); // There are too many levels.
    }
    do {
        if (last_symb_readed == SYMBOL_CONST) { // constant declarations
            get_one_symbol();
            do {
                constdeclaration();
                while (last_symb_readed == SYMBOL_COMMA) {
                    get_one_symbol();
                    constdeclaration();
                }
                if (last_symb_readed == SYMBOL_SEMICOLON) {
                    get_one_symbol();
                }
                else {
                    error(5); // Missing ',' or ';'.
                }
            } while (last_symb_readed == SYMBOL_IDENTIFIER);
        } // if

        //这里的var定义应该有问题
        if (last_symb_readed == SYMBOL_VAR) { // variable declarations
            get_one_symbol();
            do {
                vardeclaration();
                while (last_symb_readed == SYMBOL_COMMA) {
                    get_one_symbol();
                    vardeclaration();
                }
                if (last_symb_readed == SYMBOL_SEMICOLON) {
                    get_one_symbol();
                }
                else {
                    error(5); // Missing ',' or ';'.
                }
            } while (last_symb_readed == SYMBOL_IDENTIFIER); //这里和PL0给的var定义格式不一样
        } // if

        block_dx = data_alloc_idx; // save dx before handling procedure call!
        while (last_symb_readed == SYMBOL_PROCEDURE) { // procedure declarations
            get_one_symbol();
            if (last_symb_readed == SYMBOL_IDENTIFIER) {
                enter(ID_PROCEDURE);
                get_one_symbol();
            }
            else {
                error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
            }

            if (last_symb_readed == SYMBOL_SEMICOLON) {
                get_one_symbol();
            }
            else {
                error(5); // Missing ',' or ';'.
            }

            curr_block_level++;
            savedTx = curr_table_index;
            set1 = createset(SYMBOL_SEMICOLON, SYMBOL_NULL);
            set = uniteset(set1, fsys);
            block(set); //处理过程体，过程体内部可能还有过程体
            destroyset(set1);
            destroyset(set);
            curr_table_index = savedTx;
            curr_block_level--;

            if (last_symb_readed == SYMBOL_SEMICOLON) {
                get_one_symbol();
                set1 = createset(SYMBOL_IDENTIFIER, SYMBOL_PROCEDURE, SYMBOL_NULL);
                set = uniteset(stat_begin_sys, set1);
                test(set, fsys, 6); //检测当前读取的符号是不是语句开始符号或者是过程开始符号
                //这里是测试当前读取的符号是不是语句开始符号或者是过程开始符号
                destroyset(set1);
                destroyset(set);
            }
            else {
                error(5); // Missing ',' or ';'.
            }
        }              // while
        data_alloc_idx = block_dx; // restore dx after handling procedure call!
        set1 = createset(SYMBOL_IDENTIFIER, SYMBOL_NULL);
        set = uniteset(stat_begin_sys, set1);
        test(set, decl_begin_sys, 7); // 测试当前读取的符号是不是语句开始符号或者是声明开始符号
        destroyset(set1);
        destroyset(set);
    } while (inset(last_symb_readed, decl_begin_sys));//如果在声明
    //上面这部分都在处理声明


    code[mk->address].addr = curr_inst_index;
    mk->address = curr_inst_index;
    cx0 = curr_inst_index;
    gen(INT, 0, block_dx);
    set1 = createset(SYMBOL_SEMICOLON, SYMBOL_END, SYMBOL_NULL);
    set = uniteset(set1, fsys);
    statement(set); // 处理语句
    destroyset(set1);
    destroyset(set);
    gen(OPR, 0, OPR_RET); // return
    test(fsys, phi, 8);   // test for error: Follow the statement is an incorrect symbol.
    listcode(cx0, curr_inst_index);
} // block