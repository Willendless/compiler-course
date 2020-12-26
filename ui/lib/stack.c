#include "stack.h"
#include <stdlib.h>
#include "../utils/debug.h"

#define T Stack

T *Stack_init() {
    T *s;
    s = (T *)calloc(1, sizeof(T));
    check_mem(s);
    return s;
error:
    return NULL;
}

void *Stack_pop(T *s) {
    struct elem *top;
    void *ret;
    check(s->cnt>0, "Out of stack");
    // get top value of stack
    top = s->head.next;
    ret = top->en;
    // reset top of stack
    s->head.next = top->next;
    // free previous top
    free(top);
    s->cnt--;
    return ret;
error:
    return NULL;
}

void *Stack_top(T *s) {
    check(s->cnt>0, "Out of stack");
    return s->head.next->en;
error:
    return NULL;
}

int Stack_push(T *s, void *v) {
    struct elem *en;
    en = (struct elem *)calloc(1, sizeof(struct elem));
    check_mem(en);
    // init new entry
    en->en = v;
    en->next = s->head.next;
    // reset top of stack
    s->head.next = en;
    s->cnt++;
    return 0;
error:
    return -1;
}

int Stack_length(T *s) {
    return s->cnt;
}
