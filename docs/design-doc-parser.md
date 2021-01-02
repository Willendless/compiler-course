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
typedef enum {AST_Stmt, AST_Expr, AST_Term} NodeType;
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

### 错误处理

由于lexer返回的均是合法的token，因此parser主要考虑如何处理语法错误。

#### panic mode算法

panic mode基本思想是parser不断忽略输入中的token，直到输入中出现能够匹配同步集合的某个token。下面基于当前分析栈的栈顶token的类型分类讨论。

##### 1. 栈顶是终结符号和当前token不匹配。

+ 输出错误信息“Missing [该终结符号]”。
+ 同时从分析栈中弹出该符号，并继续分析。

##### 2. 栈顶是非终结符号A，且当前token是a。而LL_PARSE_TABLE[A][a]为空。

+ 对非终结符号构造同步集合(包含first(A)和follow(A))，不断忽略token，直到能够同步。
+ 若当前lookahead token能够表示一个语句的结束，如`T_SEMICOLON`，`T_RIGHTBRACE`则pop stack，继续分析，而非忽略token。

#### 示例

+ 缺少某些必须的token，例如分号、"then"、"else"等
+ token不正确，例如`3=abc;`
+ 缺少

```
// 1.
{
    a = 3
}
// 2.
{
 if (a = 3) then {}
}
```
