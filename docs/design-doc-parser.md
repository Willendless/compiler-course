# design doc 2: parser

本文档主要介绍toy编译器parser部分的设计。

## 架构

parser部分以lexer部分scan_token()产生的token作为输入。

## 算法

```
// 基于表驱动的算法实现生成ast树
word <- NextWord() // lookahead的token
push eof onto Stack
push the start symbol, S onto Stack
focus <- top of Stack // 当前需要match的位置
loop {
    if focus == eof and word == eof:
        success and break
    else if focus in T or focus = eof:
        if focus matches word:
            pop Stack
            word <- NextWord()
        else
            report error
    else:
        if Table[focus, word] is A->B1B2...Bk:
            pop Stack
            for i <- k to 1:
                if Bi not epsilon
                    push Bi onto Stack
        else
            report error
    focus <- top of Stack
}
```

+ 使用一个额外的语法树栈s记录ast信息
    1. 每一个non-terminal展开时，构造一个数组节点入栈
    2. 每个数组节点维护一个"当前处理到第i个节点的索引"
    3. 当i表明该数组节点均处理完成，则该节点pop出栈，同时bookeep此时栈内top节点的信息

## 数据结构

### AST数据结构

#### AST树的表示

+ 动态数组表示语法树栈成员，数组内部成员保存ast节点信息、指针，位置0维护索引

#### AST节点

```
// 节点类型
typedef enum {AST_Stmt, AST_Expr, AST_Nont} NodeType;
// stmt节点类型
typedef enum {
    STMT_PROGRAM
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
// ast节点结构
struct {
    NodeType type;
    union {
        StmtType stmt;
        ExprType expr;
    } NonTerminalType;
    union {
        Token *token; // terminal
        char *name; // non-terminal
    } attr;
    DArray ptrs; // arr for children
} AstNode;
```

### 
