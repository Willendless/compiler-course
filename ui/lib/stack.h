#ifndef _STACK_H_
#define _STACK_H_

typedef struct {
    int cnt;
    struct elem {
        void *en;
        struct elem *next;
    } head;
} Stack;

#define T Stack

T *Stack_init();
void *Stack_pop(T *);
void *Stack_top(T *);
int Stack_push(T *, void *);
int Stack_length(T *);

#undef T

#endif
