# llgen

llgen程序用于生成供toy编译器使用的LL(1) parse table。本doc主要包含llgen输入、输出文件格式说明两个部分。

## 1. llgen输入文件的格式说明

ll-spec文件被用于生成LL(1)算法的parse表。该文件由三个段构成：

### 1.1 terminal段

该段用于定义所有terminal，第一行应当为`[terminal]`。两个terminal之间用空白符（若没有特殊说明，本文空白符均包括‘ ’， ‘\t’，‘\n’，'\r'）分隔。

+ terminal段中每个terminal应当和token类型一一对应
+ 需要有epsilon，暂时和T_ERROR对应

### 1.2 non-terminal段

该段用于定义所有的non-terminal，第一行应当为`[nonterminal]`。两个non-terminal之间用空白符分隔。

> non-terminal不能包含epsilon。

### 1.3 production段

该段用于定义所有的production，第一行应当为`[production]`。head和product之间使用`->`以及一个或多个空白符分隔。两个不同head的production之间用换行符分隔。相同head的production之间使用`|`以及一个或多个空白符分隔。

> epsilon是保留字，用于支持空的情况。

### 示例：一个用于四则运算表达式解析的ll-spec文件

```
[terminal]
+ - * \
( )
num name
[nonterminal]
goal
expr
term
factor
[production]
goal -> expr
expr -> term expr'
expr' -> + term expr' | - term expr' | epsilon
term -> term * factor | term / factor | factor
term' -> * factor term' | / factor term' | epsilon
factor -> ( expr ) | num | name
```

## 2. llgen输出文件格式说明

llgen输出文件被用于实现toy语言的LL(1) parser。输出文件包含一个不完整的parser.c文件源文件。

其中仅包含两个常量数组：

+ 表示production的三维数组
+ 表示LL(1) parse table的二维数组
