# design doc 4: ir code generation

本文档主要介绍toy语言代码（三地址码）生成的实现。

## 架构简介

语义分析完成之后，保留下来的语法树存在很多冗余节点，采取SDT语法制导翻译首先转换成ast树。进而考虑再生成三地址码。

语法树 -> ast树 -> 三地址码

## 数据结构

### 三地址码表示

使用四元式来表示三地址码如下：

```c
#define MAX_LINE 1024

typedef enum {
    OPERAND_VAR, OPERAND_CONST,
    OPERAND_TEMP, OPERAND_LABEL
} OperandKind;

typedef struct {
    enum { VARIABLE, CONSTANT } kind;
    union {
        Token token;
        int tempvar_index; // 表示中间变量索引
        int label_index; // 表示label索引
    } u;
} *Operand;

typedef struct {
    enum { ADD, SUB, MUL, DIV, ASSIGN, EQUAL, LESS, LESS_E, GREATE, GREATE_E } op;
    Operand *arg1;
    Operand *arg2;
    Operand *result;
} InterCode;

InterCode *codes[MAX_LINE];
```

注意四元式(op, arg1, arg2, result)表示下的特例如下：

1. 对于单赋值形式例如`a = 1;`, op为`=`。
2. 对于条件及非条件转移指令，目标标号放在result中。

### 全局变量

```c
int tempvar_index = 1;
int label_index = 1;
```

## 算法

### 语法树 -> ast树

1. 对于A->epsilon的节点均删除，子节点为空的中间节点也删除
2. 对于除`{`和`}`外的terminal，直接生成节点

#### 对于语句树

```
program -> compoundstmt $
stmt -> ifstmt | whilestmt | assgstmt | compoundstmt $
compoundstmt -> { stmts } $
stmts -> stmt stmts | epsilon $
ifstmt -> if ( boolexpr ) then stmt else stmt $
whilestmt -> while ( boolexpr ) stmt $
assgstmt -> ID = arithexpr ; $
```

+ 对于program，返回compoundstmt结果
+ 对于stmt，返回子节点结果
+ 对于compoundstmt，返回stmts结果
+ 对于stmts，循环构造节点包含所有stmt结果
+ 对于ifstmt，构造节点返回boolexpr，stmt1，stmt2结果
+ 对于whilestmt，构造节点返回boolexpr，stmt结果
+ 对于assgstmt，构造节点返回ID和arithexpr结果

#### 对于表达式树

```
boolexpr -> arithexpr boolop arithexpr $
boolop -> < | > | <= | >= | == $
arithexpr -> multexpr arithexprprime $
arithexprprime -> + multexpr arithexprprime | - multexpr arithexprprime | epsilon $
multexpr -> simpleexpr multexprprime $
multexprprime -> * simpleexpr multexprprime | / simpleexpr multexprprime | epsilon $
simpleexpr -> ID | NUM | ( arithexpr ) $
```

+ 对于boolexpr，以boolop为根，左右作为子树
+ 对于boolop，返回和当前相同的节点
+ 对于arithexpr和multexpr，
  + 生成左子树
  + 生成右子树
  + 若右子树非空，则以右子树根为根
+ 对于arithexprime和multexprprime
  + 以第一个子节点为根
  + 如果arithexprprime非epsilon，则arithexprprime的返回值作为右子树，multexpr作为右子树的左节点
  + 如果arithexprprime为epsilon，则multexpr返回值作为右子树
+ 对于simpleexpr
  + 若为叶子，返回相同节点
  + 若为( arithexpr ), 返回arithexpr

### ast树 -> 三地址码

自顶向下遍历时翻译生成，同时传入上层提供的变量名。

ast树有以下类型节点：

non-terminal类型：

+ stmts
+ ifstmt
+ whilestmt
+ assgnstmt

terminal类型：

+ operator 操作符，可分为二元操作符和bool操作符两类
+ operand: 根据token类型可分为T_INDENTIFIER和T_NUMBER两类

#### 语句的翻译

`translate_Stmt(Stmt)`

+ ifstmt(stmt)
    ```
    label1 = new_label()
    label2 = new_label()
    label3 = new_label()
    [code1, cmp] = translate_expr_cond(stmt.1, label1, label2)
    code2 = translate_Stmt(Stmt1, sym_table)
    code3 = translate_Stmt(Stmt2, sym_table)
    return code1 + [LABEL label1] + code2 + [GOTO label3] + [LABEL label2] + code3 + [LABEL label3]
    ```
+ whilestmt(stmt)
  ```
  label1 = new_label()
  label2 = new_label()
  label3 = new_label()
  code1 = translate_cond(stmt.0, label2, label3)
  code2 = translate_stmt(stmt.1)
  return [LABEL label1] + code1 + [LABEL label2] + code2 + [goto label1] + [LABEL label3]

  ```
+ assigstmt(stmt)
  ```
  if T_[operation] {
    t1 = new_temp()
    code1 = translate_expr_arith(stmt.1, t1)
    code2 = [stmt.0 := t1]
    code = code1 + code 2
  } else {
    code = [stmt.0 := stmt.1 ]
  }
  return code
  ```
+ stmts:
    + 遍历翻译即可

#### 表达式的翻译

`translate_exp(exp, sym_table, place)`

对于表达式树，依据根节点类别分析

+ expr_arith(expr, place):
  ```
  if (expr.left is expr) {
    t1 = new_temp()
    code1 = translate_expr_arith(expr.left, t1)
  } else {
    t1 = expr.left
  }
  if (expr.right is expr) {
    t2 = new_temp()
    code2 = translate_expr_arith(expr.right, t2)
  } else {
    t2 = expr.right
  }
  code3 = [place = t1 + t2]
  return  code1 + code2 + code3
  ```
+ expr_cond(expr, label_true, label_false)
  ```
  if (expr.left is expr) {
    t1 = new_temp()
    code1 = translate_expr_arith(expr.left, t1)
  } else {
    t1 = expr.left
  }
  if (expr.right is expr) {
    t2 = new_temp()
    code2 = translate_expr_arith(expr.right, t2)
  } else {
    t2 = expr.right
  }
  code3 = [if t1 expr.op t2 goto label_true]
  return code1 + code2 + code3 + [goto label_false]
  ```
