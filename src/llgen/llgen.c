#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

static void output_table(void);
static void output_productions(void);

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
// output table
int **table;
int x_end;
int y_end;

void init(void) {
    // index -> grammar
    grammar_arr = DArray_init(70, 100);
    // grammar -> index
    grammar_table = Table_init(100, cmp_str, hash_str);
    production_arr = DArray_init(70, 100);
    first = DArray_init(sizeof(Set), 70);
    follow = DArray_init(sizeof(Set), 100);
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
    table = (int **)calloc(1, sizeof(non_index-1)*sizeof(grammar_arr->end-non_index+1));
    x_end = grammar_arr->end - non_index + 1;
    y_end = non_index;
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
    // first set for terminal
    for (i = 1; i < non_index; ++i) {
        // index -> index
        DArray_set(first, i, Set_init(5, cmp_int, hash_int)); 
        Set_put(DArray_get(first, i), (int *)i);
    }
    // first set for nonterminal
    for (i = non_index; i <= grammar_arr->end; ++i) {
        // index -> index
        DArray_set(first, i, Set_init(5, cmp_int, hash_int)); 
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
                Set *product = DArray_get(first, (int)DArray_get(production, 0));
                // if s_head doesn't include epsilon, we cannot add it here
                union_without_epsilon(s_head, product);
                // for other production union if epsilon exists in previous set
                for (k = 1; k <= production->end && Set_member(product, Table_get(grammar_table, "epsilon")); ++k) {
                    product = DArray_get(first, DArray_get(production, k));
                    union_without_epsilon(s_head, product);
                }
                if (k > production->end && Set_member(product, Table_get(grammar_table, "epsilon"))) {
                    Set_put(s_head, Table_get(grammar_table, "epsilon"));
                }
            }
            // check if first sets changed
            cur_cnt = Set_length(s_head);
            if (prev_cnt != cur_cnt) {
                // first sets changed, loop continue
                printf("first set: %s %d %d\n", DArray_get(grammar_arr, i), prev_cnt, cur_cnt);
                // Set_print(s_head);
                flag |= TRUE;
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
    int i, j, k;
    bool flag = TRUE;
    int prev_cnt;
    int cur_cnt;
    Set *s_head;
    Set *record = Set_init(100, cmp_int, hash_int);
    printf("non_index: %d, end: %d\n", non_index, grammar_arr->end);
    // init follow set
    for (i = non_index; i <= grammar_arr->end; ++i) {
        Set *tmp = Set_init(5, cmp_int, hash_int);
        check(tmp != NULL, "failed to init set");
        DArray_set(follow, i, tmp);
        check(DArray_get(follow, i) != NULL, "Failed to set follow arr.");
    }
    // put eof to follow(start)
    s_head = DArray_get(follow, non_index);
    check(s_head != NULL, "Failed to get head follow set.");
    Set_put(DArray_get(follow, non_index), Table_get(grammar_table, "eof"));
    //fixed-point
    while (flag) {
        // for each production head
        for (i = non_index; i <= production_arr->end; ++i) {
            DArray *productions = DArray_get(production_arr, i);
            // for each production with the same head
            for (j = 0; j <= productions->end; ++j) {
                // init record set to follow[head]
                Set_clear(record);
                check(Set_length(record) == 0, "Failed to clear set record.");
                Set_union(record, DArray_get(follow, i));
                // get cur production
                DArray *production = DArray_get(productions, j);
                // for each grammar
                for (k = production->end; k >= 0; --k) {
                    // index of grammar in product[k]
                    int index = DArray_get(production, k);
                    Set *cur = DArray_get(follow, index);
                    // record prev_cnt to check if cur changed
                    prev_cnt = Set_length(cur);
                    Set_union(cur, record);
                    // product[k] can be epsilon, then follow[product[k]] can be in record
                    // otherwise, record can only contains first[product[k]]
                    if (Set_member(DArray_get(first, index), "epsilon")) {
                        Set_union(record, DArray_get(first, index));
                        Set_remove(record, "epsilon");
                    } else {
                        Set_clear(record);
                        check(Set_length(record) == 0, "Failed to clear set record.");
                        Set_union(record, DArray_get(first, index));
                    }
                    cur_cnt = Set_length(cur);
                    check(cur_cnt >= prev_cnt, "Follow set elements removed.");
                    if (cur_cnt > prev_cnt) {
                        flag = TRUE;
                    }
                }
            }
        }
    }
    return;
error:
    exit(80);
}

static inline void handle_input(void) {
    fscanf(in, "%s", buf);
    check(!strcmp(buf, "[terminal]"), "Failed to read [terminal]");
    handle_grammar();
    check(!strcmp(buf, "[nonterminal]"), "Failed to read [nonterminal]");
    handle_grammar();
    check(!strcmp(buf, "[production]"), "Failed to read [production], %s instead", buf);
    handle_production();
    return;
error:
    fprintf(stderr, "failed to handle input\n");
    exit(80);
}

static inline void handle_grammar() {
    while (fscanf(in, "%s", buf) != EOF) {
        if (buf[0] == '[') {
            if (!non_index) {
                // finish processing terminal part
                non_index = grammar_arr->end + 1;
            }
            break;
        }
        // push into arr
        char *g = DArray_new(grammar_arr);
        strcpy(g, buf);
        DArray_push(grammar_arr, g);
        // put into table
        Table_put(grammar_table, g, grammar_arr->end);
    }
    check(Table_get(grammar_table, "epsilon") != NULL, "failed to add epsilon grammar");
    check(Table_get(grammar_table, "eof") != NULL, "failed to add eof grammar");
    return;
error:
    exit(80);
}

static inline void handle_production() {
    int index;
    DArray *productions;
    DArray *production;

    while (fscanf(in, "%s", buf) != EOF) {
        // get head's index
        index = Table_get(grammar_table, buf);
        check(index != NULL, "Unknown nonterminal: %s.", buf);
        // read "->"
        fscanf(in, "%s", buf);
        check(strcmp(buf, "->") == 0, "Invalid production format.");
        productions = DArray_init(sizeof(DArray), 5);
        DArray_set(production_arr, index, productions);
        // handle production body
        while (strcmp(buf, "$")) {
            // init production
            production = DArray_init(sizeof(int), 10);
            DArray_push(productions, production);
            // read each grammar
            fscanf(in, "%s", buf);
            while (strcmp(buf, "|") && strcmp(buf, "$")) {
                index = Table_get(grammar_table, buf);
                check(index != NULL, "Unknown grammar: %s.", buf);
                DArray_push(production, (void *)index);
                fscanf(in, "%s", buf);
            }
        }
    }

error:
    return;
}

static void handle_output(void) {
    printf("output table\n");
    fprintf(out, "#include <stdio.h>\n");
    fprintf(out, "#include \"parser.h\"\n");
    output_table();
    output_productions();
}

static void output_table() {
    int i, j;
    // output ll parser table
    fprintf(out, "const int[][] LL_PARSE_TABLE = [\n");

    // for each nonterminal
    for (i = 0; i < x_end; i++) {
        fprintf(out, "    [_");
        // for each terminal
        for (j = 1; j < y_end; ++j) {
            char s[10];
            sprintf(s, "%d", table[i][j]);
            fprintf(out, ", %s", s);
        }
        fprintf(out, "]\n");
    }

    fprintf(out, "];\n");
}

static void output_productions() {
    fprintf(out, "\n");
    fprintf(out, "\n");

    fprintf(out, "const int** productions");
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
    return a == b;
}

static unsigned hash_int(const void *key) {
    return (unsigned long)key >> 2;
}
