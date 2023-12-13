# include "error.c"

// 这个函数的作用是：获得一个字符
void getch(void) {
    if (char_count == line_length) { //检查当前行是否结束，如果结束直接读取下一行的第一个字符否则直接读取下一个字符
        if (feof(infile)) { //判断文件是否结束
            printf("\nPROGRAM INCOMPLETE\n"); //输出程序不完整
            exit(1);
        }
        line_length = char_count = 0;
        printf("%5d  ", curr_inst_index); //输出指令序号
        while ((!feof(infile)) && ((last_char_readed = getc(infile)) != '\n')) {
        //如果文件没有结束，且读取的字符不是换行符
            printf("%c", last_char_readed);
            line[++line_length] = last_char_readed;
        } // while 读取一整行
        printf("\n");
        line[++line_length] = ' ';
    }
    last_char_readed = line[++char_count]; //读取下一个字符
} // getch

// gets a symbol from input stream.
// 获得一个符号
void get_one_symbol(void) {
    int flag_of_sym, index_of_word;
    char word_buf[MAXIDLEN + 1]; // 词法分析的缓冲区

    while (last_char_readed == ' ' || last_char_readed == '\t')
        getch();

    if (isalpha(last_char_readed)) {   // isalpha()函数用来判断一个字符是否为字母
        // symbol is a reserved word or an identifier.
        index_of_word = 0;
        do {
            if (index_of_word < MAXIDLEN)
                word_buf[index_of_word++] = last_char_readed;
            getch();
        } while (isalpha(last_char_readed) || isdigit(last_char_readed));

        //下面这段代码非常迷惑
        word_buf[index_of_word] = 0;
        strcpy(last_id_readed, word_buf); // strcpy把word的内容复制到last_id_readed中
        word[0] = last_id_readed; // word[0]是一个指针，指向last_id_readed
        flag_of_sym = NRW;
        while (strcmp(last_id_readed, word[flag_of_sym--])) ;
        //上述循环的作用是：判断last_id_readed是否是保留字，如果是返回1，否则返回0
        if (++flag_of_sym)
            last_symb_readed = wsym[flag_of_sym]; // symbol is a reserved word
        else
            last_symb_readed = SYMBOL_IDENTIFIER; // symbol is an identifier


    }
    else if (isdigit(last_char_readed)) { // isdigit()函数用来判断一个字符是否为数字
        // symbol is a number.
        index_of_word = last_num_readed = 0;
        last_symb_readed = SYMBOL_NUMBER;
        do {
            last_num_readed = last_num_readed * 10 + last_char_readed - '0';
            index_of_word++;
            getch();
        } while (isdigit(last_char_readed));
        if (index_of_word > MAXNUMLEN)
            error(25); // The number is too great.
    }
    else if (last_char_readed == ':') {
        getch();
        if (last_char_readed == '=') {
            last_symb_readed = SYMBOL_BECOMES; // :=
            getch();
        }
        else {
            last_symb_readed = SYMBOL_NULL; // illegal?
        }
    }
    else if (last_char_readed == '>') {
        getch();
        if (last_char_readed == '=') {
            last_symb_readed = SYMBOL_GEQ; // >=
            getch();
        }
        else {
            last_symb_readed = SYMBOL_GTR; // >
        }
    }
    else if (last_char_readed == '<') {
        getch();
        if (last_char_readed == '=') {
            last_symb_readed = SYMBOL_LEQ; // <=
            getch();
        }
        else if (last_char_readed == '>') {
            last_symb_readed = SYMBOL_NEQ; // <>
            getch();
        }
        else {
            last_symb_readed = SYMBOL_LES; // <
        }
    }
    else { // other tokens
        flag_of_sym = NSYM;
        csym[0] = last_char_readed;
        while (csym[flag_of_sym--] != last_char_readed) ;
        if (++flag_of_sym) {
            last_symb_readed = ssym[flag_of_sym];
            getch();
        }
        else {
            printf("Fatal Error: Unknown character.\n");
            exit(1);
        }
    }
} // getsym