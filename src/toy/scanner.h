#ifndef _SCANNER_H_
#define _SCANNER_H_

#include "token.h"

void init_scanner(const char *source);
const Token scan_token();

#endif
