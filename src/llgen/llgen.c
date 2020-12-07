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
static void gen_firstplus_sets(void);

static Set *get_grammar_set(DArray *arr, const char *grammar);
static void union_without_epsilon(Set *dest, Set *src);

static bool cmp_str(const void *a, const void *b);
static unsigned hash_str(const void *key);
static bool cmp_int(const void *a, const void *b);
static unsigned hash_int(const void *key);

static void print_firstfollow_set(DArray *arr);
static void print_firstplus_set(void);

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
DArray *firstplus;
// buf
char buf[1024];
// output table
int **table;
int x_end;
int y_end;
// output productions
int max_product;

void init(void) {
    // index -> grammar
    grammar_arr = DArray_init(70, 100);
    // grammar -> index
    grammar_table = Table_init(100, cmp_str, hash_str);
    production_arr = DArray_init(70, 100);
    first = DArray_init(sizeof(Set), 70);
    follow = DArray_init(sizeof(Set), 100);
    firstplus = DArray_init(sizeof(Set), 100);
    char *dum = DArray_new(grammar_arr);
    strcpy(dum, "dummy entry");
    DArray_push(grammar_arr, dum);
}

void release(void) {
    int i;
    DArray_clear_destroy(grammar_arr);
    Table_clear_destroy(grammar_table);
    // production_arr
    for (i = 0; i <= production_arr->end; ++i) {
        DArray_clear_destroy(DArray_get(production_arr, i));
    }
    Set_clear_destroy(first);
    Set_clear_destroy(follow);
    Set_clear_destroy(firstplus);
}

void run(FILE *_in, FILE *_out) {
    in = _in;
    out = _out;
    init(); // initialize some data structures
    handle_input(); // parse input file
    gen_table(); // table generation
    handle_output(); // generate output file
    release(); // release memory
}

void gen_table(void) {
    int i, j, k, i_firstplus, i_production;
    int epsilon_index = Table_get(grammar_table, "epsilon");
    gen_first_sets();
    gen_follow_sets();
    gen_firstplus_sets();
    // TODO(ljr): table generatation algo
    x_end = grammar_arr->end - non_index;
    y_end = non_index - 1;
    table = (int **)calloc(x_end + 1, sizeof(int *));
    for (i = 0; i <= x_end; ++i) {
        table[i] = (int *)calloc(y_end + 1, sizeof(int));
    }

    // for each nonterminal
    i_firstplus = 0; // traverse production index from 0
    i_production = 1; // output production index begin from 1
    for (i = non_index; i <= grammar_arr->end; ++i) {
        printf("%d %s: ", i, DArray_get(grammar_arr, i));
        int end = ((DArray *)DArray_get(production_arr, i))->end;
        // for each production with head = current nonterminal
        for (j = 0; j <= end; ++j) {
            // for each terminal
            for (k = 1; k < non_index; ++k) {
                if (Set_member(DArray_get(firstplus, i_firstplus), k)) {
                    printf("   %-5s", DArray_get(grammar_arr, k));
                    table[i - non_index][k] = i_production;
                }
            }
            i_firstplus++;
            i_production++;
        }
        printf("\n");
    }
}

static void gen_firstplus_sets(void) {
    int i, j, k;
    int epsilon_index = Table_get(grammar_table, "epsilon");
    // for each production head
    for (i = non_index; i <= production_arr->end; ++i) {
        DArray *productions = DArray_get(production_arr, i);
        // for each production
        for (j = 0; j <= productions->end; ++j) {
            // calc first+ set for head -> body
            // = first(head->body) if epsilon not in first(head->body) 
            // = first(head->body) + follow(head) if epsilon in first(head->body)
            DArray *production = DArray_get(productions, j);
            Set *fp = Set_init(10, cmp_int, hash_int);
            check(fp != NULL, "Failed to init set");
            bool flag = TRUE;
            DArray_push(firstplus, fp);
            // for each product
            for (k = 0; k <= production->end; ++k) {
                Set *tmp = DArray_get(first, DArray_get(production, k));
                Set_union(fp, tmp);
                Set_remove(fp, epsilon_index);
                if (!Set_member(tmp, epsilon_index)) {
                    flag = FALSE;
                    break;
                }
            }
            // epsilon is in first[product[i]], i from 0 to production->end
            if (flag) {
                Set_put(fp, epsilon_index);
                Set_union(fp, DArray_get(follow, i));
            }
        }
    }
    printf("\nfirstplus:\n");
    print_firstplus_set();
    printf("\n");
error:
    return;
}

static void gen_first_sets(void) {
    bool flag = TRUE;
    int prev_cnt, cur_cnt;
    int i, j, k;
    int epsilon_index = Table_get(grammar_table, "epsilon");
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
                for (k = 1; k <= production->end && Set_member(product, epsilon_index); ++k) {
                    product = DArray_get(first, DArray_get(production, k));
                    union_without_epsilon(s_head, product);
                }
                if (k > production->end && Set_member(product, epsilon_index)) {
                    Set_put(s_head, Table_get(grammar_table, "epsilon"));
                }
            }
            // check if first sets changed
            cur_cnt = Set_length(s_head);
            if (prev_cnt != cur_cnt) {
                // first sets changed, loop continue
                // printf("first set: %s %d %d\n", DArray_get(grammar_arr, i), prev_cnt, cur_cnt);
                // Set_print(s_head);
                flag |= TRUE;
            }
        }
    }
    printf("first sets:\n");
    print_firstfollow_set(first);
    printf("\n");
}

static void union_without_epsilon(Set *dest, Set *src) {
    bool have_epsilon;
    int epsilon_index = Table_get(grammar_table, "epsilon");
    assert(dest != NULL);
    assert(src != NULL);
    have_epsilon = Set_member(dest, epsilon_index);
    if (have_epsilon) {
        Set_union(dest, src);
    } else {
        Set_union(dest, src);
        Set_remove(dest, epsilon_index);
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
    int epsilon_index = Table_get(grammar_table, "epsilon");
    printf("non_index: %d, end: %d\n", non_index, grammar_arr->end);
    // init follow set
    for (i = 1; i <= grammar_arr->end; ++i) {
        // empty set
        DArray_set(follow, i, Set_init(5, cmp_int, hash_int));
        check(DArray_get(follow, i) != NULL, "Failed to set follow arr.");
    }
    // put eof to follow(start)
    s_head = DArray_get(follow, non_index);
    check(s_head != NULL, "Failed to get head follow set.");
    Set_put(s_head, Table_get(grammar_table, "eof"));
    //fixed-point
    while (flag) {
        flag = FALSE;
        // for each production head
        for (i = non_index; i <= production_arr->end; ++i) {
            DArray *productions = DArray_get(production_arr, i);
            // for each production with the same head
            for (j = 0; j <= productions->end; ++j) {
                // init record set to follow[head]
                Set_clear(record);
                check(Set_length(record) == 0, "Failed to clear set record, len: %d.", Set_length(record));
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
                    if (Set_member(DArray_get(first, index), epsilon_index)) {
                        Set_union(record, DArray_get(first, index));
                        Set_remove(record, epsilon_index);
                    } else {
                        Set_clear(record);
                        check(Set_length(record) == 0, "Failed to clear set record.");
                        Set_union(record, DArray_get(first, index));
                    }
                    cur_cnt = Set_length(cur);
                    check(cur_cnt >= prev_cnt, "Follow set elements removed.");
                    if (cur_cnt > prev_cnt) {
                        // printf("follow set: %s %d -> %d\n", DArray_get(grammar_arr, k), cur_cnt, prev_cnt);
                        flag = TRUE;
                    }
                }
            }
        }
    }
    printf("\nfollow sets:\n");
    print_firstfollow_set(follow);
    printf("\n");
    return;
error:
    exit(80);
}

static inline void print_firstplus_set() {
    int i, j, k;
    int cnt;

    j = 0;
    cnt = 0;
    for (i = 0; i <= firstplus->end; ++i) {
        Set *s = DArray_get(firstplus, i);
        if (cnt > ((DArray *)DArray_get(production_arr, non_index+j))->end) {
            cnt = 0;
            j++;
        }
        printf("%s: ", DArray_get(grammar_arr, non_index+j));
        for (k = 0; k < s->bucket_size; ++k) {
            struct set_entry *p = s->buckets[k];
            if (p) {
                for (; p; p = p->next) {
                    printf("   %-5s", DArray_get(grammar_arr, p->member));
                }
            }
        }
        printf("\n");
        cnt++;
    }
}

static inline void print_firstfollow_set(DArray *arr) {
    int i, j;
    assert(arr != NULL);

    for (i = non_index; i <= grammar_arr->end; ++i) {
        Set *s = DArray_get(arr, i);
        printf("%s:", DArray_get(grammar_arr, i));
        for (j = 0; j < s->bucket_size; ++j) {
            struct set_entry *p = s->buckets[j];
            if (p) {
                for (; p; p = p->next) {
                    printf("   %-5s", DArray_get(grammar_arr, p->member));
                }
            }
        }
        printf("\n");
    }
}

static inline void handle_input(void) {
    fscanf(in, "%s", buf);
    check(!strcmp(buf, "[terminal]"), "Failed to read [terminal]");
    handle_grammar();
    check(!strcmp(buf, "[nonterminal]"), "Failed to read [nonterminal]");
    handle_grammar();
    check(!strcmp(buf, "[production]"), "Failed to read [production], %s instead", buf);
    handle_production();
    fclose(in);
    return;
error:
    fclose(in);
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
    int product_cnt;
    DArray *productions;
    DArray *production;

    product_cnt = 0;
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
                product_cnt++;
                index = Table_get(grammar_table, buf);
                check(index != NULL, "Unknown grammar: %s.", buf);
                DArray_push(production, (void *)index);
                fscanf(in, "%s", buf);
            }
            if (product_cnt > max_product) max_product = product_cnt;
            product_cnt = 0;
        }
    }

error:
    return;
}

static void handle_output(void) {
    printf("output table\n");
    fprintf(out, "#include <stdio.h>\n");
    fprintf(out, "#include \"parser.h\"\n\n");
    output_table();
    output_productions();
    fclose(out);
}

static void output_table() {
    int i, j;
    // output ll parser table
    fprintf(out, "// ll(1) parser table\n");
    fprintf(out, "const int LL_PARSE_TABLE[%d][%d] = {\n", x_end + 1, y_end + 1);

    // for each nonterminal
    for (i = 0; i <= x_end; i++) {
        fprintf(out, "    {0");
        // for each terminal
        for (j = 1; j <= y_end; ++j) {
            fprintf(out, ", %d", table[i][j]);
        }
        fprintf(out, "}%s\n", i == x_end? "" : ",");
    }

    fprintf(out, "};\n");
}

static void output_productions() {
    int i, j, k, m;
    int i_production;
    fprintf(out, "// begin index of nonterminal\n");
    fprintf(out, "const int non_index = %d;\n", non_index);
    fprintf(out, "// production table\n");
    fprintf(out, "const int productions[%d][%d] = {\n", production_arr->end - non_index + 1, max_product);
    // for productions with same head
    i_production = 0;
    for (i = non_index; i <= production_arr->end; ++i) {
        DArray *productions = DArray_get(production_arr, i);
        // for each production
        for (j = 0; j <= productions->end; ++j) {
            fprintf(out, "    ");
            for (m = 0; m < 5 && j <= productions->end; ++m, ++j) {
                fprintf(out, "{");
                DArray *production = DArray_get(productions, j);
                // for each product
                for (k = 0; k <= production->end; ++k) {
                    fprintf(out, "%s%d", k == 0? "" : ", ", DArray_get(production, k));
                }
                fprintf(out, "},%s", m == 2? "" : " ");
            }
            fprintf(out, "\n");
        }
    }

    fprintf(out, "};\n");
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
