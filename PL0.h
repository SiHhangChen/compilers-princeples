#include <stdio.h>

#define NRW        11     // number of reserved words,即保留字的个数
#define TXMAX      500    // length of identifier table
#define MAXNUMLEN  14     // maximum number of digits in numbers
#define NSYM       10     // maximum number of symbols in array ssym and csym
#define MAXIDLEN   10     // length of identifiers，即标识符的最大长度

#define MAXADDRESS 32767  // maximum address
#define MAXLEVEL   32     // maximum depth of nesting block
#define CXMAX      500    // size of code array

#define MAXSYM     30     // maximum number of symbols  

#define STACKSIZE  1000   // maximum storage

//这部分是表示符号类型
enum symbol_type {
	SYMBOL_NULL,
	SYMBOL_IDENTIFIER, //标识符
	SYMBOL_NUMBER, //数字
	SYMBOL_PLUS, //加号
	SYMBOL_MINUS, //减号
	SYMBOL_TIMES, //乘号
	SYMBOL_SLASH, //除号
	SYMBOL_ODD, //odd
	SYMBOL_EQU, //等号
	SYMBOL_NEQ, //不等号
	SYMBOL_LES, //小于号
	SYMBOL_LEQ, //小于等于号
	SYMBOL_GTR, //大于号
	SYMBOL_GEQ, //大于等于号
	SYMBOL_LPAREN, //左括号
	SYMBOL_RPAREN, //右括号
	SYMBOL_COMMA,//逗号
	SYMBOL_SEMICOLON,//分号
	SYMBOL_PERIOD, //句号
	SYMBOL_BECOMES, //赋值号
    SYMBOL_BEGIN, //begin
	SYMBOL_END, //end
	SYMBOL_IF, //if
	SYMBOL_THEN, //then
	SYMBOL_WHILE, //while
	SYMBOL_DO, //do
	SYMBOL_CALL, //call
	SYMBOL_CONST, //const
	SYMBOL_VAR, //var
	SYMBOL_PROCEDURE //procedure
};

enum idtype {
	ID_CONSTANT, ID_VARIABLE, ID_PROCEDURE
};

enum opcode {
	LIT, OPR, LOD, STO, CAL, INT, JMP, JPC
};

enum oprcode {
	OPR_RET, OPR_NEG, OPR_ADD, OPR_MIN,
	OPR_MUL, OPR_DIV, OPR_ODD, OPR_EQU,
	OPR_NEQ, OPR_LES, OPR_LEQ, OPR_GTR,
	OPR_GEQ
};


typedef struct {
	int func_code; // function code
	int level; // level
	int addr; // displacement address， 位移地址
} instruction;

//////////////////////////////////////////////////////////////////////
char* err_msg[] =
{
/*  0 */    "",
/*  1 */    "Found ':=' when expecting '='.",
/*  2 */    "There must be a number to follow '='.",
/*  3 */    "There must be an '=' to follow the identifier.",
/*  4 */    "There must be an identifier to follow 'const', 'var', or 'procedure'.",
/*  5 */    "Missing ',' or ';'.",
/*  6 */    "Incorrect procedure name.",
/*  7 */    "Statement expected.",
/*  8 */    "Follow the statement is an incorrect symbol.",
/*  9 */    "'.' expected.",
/* 10 */    "';' expected.",
/* 11 */    "Undeclared identifier.",
/* 12 */    "Illegal assignment.",
/* 13 */    "':=' expected.",
/* 14 */    "There must be an identifier to follow the 'call'.",
/* 15 */    "A constant or variable can not be called.",
/* 16 */    "'then' expected.",
/* 17 */    "';' or 'end' expected.",
/* 18 */    "'do' expected.",
/* 19 */    "Incorrect symbol.",
/* 20 */    "Relative operators expected.",
/* 21 */    "Procedure identifier can not be in an expression.",
/* 22 */    "Missing ')'.",
/* 23 */    "The symbol can not be followed by a factor.",
/* 24 */    "The symbol can not be as the beginning of an expression.",
/* 25 */    "The number is too great.",
/* 26 */    "",
/* 27 */    "",
/* 28 */    "",
/* 29 */    "",
/* 30 */    "",
/* 31 */    "",
/* 32 */    "There are too many levels."
};

//////////////////////////////////////////////////////////////////////
char last_char_readed;              // last character read
int  last_symb_readed;              // last symbol read
char last_id_readed[MAXIDLEN + 1];  // last identifier read,即最后读到的标识符
int  last_num_readed;               // last number read
int  char_count;                    // character count
int  line_length;                   // line length 
int  program_text_index;            // program text index,即下一个要读的字符的位置
int  error_flag;                    // error flag         
int  curr_inst_index;               // index of current instruction to be generated. 即下一个要生成的指令的位置
int  curr_block_level = 0;          // current block level 即当前的层次
int  curr_table_index = 0;          // current table index 即当前的符号表的位置
int  data_alloc_idx;                // data allocation index, 用来记录当前层的数据段的大小
char line[80];

instruction code[CXMAX];
// 保留字
char* word[NRW + 1] =
{
	"", /* place holder */
	"begin", "call", "const", "do", "end","if",
	"odd", "procedure", "then", "var", "while"
};

//保留字的符号值
int wsym[NRW + 1] =
{
	SYMBOL_NULL, SYMBOL_BEGIN, SYMBOL_CALL, SYMBOL_CONST, SYMBOL_DO, SYMBOL_END,
	SYMBOL_IF, SYMBOL_ODD, SYMBOL_PROCEDURE, SYMBOL_THEN, SYMBOL_VAR, SYMBOL_WHILE
};

int ssym[NSYM + 1] =
{
	SYMBOL_NULL, SYMBOL_PLUS, SYMBOL_MINUS, SYMBOL_TIMES, SYMBOL_SLASH,
	SYMBOL_LPAREN, SYMBOL_RPAREN, SYMBOL_EQU, SYMBOL_COMMA, SYMBOL_PERIOD, SYMBOL_SEMICOLON
};

// csym的意思是
char csym[NSYM + 1] =
{
	' ', '+', '-', '*', '/', '(', ')', '=', ',', '.', ';'
};

#define MAXINS   8
char* mnemonic[MAXINS] =
{
	"LIT", "OPR", "LOD", "STO", "CAL", "INT", "JMP", "JPC"
};

typedef struct
{
	char name[MAXIDLEN + 1];
	int  kind;
	int  value;
} comtab; // comtab的意思是:常量表

comtab table[TXMAX]; //常量表

typedef struct {
	char  name[MAXIDLEN + 1];
	int   kind;
	short level;
	short address;
} mask; 
// mask的意思是:变量

//在符号表table中存在常量和变量，而mask是用来表示变量的，comtab是用来表示常量的

FILE* infile;

//error.c
void error(int n); // 错误处理函数
void test(symset s1, symset s2, int n); // 先判断上一个读取的符号是否在s1中，如果不在则报错，然后跳过所有不在s1和s2中的符号
//word.c
void getch(void); // 从输入流中读取一个字符
void get_one_symbol(void); // 从输入流中读取一个symbol
//block.c
void constdeclaration(); // 处理const声明
void vardeclaration(void); // 处理var声明
void block(symset fsys); // 处理block,即语法分析
//statement.c
void factor(symset fsys); // 处理因子
void term(symset fsys); // 处理项
void expression(symset fsys); // 处理表达式
void condition(symset fsys); // 处理条件
void statement(symset fsys); // 处理语句
//base.c
int position(char* id); // 在符号表中查找标识符的位置
void enter(int kind); // 把常量、变量、过程的信息写入符号表
void gen(int x, int y, int z); // 生成目标代码
void listcode(int from, int to); // 列出目标代码
//interpret.c
void interpret(void); // 解释执行

// EOF PL0.h
