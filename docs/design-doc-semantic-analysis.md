# design doc 3: semantic analysis

本文档介绍toy语言语义分析的实现。

## 语义错误的情况

由于toy语言不存在类型，因此语义分析只需要检查*变量是否定义*。考虑在parse阶段构造符号表。

## 数据结构

### 符号表

考虑使用hashtable记录已经定义的变量信息。

```c
typedef struct {
    
} SymtableEntry;
```

## 算法

遍历语法树，根据遇到的non-terminal的token类型分类讨论。

1. 当遇到`assgstmt`时，将第一个子节点token`ID`对应的变量名插入符号表
2. 当遇到`simpleexpr`且第一个子节点为`ID`时查表
    + 若未查到，则输出表示语义错误的信息。
