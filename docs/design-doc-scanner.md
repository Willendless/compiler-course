# design doc 1: scanner

## 初步计划

+ 初步测试 - 1天
+ repl方便调试 - 1天
+ 测试 - 0.5天

## 数据结构

### Token

```c
#define TOKEN_MAX_LEN 120

struct Token {
    char lexme[TOKEN_MAX_LEN];
    TokenType type;
    union {
        double d_v;
        int i_v;
    } token_val;
    int line_num;
    int line_pos;
};
```

### TokenType

```c
enum TokenType {
    // keywords
    INT, REAL, IF, THEN, ELSE, WHILE,

    // literal
    IDENTIFIER, NUM_INT, NUM_DOUBLE,

    // delimeters
    LEFT_PAREN, RIGHT_PAREN, // "(", ")"
    LEFT_BRACE, RIGHT_BRACE, // "{", "}"
    COMMA, // ","
    DOT, // "."
    SEMICOLON, // ";"

    // operators
    PLUS, MINUS, // "+", "-"
    STAR, SLASH, // "*", "/"

    // one or two character tokens
    EQUAL, EQUAL_EQUAL, // "=", "=="
    LESS, LESS_EQUAL, // "<", "<="
    GREATER, GREATER_EQUAL, // ">", ">="
    BANG_EQUAL // "!="

}
```

## 算法

1. repl
2. scanner: 基于三个底层函数
    + advance(): cur指针递增，返回上一个字符
    + peek(): 返回当前字符
    + match(char): 匹配当前字符，若匹配成功，cur指针递增 

### TODO: double buffering缓冲区

### 错误处理

当程序出现词法错误时，采取的方式是，跳过当前错误部分，返回下一个正常的token。对于parser来说，其调用`scan_token()`接受到的均是正常的token，因此相当于对parser隐藏了词法错误信息。

#### 示例

例如，对于如下程序：

```
{
    $$$$$
    x = 3;
}
```

parser仍然能够正常输出语法树。

#### 输出格式

出现未定义字符以及不符合词法单元定义的字符，输出下列格式的信息：

```Error type: lexer at Line [行号]：[说明文字]```
