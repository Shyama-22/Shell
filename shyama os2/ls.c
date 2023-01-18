#define _GNU_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
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
    
    DIR *d = opendir(".");
    if(!d) { fprintf(stderr, "Error listing directory in path '%s'\n", path); }

    struct dirent *dir;
    int a_switch = is_switch_on(argc, argv, 'a');
    int A_switch = is_switch_on(argc, argv, 'A');
    while ((dir = readdir(d)) != NULL) {
        if(dir->d_name[0] == '.' && !a_switch) { continue; }
        
        if(A_switch  && (strcmp(dir->d_name, ".")==0 || strcmp(dir->d_name, "..")==0)) {
            continue;
        }
        
        printf("%s\n", dir->d_name);
    }
    closedir(d);

    free(path);
    free(args->data);
    free(args);

    return 0;
}
