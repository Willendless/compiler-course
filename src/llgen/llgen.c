#include <stdio.h>
#include <string.h>
#include "../lib/set.h"
#include "../lib/table.h"
#include "../lib/darray.h"

static void handle_input(void);
static void handle_grammar(void);
static void handle_production(void);
static void handle_output(void);
static void gen_first_sets(void);
static void gen_follow_sets(void);
static void gen_table(void);

static Set *get_grammar_set(DArray *arr, const char *grammar);
static void union_without_epsilon(Set *dest, Set *src);

static bool cmp_str(const void *a, const void *b);
static unsigned hash_str(const void *key);
static bool cmp_int(const void *a, const void *b);
static unsigned hash_int(const void *key);

// in, out
FILE* in;
FILE* out;
// grammar representation
DArray *grammar_arr;
Table *grammar_table;
int non_index;
// production representation
DArray *production_arr;
// first and follow set
DArray *first;
DArray *follow;
// buf
char buf[1024];
static const char epsilon[] = "epsilon";
// output table
int **table;

void init(void) {
    grammar_arr = DArray_init(70, 100);
    grammar_table = Table_init(100, cmp_str, hash_str);
    production_arr = DArray_init(70, 100);
    first = DArray_init(sizeof(Set), 70);
    follow = DArray_init(sizeof(Set), 70);
    DArray_push(grammar_arr, "dummy entry");
}

void run(FILE *_in, FILE *_out) {
    in = _in;
    out = _out;
    init(); // initialize some data structures
    handle_input(); // parse input file
    gen_table(); // table generation
    handle_output(); // generate output file
}

void gen_table(void) {
    int i, j;
    gen_first_sets();
    gen_follow_sets();
    // TODO(ljr): table generatation algo
    table = (int **)malloc(sizeof(non_index-1)*sizeof(grammar_arr->end-non_index+1));
    // for each nonterminal
    for (i = non_index; i <= grammar_arr->end; ++i) {
        char *grammar = DArray_get(grammar_arr, i);
        Set *fir = get_grammar_set(first, grammar);
        Set *fol = get_grammar_set(follow, grammar);
        // for each terminal
        for (j = 1; j < non_index; ++j) {
            char *terminal = DArray_get(grammar_arr, j);
            if (Set_member(fir, terminal)
                || Set_member(fol, terminal)) {
                    table[i][j] = i;
            }
        }
    }
}

static void gen_first_sets(void) {
    bool flag = TRUE;
    int prev_cnt, cur_cnt;
    int i, j, k;
    Set *s_first;
    Set *cur;
    // first set for terminal
    for (i = 1; i < non_index; ++i) {
        DArray_set(first, i, Set_init(5, cmp_str, hash_str)); 
        Set_put(DArray_get(first, i), i);
    }
    // first set for nonterminal
    for (i = non_index; i <= grammar_arr->end; ++i) {
        DArray_set(first, i, Set_init(5, cmp_str, hash_str)); 
    }
    // fixed-point
    while (flag) {
        flag = FALSE;
        // for each production head
        for (i = non_index; i <= production_arr->end; ++i) {
            // first set for head
            Set *s_head = DArray_get(first, i);
            DArray *productions = DArray_get(production_arr, i);
            prev_cnt = Set_length(s_head);
            // for each production on one head
            for (j = 0; j <= productions->end; ++j) {
                DArray *production = DArray_get(productions, j);
                char *product0 = DArray_get(production, 0);
                Set *pre = get_grammar_set(first, product0);
                // if s_head doesn't include epsilon, we cannot add it here
                union_without_epsilon(s_head, pre);
                // for other production union if epsilon exists in previous set
                for (k = 1; k <= production->end && Set_member(pre, "epsilon"); ++k) {
                    char *productk = DArray_get(production, k);
                    cur = get_grammar_set(first, productk); 
                    union_without_epsilon(s_head, cur);
                }
                if (k > production->end && Set_member(cur, "epsilon")) {
                    Set_put(s_head, "epsilon");
                }
                // check if first sets changed
                cur_cnt = Set_length(s_head);
                if (prev_cnt != cur_cnt) {
                    // first sets changed, loop continue
                    flag |= TRUE;
                }
            }
        }
    }
}

static void union_without_epsilon(Set *dest, Set *src) {
    bool have_epsilon;
    assert(dest != NULL);
    assert(src != NULL);
    have_epsilon = Set_member(dest, "epsilon");
    if (have_epsilon) {
        Set_union(dest, src);
    } else {
        Set_union(dest, src);
        Set_remove(dest, "epsilon");

    }
}

static Set *get_grammar_set(DArray *arr, const char *grammar) {
    Set *ret;
    int index;
    assert(arr != NULL);
    assert(grammar != NULL);
    index = Table_get(grammar_table, grammar);
    check(index >= 1, "Failed to get grammar index.");
    ret = DArray_get(arr, index);
    check(ret != NULL, "Failed to get set.");
    return ret;
error:
    return NULL;
}

static void gen_follow_sets(void) {

}

static inline void handle_input(void) {
    fscanf(in, "%s", buf);
    check(!strcmp(buf, "[terminal]"), "Failed to read [terminal]");
    handle_grammar();
    check(!strcmp(buf, "[nonterminal]"), "Failed to read [nonterminal]");
    handle_grammar();
    check(!strcmp(buf, "[production]"), "Failed to read [production]");
    handle_production();
error:
    exit(80);
}

static inline void handle_grammar() {
    while (fscanf(in, "%s", buf) != EOF) {
        if (buf[0] == '[') {
            // finish processing terminal part
            non_index = grammar_arr->end + 1;
            break;
        }
        // push into arr
        char *g = DArray_new(grammar_arr);
        strcpy(g, buf);
        DArray_push(grammar_arr, g);
        // put into table
        Table_put(grammar_table, g, grammar_arr->end);
    }
}

static inline void handle_production() {
    int index;
    DArray *productions;
    DArray *production;
    char *tmp;

    while (fscanf(in, "%s", buf) != EOF) {
        // read production head which must be nonterminal
        fscanf(in, "%s", buf);
        // get head's index
        index = Table_get(grammar_table, buf);
        check(index != NULL, "Unknown to get nonterminal");
        check(strcmp(buf, "->") == 0, "Invalid production format.");
        productions = DArray_init(sizeof(DArray), 5);
        DArray_set(production_arr, index, productions);
        // handle production body
        while (strcmp(buf, "$")) {
            // init production
            production = DArray_new(productions);
            DArray_push(productions, production);
            // read each grammar
            fscanf(in, "%s", buf);
            while (strcmp(buf, "!") && strcmp(buf, "$")) {
                tmp = DArray_new(production);
                strcpy(tmp, buf);
                DArray_push(production, tmp);
                fscanf(in, "%s", buf);
            }
        }
    }

error:
    return;
}

static void handle_output(void) {

}

static bool cmp_str(const void *a, const void *b) {
    return !strcmp(a, b) ? TRUE : FALSE;
}

static unsigned hash_str(const void *key) {
    unsigned hash = 2166136261u;
    char *p = key;

    for (; *p != '\0'; p++) {
        hash ^= *p;
        hash *= 16777619;
    }
    return hash;
}

static bool cmp_int(const void *a, const void *b) {
    return a != b;
}

static unsigned hash_init(const void *key) {
    return (unsigned long)key >> 2;
}
