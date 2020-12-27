#ifndef _CODE_GEN_H_
#define _CODE_GEN_H_

#include "ast.h"

typedef enum {
    OPERAND_VAR, OPERAND_CONST,
    OPERAND_TEMP, OPERAND_LABEL
} OperandKind;

typedef struct {
    OperandKind kind;
    union {
        // SymtableEntry *symbol; // 表示源程序中的变量在符号表中的指针
        // double value; // 表示字面量常量
        int tempvar_index; // 表示中间变量索引
        int label_index; // 表示label的索引
        struct {
            Token token; // 表示变量的token，为了输出变量名
            int index; // 表示变量的索引
        } var_index;
    } attr;
} Operand;

typedef enum {
    OP_ADD, OP_SUB, OP_MUL, OP_DIV,
    OP_ASSIGN, OP_EQUAL, OP_LESS,
    OP_LESS_E, OP_GREATE, OP_GREATE_E, OP_GOTO, OP_LABEL,
    OP_ERROR
} InterCodeOp;

struct _InterCode {
    InterCodeOp  op;
    Operand *arg1;
    Operand *arg2;
    Operand *result;
    struct _InterCode *next;
    struct _InterCode *prev;
};

typedef struct _InterCode InterCode;

DArray *code_generation(AstNode *, FILE *);

#endif
