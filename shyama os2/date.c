#define _GNU_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>


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
    int u_switch = is_switch_on(argc, argv, 'u');
    int R_switch = is_switch_on(argc, argv, 'R');

    if(R_switch) {
        struct tm *local;
        time_t t;

        t = time(NULL);
        local = gmtime(&t);
        printf("%s", asctime(local));
    } else if(u_switch) {
        struct tm *local;
        time_t t;

        t = time(NULL);
        local = gmtime(&t);
        printf("%s", asctime(local));
    }

    return 0;
}
