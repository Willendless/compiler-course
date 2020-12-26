#ifndef __debug_h__
#define __debug_h__

#include <stdio.h>
#include <errno.h>
#include <string.h>

// DEFAULT DEBUG
#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d (%s): " M "\n", \
                              __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#endif

#define clean_errno() (errno == 0? "NONE" : strerror(errno))

// ERROR LOG
#define log_err(M, ...) fprintf(stderr, \
                                "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, \
                                clean_errno(), ##__VA_ARGS__)

// WARNING LOG
#define log_warn(M, ...) fprintf(stderr, \
                                "[WARN] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, \
                                clean_errno(), ##__VA_ARGS__)

// INFO LOG
#define log_info(M, ...) fprintf(stderr, \
                                "[INFO] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, \
                                clean_errno(), ##__VA_ARGS__)

// ASSERT TRUE
#define check(A, M, ...) if(!(A)) {\
                                   log_warn(M, ##__VA_ARGS__);\
                                   errno=0; goto error;}

// ASSERT TRUE IN DEBUG MODE
#define check_debug(A, M, ...) if (!(A)) { debug(M, ##__VA_ARGS__);\
                                           errno=0; goto error; }

// UNREACHED CODE
#define sentinel(M, ...) { log_err(M, ##__VA_ARGS__);\
                           errno=0; goto error;}

// CHECK NULL PTR
#define check_mem(A) check((A), "Out of memory")

#endif
