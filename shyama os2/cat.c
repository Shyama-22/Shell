#define _GNU_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct {
    void *data;
    int length;
} Array;

int is_switch_on(int argc, char **argv, char c) {
    char *arg;
    for(int i=1; i<argc; i++) {
        arg = argv[i];
        if(arg[0] == '-') {
            int j=0;
            while(arg[j]) {
                if(arg[j]==c) { return 1; }
                j++;
            }
        }
    }
    return 0;
}

Array *get_args(int argc, char **argv) {
    char *arg;
    int count = 0;
    for(int i=1; i<argc; i++) {
        if(argv[i][0] != '-') { count++; }
    }
    char **all_args = malloc(sizeof(char*) * count);
    int j = 0;
    for(int i=1; i<argc; i++) {
        if(argv[i][0] != '-') { all_args[j++] = argv[i]; }
    }
    Array *array = malloc(sizeof(Array));
    array->data = (void*) all_args;
    array->length = count;
    return array;
}


int cat(FILE *ptr, int n, int b, int *line_no) {
    char *line = NULL;
    size_t line_length;
    ssize_t read;

    while(read = getline(&line, &line_length, ptr) != -1) {
        if((line[0] == '\n' || line[0] == '\r') && b) { continue; }
        if(n) {
            printf("%4d    %s", *line_no, line);
        } else {
            printf("%s", line);
        }
        *line_no = *line_no + 1;
    }
    free(line);
}

int main(int argc, char **argv) {
    Array *args = get_args(argc, argv);
    char *path = get_current_dir_name();

    int n_switch = is_switch_on(argc, argv, 'n');
    int b_switch = is_switch_on(argc, argv, 'b');
    int line_no = 1;
    if(args->length == 0) {
        cat(stdin, n_switch, b_switch, &line_no);
    }

    for(int i=0; i<args->length; i++) {
        char *data = (char*) ((char**) args->data)[i];
        printf("Trying '%s'\n", data);
        FILE *f = fopen(data, "r");
        cat(f, n_switch, b_switch, &line_no);
        
        fclose(f);
    }

    free(args->data);
    free(args);
    free(path);

    return 0;
}
