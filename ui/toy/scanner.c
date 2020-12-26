#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "utils/debug.h"
#include "utils/bool.h"
#include "scanner.h"
#include "compiler.h"

#define T Token
#define TT TokenType

typedef struct {
    const char *start; // current beginning of the lexme
    const char *cur; // current character
    int line_num;
    int line_pos;
} Scanner;

Scanner scanner;

void init_scanner(const char *source) {
    scanner.start = source;
    scanner.cur = source;
    scanner.line_num = 1;
    scanner.line_pos = 0;
}

static bool is_at_end(void);
static char advance(void);
static bool match(char);
static char peek(void);
static char next(void);
static char next_next(void);
static char next_next_next();
static bool is_digit(char);
static void skip_whitespace();

static T number(void);
static bool is_fraction(void);
static void fraction(void);
static bool is_exponent(void);
static void exponent(void);

static bool is_alpha(char);
static T identifier(void);
static TT get_identifier_type(void);

static char comment(void);

static T make_token(TT);
static T error_token();

extern char compile_output[2048];

/**
 * Return a token each time being called or an error token with message.
 */
T scan_token() {
    // TODO(ljr): complete scanner
    char ch;
    scanner.start = scanner.cur;
    if (is_at_end()) {
        return make_token(T_EOF);
    }

    skip_whitespace();
    if (is_at_end()) {
        // eof after whitespace
        return make_token(T_EOF);
    }

    // ch = advance();
    ch = peek();

    // comment
    while (ch == '/' && next() == '/') {
        ch = comment();
        if (is_at_end()) {
            // eof after comment
            return make_token(T_EOF);
        }
        skip_whitespace();
        ch = peek();
        if (is_at_end()) {
            // eof after whitespace
            return make_token(T_EOF);
        }
    }
    // number
    if (is_digit(ch)) {
        return number();
    }
    // identifier
    if (is_alpha(ch)) {
        return identifier();
    }

    switch (ch) {
    // delimiters
    case '(': advance(); return make_token(T_LEFT_PAREN);
    case ')': advance(); return make_token(T_RIGHT_PAREN);
    case '{': advance(); return make_token(T_LEFT_BRACE);
    case '}': advance(); return make_token(T_RIGHT_BRACE);
    case ',': advance(); return make_token(T_COMMA);
    case ';': advance(); return make_token(T_SEMICOLON);
    // operators
    case '+': advance(); return make_token(T_PLUS);
    case '-': advance(); return make_token(T_MINUS);
    case '*': advance(); return make_token(T_STAR);
    case '/': advance(); return make_token(T_SLASH);
    // one or two character tokens
    case '=': return match('=') ? make_token(T_EQUAL_EQUAL) : make_token(T_EQUAL);
    case '<': return match('=') ? make_token(T_LESS_EQUAL) : make_token(T_LESS);
    case '>': return match('=') ? make_token(T_GREATER_EQUAL) : make_token(T_GREATER);
    case '!': if (match('=')) return make_token(T_BANG_EQUAL);
    default:
        break;
    }

    // advance();
    return error_token();
}

static inline void skip_whitespace(void) {
    char ch;
    while (TRUE) {
        ch = peek();
        switch (ch) {
        case ' ':
        case '\t':
        case '\r':
            scanner.start++;
            advance();
            break;
        case '\n':
            scanner.start++;
            scanner.line_num++;
            advance();
            scanner.line_pos = 0;
            break;
        default:
            return;
        }
    }
}

static inline bool is_at_end(void) {
    return *scanner.cur == '\0';
}

static inline char advance() {
    scanner.cur++;
    scanner.line_pos++;
    return scanner.cur[-1];
}

static inline char peek() {
    return *scanner.cur;
}

static inline char next() {
    if (is_at_end()) return '\0';
    return *(scanner.cur + 1);
}

static inline char next_next() {
    if (next() == '\0') return '\0';
    return *(scanner.cur + 2);
}

static inline char next_next_next() {
    if (next_next() == '\0') return '\0';
    return *(scanner.cur + 3);
}

static inline bool match(char m) {
    if (is_at_end()) return FALSE;
    // not match then advance once
    advance();
    if (peek() != m) return FALSE;
    // match then advance twice
    advance();
    return TRUE;
}

static inline bool is_digit(char ch) {
    return ch >= '0' && ch <= '9';
}

static inline bool is_alpha(char ch) {
    return (ch >= 'a' && ch <= 'z')
            || (ch >= 'A' && ch <= 'Z');
}

/**
 * realnumber <- digit+ exponent
 *            | digit+ fraction (exponent | nil)
 */
static inline T number(void) {
    assert(is_digit(peek()));
    
    // digit part
    while (is_digit(peek())) {
        advance();
    }

    // fraction part
    if (is_fraction()) {
        fraction();
    }

    // exponent part
    if (is_exponent()) {
        exponent();
    }

    return make_token(T_NUMBER);
}

static inline bool is_fraction() {
    return peek() == '.' && is_digit(next());
}

static inline bool is_exponent() {
    return (peek() == 'E' || peek() == 'e')
            && (is_digit(next())
                || (next() == '+' && is_digit(next_next()))
                || (next() == '-' && is_digit(next_next())));
}

static inline void fraction(void) {
    assert(peek() == '.');
    advance();
    while (is_digit(peek())) {
        advance();
    }
}

static inline void exponent(void) {
    assert(peek() == 'E' || peek() == 'e');
    advance();
    assert(peek() == '+' || peek() == '-');
    advance();
    while (is_digit(peek())) {
        advance();
    }
}

static inline T identifier(void) {
    assert(is_alpha(peek()));
    while (is_digit(peek()) || is_alpha(peek())) advance();
    return make_token(get_identifier_type());
}

static TT check_keyword(int, int, const char*, TT);

static TT get_identifier_type(void) {
    // TODO(ljr): add more keywords
    // (int, if), real, then, else, while

    switch (*scanner.start) {
        case 'r': return check_keyword(1, 3, "eal", T_REAL);
        case 't': return check_keyword(1, 3, "hen", T_THEN);
        case 'e': return check_keyword(1, 3, "lse", T_ELSE);
        case 'w': return check_keyword(1, 4, "hile", T_WHILE);
        case 'i': 
            if (scanner.start[1] == 'n') {
                return check_keyword(1, 2, "nt", T_INT);
            } else {
                return check_keyword(1, 1, "f", T_IF);
            }
        default: break;
    }

    return T_IDENTIFIER;
}

static TT check_keyword(int l, int r, const char *s, TT type) {
    if (r - l + 1 == scanner.cur - scanner.start - l
        && !strncmp(s, scanner.start + l, r - l + 1)) {
            return type;
    }
    return T_IDENTIFIER;
}

// ignore comment excluding last \n or eof
static inline char comment(void) {
    // current is the first '/'
    assert(peek() == '/');
    assert(next() == '/');
    // invariant: cur not \n and not end
    while (peek() != '\n' && !is_at_end()) {
        advance();
    }
    // cur is \n or end
    scanner.start = scanner.cur;
    if (peek() == '\n') {
        scanner.line_num++;
        scanner.line_pos = 0;
        return peek();
    }
    return peek();
}

static inline T make_token(TT token_type) {
    T t;
    t.start = scanner.start;
    t.length = scanner.cur - scanner.start;
    t.type = token_type;
    t.line_num = scanner.line_num;
    t.line_pos = scanner.line_pos;
    return t;
}

static inline T error_token() {
    Token t;
    // first proceed to next pos
    advance();
    t = make_token(T_ERROR);
    char tmp[100];
    sprintf(tmp, "Invalid token \"%.*s\", len: %d, ch: %d", t.length, t.start, t.length, *t.start);
    report_error(compile_output, LEX_ERROR, t.line_num, t.line_pos, tmp);
    return scan_token();
}
