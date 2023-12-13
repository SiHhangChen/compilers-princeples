# include "set.h"
# include "PL0.h"

// 这个函数的作用是：生成目标代码
int base(int stack[], int currentLevel, int levelDiff) {
    int base_pointer = currentLevel;

    while (levelDiff--)
        base_pointer = stack[base_pointer];
    return base_pointer;
}// base函数的意思是：求层差为levelDiff的层的基地址

void interpret() {
    int pc; // program counter
    int stack[STACKSIZE];
    int top;          // top of stack
    int base_pointer; // program, base, and top-stack register
    instruction inst; // instruction register

    printf("Begin executing PL/0 program.\n");

    pc = 0;
    base_pointer = 1;
    top = 3;
    stack[1] = stack[2] = stack[3] = 0;
    do
    {
        inst.func_code = code[pc].func_code;
        inst.level = code[pc].level;
        inst.addr = code[pc++].addr;
        switch (inst.func_code)
        {
        case LIT:
            stack[++top] = inst.addr;
            break;
        case OPR:
            switch (inst.addr) // operator
            {
            case OPR_RET:
                top = base_pointer - 1;
                pc = stack[top + 3];
                base_pointer = stack[top + 2];
                break;
            case OPR_NEG:
                stack[top] = -stack[top];
                break;
            case OPR_ADD:
                top--;
                stack[top] += stack[top + 1];
                break;
            case OPR_MIN:
                top--;
                stack[top] -= stack[top + 1];
                break;
            case OPR_MUL:
                top--;
                stack[top] *= stack[top + 1];
                break;
            case OPR_DIV:
                top--;
                if (stack[top + 1] == 0)
                {
                    fprintf(stderr, "Runtime Error: Divided by zero.\n");
                    fprintf(stderr, "Program terminated.\n");
                    continue;
                }
                stack[top] /= stack[top + 1];
                break;
            case OPR_ODD:
                stack[top] %= 2;
                break;
            case OPR_EQU:
                top--;
                stack[top] = stack[top] == stack[top + 1];
                break;
            case OPR_NEQ:
                top--;
                stack[top] = stack[top] != stack[top + 1];
                break;
            case OPR_LES:
                top--;
                stack[top] = stack[top] < stack[top + 1];
                break;
            case OPR_GEQ:
                top--;
                stack[top] = stack[top] >= stack[top + 1];
                break;
            case OPR_GTR:
                top--;
                stack[top] = stack[top] > stack[top + 1];
                break;
            case OPR_LEQ:
                top--;
                stack[top] = stack[top] <= stack[top + 1];
                break;
            } // switch
            break;
        case LOD:
            stack[++top] = stack[base(stack, base_pointer, inst.level) + inst.addr];
            break;
        case STO:
            stack[base(stack, base_pointer, inst.level) + inst.addr] = stack[top];
            printf("%d\n", stack[top]);
            top--;
            break;
        case CAL:
            stack[top + 1] = base(stack, base_pointer, inst.level);
            // generate new block mark
            stack[top + 2] = base_pointer;
            stack[top + 3] = pc;
            base_pointer = top + 1;
            pc = inst.addr;
            break;
        case INT:
            top += inst.addr;
            break;
        case JMP:
            pc = inst.addr;
            break;
        case JPC:
            if (stack[top] == 0)
                pc = inst.addr;
            top--;
            break;
        } // switch
    } while (pc);

    printf("End executing PL/0 program.\n");
} // interpret