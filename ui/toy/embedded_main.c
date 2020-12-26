#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "compiler.h"

#define MAX_LINE_LEN 1024

char line[MAX_LINE_LEN];

void handle_cmd(void);
void handle_file(char *path);
char *read_file(char *path);

//int main(int argc, char* argv[]) {
//    if (argc <= 1) {
//        handle_cmd();
//    } else if (argc == 2) {
//        handle_file(argv[1]);
//    } else {
//        perror("Usage: toy.out [path]");
//        exit(64);
//    }
//    return 0;
//}

void handle_cmd(void) {
    printf("Hello world! This is the interpreter implementation of the toy language used for debug.\n");
    while (1) {
        printf("> ");
        if (fgets(line, MAX_LINE_LEN, stdin) == NULL) {
            perror("Failed to get input line.");
            break;
        }
        if (!strncmp(line, "quit", 4)) {
            break;
        }
        compile(line);
        printf("Your input: %s", line);
    }
}

void handle_file(char *path) {
    assert(path != NULL);

    char *source = read_file(path);

    // TODO(ljr): complete vm part
    compile(source);
    
    free(source);

}

char *read_file(char *path) {
    size_t file_size;
    FILE* file;
    char* buffer;
    size_t read_count;

    file = fopen(path, "rb");
    if (file == NULL) {
        perror("Failed to open file");
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    buffer = (char *)malloc(file_size + 1);
    if (buffer == NULL) {
        perror("Failed to allocate memory for reading file at ");
        exit(74);
    }

    read_count = fread(buffer, sizeof(char), file_size, file);
    if (read_count < file_size) {
        perror("Could not read file at path ");
        exit(74);
    }
    buffer[file_size] = '\0';

    return buffer;
}
