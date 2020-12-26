#ifndef _COMPILER_H_
#define _COMPILER_H_

#include <stdio.h>

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

static inline void report_error(char *buffer_output, COMPILE_ERROR_TYPE type, int line, int pos, const char *msg) {
    sprintf(buffer_output, "%sError type: %s at (Line %d:, Pos: %d): %s.\n",
                buffer_output,
                compile_error_type[type],
                line, pos, msg);
}

extern char compile_output[2048];

#endif
