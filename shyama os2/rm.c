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

int rm(char *path, int i, int f) {
    if(i) {
        printf("rm: remove file '%s' (Y/n)? ", path);
        char *line = NULL;
        size_t line_length;
        ssize_t read;
        if(read = getline(&line, &line_length, stdin) != -1) {
            char a = line[0], b = line[1];
            if((a=='y' || a=='Y') && (b=='\r'||b=='\n')) {
                // pass
            } else {
                return 0;
            }
        }
    }
    if(remove(path) == -1) {
        if(!f) { fprintf(stderr, "rm: Cannot remove '%s'\n", path); }
    }
}

int main(int argc, char **argv) {
    Array *args = get_args(argc, argv);
    if(args->length == 0) {
        fprintf(stderr, "rm: missing operand");
        return 1;
    }

    int f_switch = is_switch_on(argc, argv, 'f');
    int i_switch = is_switch_on(argc, argv, 'i');
    for(int i=0; i<args->length; i++) {
        struct stat st = {0};
        char *data = (char*) ((char**) args->data)[i];
        rm(data, i_switch, f_switch);
    }

    free(args->data);
    free(args);
    return 0;
}
