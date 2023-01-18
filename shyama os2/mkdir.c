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

int main(int argc, char **argv) {
    Array *args = get_args(argc, argv);
    char *path = get_current_dir_name();

    if(args->length == 0) {
        fprintf(stderr, "mkdir: missing operand");
        return 1;
    }

    int v_switch = is_switch_on(argc, argv, 'v');
    int p_switch = is_switch_on(argc, argv, 'p');
    for(int i=0; i<args->length; i++) {
        struct stat st = {0};
        char *data = (char*) ((char**) args->data)[i];
        if (stat(data, &st) == -1) {
            int x = mkdir(data, 0700);
            if(x == -1) {
                fprintf(stderr, "mkdir: cannot create directory '%s'\n", data);
            } else {
                if(v_switch) {
                    printf("mkdir: created directory '%s'\n", data);
                }
            }
        }
    }

    free(args->data);
    free(args);
    free(path);
    return 0;
}
