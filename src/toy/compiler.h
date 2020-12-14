#ifndef _COMPILER_H_
#define _COMPILER_H_

void compile(const char *source);

typedef enum {
    LEX_ERROR,
    SYNTAX_ERROR,
    SEMANTIC_ERROR
} COMPILE_ERROR_TYPE;

static const char *compile_error_type[] = {
    "LEX_ERROR",
    "SYNTAX_ERROR",
    "SEMANTIC_ERROR"
};

#define report_msg(M, ...) fprintf(stderr, \
                                         M "\n", ##__VA_ARGS__)

static inline void report_error(COMPILE_ERROR_TYPE type, int line, int pos, const char *msg) {
    fprintf(stderr, "Error type: %s at (Line %d:, Pos: %d): %s.\n",
                compile_error_type[type],
                line, pos, msg);
}

#endif
