#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/limits.h>
#include <pthread.h>
#if SC_PLATFORM == SC_PLATFORM_LINUX
#include <errno.h>
#endif


typedef struct {
    char* data;
    int length;
} String;


void *pthread_system(void *vargp)
{
    system((char*) vargp);
}

char **tokenize(char *s, int length, char ch) {
    char **argv = malloc(sizeof(char**) * 101);
    int delimmed = 1, k=0;
    for(int i=0; i<length && k < 100; i++) {
        if(s[i] == ch) { s[i] = '\0'; delimmed=1; continue; }
        if(delimmed) {
            delimmed = 0;
            argv[k++] = &s[i];
        }
    }
    argv[k] = NULL;
    return argv;
}

int process_command(char **path_ptr, char *command, int command_length) {
    int use_pthread = 0;
    if(command[command_length-1] == 't' && command[command_length-2] == '&') {
        // use pthread
        char *s = command;
        int i = command_length - 1;
        s[i--] = '\0'; s[i--] = '\0';
        while((s[i]=='\n' || s[i]=='\r' || s[i]==' ' || s[i]=='\t' || s[i]=='\f') && i>=0) { s[i--]='\0'; }
        command_length = i + 1;
        use_pthread = 1;
    }
    char *path = *path_ptr, *saveptr;
    // printf("command: %s\n", command);
    char **argv = tokenize(command, command_length, ' ');
    char *first_token = argv[0];
    // printf("1: '%s'\n", first_token);
    if(first_token == NULL) { return 0; }

    if(strcmp(first_token, "cd") == 0) {
        char *second_token = argv[1];
        if(second_token == NULL) {
            printf("cd: missing operand!\n"); return 1;
        }
        if(argv[2] != NULL) {
            printf("cd: too many arguments\n");
            return 1;
        }
        char *new_path = malloc(PATH_MAX);
        if(realpath(second_token, new_path) == NULL) {
            printf("cd: Error changing directory\n"); return 1;
        }
        free(path);
        *path_ptr = path = new_path;
        chdir(path);
        return 0;
    } else if(strcmp(first_token, "echo") == 0) {
        char *token;
        int k=1;
        while(argv[k]) {
            printf("%s ", argv[k++]);
        }
        printf("\n");
    } else if(strcmp(first_token, "pwd") == 0) {
        puts(path);
    } else if(strcmp(first_token, "exit") == 0) {
        exit(0);
    } else {
        // printf("command: '%s', %d\n", command, command[command_length-1] == 't' && command[command_length-2] == '&');
        if(use_pthread) {

            pthread_t thread_id;
            pthread_create(&thread_id, NULL, pthread_system, command);
            pthread_join(thread_id, NULL); return 0;
        } else {
            // use fork
            // char *argv[100];
            // char *tmp;
            // int k = 1;
            // while((tmp = strtok_r(command, " ", &saveptr)) != NULL) { printf("tmp: %s\n", tmp); }
            // argv[0] = first_token;
            // argv[k] = NULL;
            char *second_token = argv[1];
            // printf("Running: '%s' '%s %s'\n", first_token, first_token, second_token);

            pid_t pid, child_pid;
            int status = 0;
            if(child_pid = fork() == 0) {
                // child process
                execvp(first_token, argv);
                exit(0);
            } else {
                // parent process
                while ((pid = wait(&status)) > 0);
            }
        }
    }
    free(argv);
}

int trim_whitespace(String *s) {
    char c, *data = s->data;
    int i = -1, len = s->length;

    // skip leading spaces
    do {
        c = data[++i];
    } while((c=='\n' || c =='\r' || c==' ' || c=='\t' || c=='\f') && c<len);
    s->data = &data[i];
    s->length = len - i;

    // find the last character, Last character may not be at len-1
    i = s->length;
    while(data[--i] == '\0'){}
    i++;

    // skip trailing spaces
    do {
        c = data[--i];
    } while((c=='\n' || c =='\r' || c==' ' || c=='\t' || c=='\f') && c>=0);
    s->length = i + 1;
    s->data[s->length] = '\0';

    return 0;
}

int main(int argc, char **argv)
{
    char *path = get_current_dir_name();

    char *command = NULL;
    size_t command_length = 0;
    ssize_t read;

    String trimmed_command;

    do {
        printf("%s$ ", path);
        
        if(read = getline(&command, &command_length, stdin) == -1) {
            break;
        }

        // remove leading & trailing whitespaces
        trimmed_command.data = command;
        trimmed_command.length = command_length;
        trim_whitespace(&trimmed_command);

        process_command(&path, trimmed_command.data, trimmed_command.length);
    } while(1);

    free(path);
    free(command);
    return 0;
}

