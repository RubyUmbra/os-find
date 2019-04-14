#include "find_filters.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/stat.h>


long __inum = -1;

char* __name = NULL;

int __size_mode = 0;
long __size = -1;

long __nlinks = -1;

char* __exec_name = NULL;

void set_filter(char* name, char* value) {
    if (strcmp(name, "-inum") == 0) {
        __inum = atoi(value);
        if (__inum < 0) {
            perror("find: bad inum argument value");
            exit(EXIT_FAILURE);
        }
        return;
    }
    if (strcmp(name, "-name") == 0) {
        __name = value;
        return;
    }
    if (strcmp(name, "-size") == 0) {
        if (value[0] == '-') {
            __size_mode = -1;
        } else if (value[0] == '+') {
            __size_mode = 1;
        } else if (value[0] == '=') {
            __size_mode = 0;
        } else {
            perror("find: bad size argument value");
            exit(EXIT_FAILURE);
        }
        __size = atoi(value + 1);
        if (__size < 0) {
            perror("find: bad size argument value");
            exit(EXIT_FAILURE);
        }
        return;
    }
    if (strcmp(name, "-nlinks") == 0) {
        __nlinks = atoi(value);
        if (__nlinks < 0) {
            perror("find: bad nlinks argument value");
            exit(EXIT_FAILURE);
        }
        return;
    }
    if (strcmp(name, "-exec") == 0) {
        __exec_name = value;
        return;
    }
    perror("find: bad arguments");
    exit(EXIT_FAILURE);
}

int __execute(char* path) {
    if (__exec_name != NULL) {
        pid_t pid;
        int status;
        pid = fork();
        if (pid == 0) {
            char** args = (char**) malloc(1 * sizeof(char*));
            if (!args) {
                perror("find: allocation error");
                exit(EXIT_FAILURE);
            }
            args[0] = path;
            if (execve(__exec_name, args, NULL)) {
                fprintf(stderr, "find: '%s': ", path);
                perror("");
            }
            perror("find");
            return (EXIT_FAILURE);
        } else if (pid < 0) {
            perror("find");
            return (EXIT_FAILURE);
        } else {
            do {
                if (waitpid(pid, &status, WUNTRACED) == -1) {
                    perror("find");
                    return (EXIT_FAILURE);
                }
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
    } else {
        printf("%s\n", path);
    }
    return EXIT_SUCCESS;
}

int pass_filters(char* path) {
    struct stat st;
    if (stat(path, &st) < 0) {
        fprintf(stderr, "find: '%s'", path);
        perror(" ");
        return -1;
    }
    int is_dir = S_ISDIR(st.st_mode) ? 1 : 0;
    if (__inum >= 0) { if (__inum != st.st_ino) { return is_dir; }}
    if (__name != NULL) { if (strcmp(__name, basename(strdup(path))) != 0) { return is_dir; }}
    if (__size >= 0) {
        if (__size_mode < 0) {
            if (__size < st.st_size) { return is_dir; }
        } else if (__size_mode > 0) {
            if (__size > st.st_size) { return is_dir; }
        } else {
            if (__size != st.st_size) { return is_dir; }
        }
    }
    if (__nlinks >= 0) { if (__nlinks != st.st_nlink) { return is_dir; }}
    __execute(path);
    return is_dir;
}
