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

### TODO: 符号表的设计