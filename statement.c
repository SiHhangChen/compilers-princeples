# include "base.c"
# include "word.c"
// 这个函数的作用是：处理因子
void factor(symset fsys) {
    void expression(symset fsys);
    int i;
    symset set;

    test(fact_begin_sys, fsys, 24); // The symbol can not be as the beginning of an expression.

    if (inset(last_symb_readed, fact_begin_sys)) {
        if (last_symb_readed == SYMBOL_IDENTIFIER){
            if ((i = position(last_id_readed)) == 0) {
                error(11); // Undeclared identifier.
            }
            else {
                switch (table[i].kind) {
                    mask *mk;
                case ID_CONSTANT:
                    gen(LIT, 0, table[i].value);
                    break;
                case ID_VARIABLE:
                    mk = (mask *)&table[i];
                    gen(LOD, curr_block_level - mk->level, mk->address);
                    break;
                case ID_PROCEDURE:
                    error(21); // Procedure identifier can not be in an expression.
                    break;
                } // switch
            }
            get_one_symbol();
        }
        else if (last_symb_readed == SYMBOL_NUMBER) {
            if (last_num_readed > MAXADDRESS) {
                error(25); // The number is too great.
                last_num_readed = 0;
            }
            gen(LIT, 0, last_num_readed);
            get_one_symbol();
        }
        else if (last_symb_readed == SYMBOL_LPAREN) {
            get_one_symbol();
            set = uniteset(createset(SYMBOL_RPAREN, SYMBOL_NULL), fsys);
            expression(set);
            destroyset(set);
            if (last_symb_readed == SYMBOL_RPAREN) {
                get_one_symbol();
            }
            else {
                error(22); // Missing ')'.
            }
        }
        else if (last_symb_readed == SYMBOL_MINUS) {   // UMINUS,  Expr -> '-' Expr
            get_one_symbol();
            factor(fsys);
            gen(OPR, 0, OPR_NEG); 
        }
        test(fsys, createset(SYMBOL_LPAREN, SYMBOL_NULL), 23);
    } // if
} // factor

//////////////////////////////////////////////////////////////////////
// 这个函数的作用是：处理项
void term(symset fsys) {
    int mulop;
    symset set;

    set = uniteset(fsys, createset(SYMBOL_TIMES, SYMBOL_SLASH, SYMBOL_NULL));
    factor(set);
    while (last_symb_readed == SYMBOL_TIMES || last_symb_readed == SYMBOL_SLASH) {
        mulop = last_symb_readed;
        get_one_symbol();
        factor(set);
        if (mulop == SYMBOL_TIMES) {
            gen(OPR, 0, OPR_MUL);
        }
        else {
            gen(OPR, 0, OPR_DIV);
        }
    } // while
    destroyset(set);
} // term

//////////////////////////////////////////////////////////////////////
// 这个函数的作用是：处理表达式
void expression(symset fsys) {
    int addop;
    symset set;

    set = uniteset(fsys, createset(SYMBOL_PLUS, SYMBOL_MINUS, SYMBOL_NULL));

    term(set);
    while (last_symb_readed == SYMBOL_PLUS || last_symb_readed == SYMBOL_MINUS) {
        addop = last_symb_readed;
        get_one_symbol();
        term(set);
        if (addop == SYMBOL_PLUS) {
            gen(OPR, 0, OPR_ADD);
        }
        else {
            gen(OPR, 0, OPR_MIN);
        }
    } // while

    destroyset(set);
} // expression

//////////////////////////////////////////////////////////////////////
void condition(symset fsys) {
    int relop;
    symset set;

    if (last_symb_readed == SYMBOL_ODD) {
        get_one_symbol();
        expression(fsys);
        gen(OPR, 0, 6);
    }
    else {
        set = uniteset(relate_set, fsys);
        expression(set);
        destroyset(set);
        if (!inset(last_symb_readed, relate_set)) {
            error(20);
        }
        else {
            relop = last_symb_readed;
            get_one_symbol();
            expression(fsys);
            switch (relop) {
                case SYMBOL_EQU:
                    gen(OPR, 0, OPR_EQU);
                    break;
                case SYMBOL_NEQ:
                    gen(OPR, 0, OPR_NEQ);
                    break;
                case SYMBOL_LES:
                    gen(OPR, 0, OPR_LES);
                    break;
                case SYMBOL_GEQ:
                    gen(OPR, 0, OPR_GEQ);
                    break;
                case SYMBOL_GTR:
                    gen(OPR, 0, OPR_GTR);
                    break;
                case SYMBOL_LEQ:
                    gen(OPR, 0, OPR_LEQ);
                    break;
            } // switch
        }     // else
    }         // else
} // condition

//////////////////////////////////////////////////////////////////////
// 这个函数的作用是：处理语句
void statement(symset fsys) {
    int idx, cx1, cx2;
    symset set1, set;

    if (last_symb_readed == SYMBOL_IDENTIFIER) { // variable assignment
        mask *mk;
        if (!(idx = position(last_id_readed))) {
            error(11); // Undeclared identifier.
        }
        else if (table[idx].kind != ID_VARIABLE) {
            error(12); // Illegal assignment.
            idx = 0;
        }
        get_one_symbol();
        if (last_symb_readed == SYMBOL_BECOMES) {
            get_one_symbol();
        }
        else {
            error(13); // ':=' expected.
        }
        expression(fsys);
        mk = (mask *)&table[idx];
        if (idx) {
            gen(STO, curr_block_level - mk->level, mk->address);
        }
    }
    else if (last_symb_readed == SYMBOL_CALL) { // procedure call
        get_one_symbol();
        if (last_symb_readed != SYMBOL_IDENTIFIER) {
            error(14); // There must be an identifier to follow the 'call'.
        }
        else {
            if (!(idx = position(last_id_readed))) {
                error(11); // Undeclared identifier.
            }
            else if (table[idx].kind == ID_PROCEDURE) {
                mask *mk;
                mk = (mask *)&table[idx];
                gen(CAL, curr_block_level - mk->level, mk->address);
            }
            else {
                error(15); // A constant or variable can not be called.
            }
            get_one_symbol();
        }
    }
    else if (last_symb_readed == SYMBOL_IF) { // if statement
        get_one_symbol();
        set1 = createset(SYMBOL_THEN, SYMBOL_DO, SYMBOL_NULL);
        set = uniteset(set1, fsys);
        condition(set);
        destroyset(set1);
        destroyset(set);
        if (last_symb_readed == SYMBOL_THEN) {
            get_one_symbol();
        }
        else {
            error(16); // 'then' expected.
        }
        cx1 = curr_inst_index;
        gen(JPC, 0, 0);
        statement(fsys);
        code[cx1].addr = curr_inst_index;
    }
    else if (last_symb_readed == SYMBOL_BEGIN) { // block
        get_one_symbol();
        set1 = createset(SYMBOL_SEMICOLON, SYMBOL_END, SYMBOL_NULL);
        set = uniteset(set1, fsys);
        statement(set);
        while (last_symb_readed == SYMBOL_SEMICOLON || inset(last_symb_readed, stat_begin_sys)) {
            if (last_symb_readed == SYMBOL_SEMICOLON) {
                get_one_symbol();
            }
            else {
                error(10);
            }
            statement(set);
        } // while
        destroyset(set1);
        destroyset(set);
        if (last_symb_readed == SYMBOL_END) {
            get_one_symbol();
        }
        else {
            error(17); // ';' or 'end' expected.
        }
    }
    else if (last_symb_readed == SYMBOL_WHILE) { // while statement
        cx1 = curr_inst_index;
        get_one_symbol();
        set1 = createset(SYMBOL_DO, SYMBOL_NULL);
        set = uniteset(set1, fsys);
        condition(set);
        destroyset(set1);
        destroyset(set);
        cx2 = curr_inst_index;
        gen(JPC, 0, 0);
        if (last_symb_readed == SYMBOL_DO) {
            get_one_symbol();
        }
        else {
            error(18); // 'do' expected.
        }
        statement(fsys);
        gen(JMP, 0, cx1);
        code[cx2].addr = curr_inst_index;
    }
    test(fsys, phi, 19);
} // statement