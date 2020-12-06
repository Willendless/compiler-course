#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include "llgen.h"
#include "../utils/debug.h"

const char *usage(void);

char in_name[1024] = "ll-spec.txt";
char out_name[1024] = "compiler.c";

int main(int argc, char *argv[]) {
    int ch;
    FILE *in;
    FILE *out;

    printf("%s\n", usage());

    check(argc >= 2 && argc <= 3, "wrong number of command line argument.");
    while ((ch = getopt(argc, argv, "i:o:")) != -1) {
        switch (ch) {
        case 'i': printf("%s\n", optarg); strcpy(in_name, optarg); break;
        case 'o': strcpy(out_name, optarg); break;
        default:
            printf("%c:%s???\n", ch, optarg);
            check(1==0, "Invalid command line argument.");
        }
    }

    in = fopen(in_name, "r");
    out = fopen(out_name, "rw+");

    run(in, out);
    return 0;
error:
    return -1;
}

const char * usage(void) {
    return "Usage: llgen -i [input file path] [-o[output file path]]";
}
