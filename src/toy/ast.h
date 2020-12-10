#ifndef _AST_H_
#define _AST_H_

#include "token.h"
#include "../lib/darray.h"

// 节点类型
typedef enum {AST_Stmt, AST_Expr, AST_Nont} NodeType;

// stmt节点类型
typedef enum {
    STMT_PROGRAM,
    STMT_Stmt, STMT_Compound,
    STMT_Stmts, STMT_If,
    STMT_While, STMT_Assign
} StmtType;

// expr节点类型
typedef enum {
    EXPR_Bool = STMT_Assign + 1,
    EXPR_BoolOp,
    EXPR_Arith,
    EXPR_ArithPrime,
    EXPR_Mult,
    EXPR_MultPrime,
    EXPR_Simple
} ExprType;

typedef union {
    StmtType stmt;
    ExprType expr;
} NonterminalType;

// ast节点结构
typedef struct {
    NodeType type; // whether or not terminal
    NonterminalType subtype; // what type of nonterminal
    union {
        Token *token; // terminal
        char *name; // non-terminal
    } attr;
    int index; // record next children to be accessed
    DArray *ptrs; // arr for children
} AstNode;

AstNode *AstNode_init(NodeType type, NonterminalType subtype, Token *token, char *s);

#endif
