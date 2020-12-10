#ifndef _TOKEN_H_
#define _TOKEN_H_

typedef enum {
    // keywords
    T_INT=1, T_REAL, T_IF, T_THEN, T_ELSE, T_WHILE,

    // literal
    T_IDENTIFIER, T_NUMBER,

    // delimeters
    T_LEFT_PAREN, T_RIGHT_PAREN,
    T_LEFT_BRACE, T_RIGHT_BRACE,
    T_COMMA, T_DOT, T_SEMICOLON,

    // operators
    T_PLUS, T_MINUS, T_STAR, T_SLASH,

    // one or two character tokens
    T_EQUAL, T_EQUAL_EQUAL,
    T_LESS, T_LESS_EQUAL,
    T_GREATER, T_GREATER_EQUAL,
    T_BANG_EQUAL,

    // eof
    T_EOF,
    T_ERROR

#ifdef VM

#endif

} TokenType;

typedef struct {
    char *start;
    int length;
    TokenType type;
    // union {
    //     double m_double;
    //     int m_int;
    // } value;
    int line_num;
    int line_pos;
} Token;

extern const char *token_type_string[];

#endif
