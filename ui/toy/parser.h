#ifndef _PARSER_H_
#define _PARSER_H_

#include "ast.h"

extern const char *NONTERMINAL_NAME[];
AstNode* parse(const char *);

#endif
