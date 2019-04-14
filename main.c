#include "paths_stack.h"
#include "find_filters.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define _GNU_SOURCE

#include <sys/syscall.h>

#define FIND_SOFT_ERROR(msg) {perror(msg);return(EXIT_FAILURE);}
#define FIND_HARD_ERROR(msg) {perror(msg);exit(EXIT_FAILURE);}

#define FIND_BUF_SIZE 1024

void __parse_args(int argc, char** argv) {
    if (argc < 2) FIND_HARD_ERROR("find: path must be specified by first argument")
    paths_push(argv[1]);
    for (int i = 2; argv[i]; i += 2) {
        if (!argv[i + 1]) FIND_HARD_ERROR("find: bad arguments")
        set_filter(argv[i], argv[i + 1]);
    }
}

struct linux_dirent {
    long d_ino;
    off_t d_off;
    unsigned short d_reclen;
    char d_name[];
};

int __add_entries(char* path) {
    int fd = open(path, O_RDONLY | O_DIRECTORY);
    if (fd == -1) {
        fprintf(stderr, "find: '%s': ", path);
        FIND_SOFT_ERROR("")
    }
    char buf[FIND_BUF_SIZE];
    struct linux_dirent* d;
    for (int nread = 0;;) {
        nread = syscall(SYS_getdents, fd, buf, FIND_BUF_SIZE);
        if (nread == -1) {
            fprintf(stderr, "find: '%s': ", path);
            FIND_SOFT_ERROR("")
        }
        if (nread == 0) break;
        for (int i = 0; i < nread; i += d->d_reclen) {
            d = (struct linux_dirent*) (buf + i);
            if (strcmp(d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0) {
                char* next;
                if (path[strlen(path) - 1] == '/') {
                    next = malloc((strlen(path) + strlen(d->d_name)) * sizeof(char) + 1);
                    sprintf(next, "%s%s", path, d->d_name);
                } else {
                    next = malloc((strlen(path) + strlen(d->d_name)) * sizeof(char) + 2);
                    sprintf(next, "%s/%s", path, d->d_name);
                }
                paths_push(next);
            }
        }
    }
    return EXIT_SUCCESS;
}

void __walk(char* path) {
    int is_dir = pass_filters(path);
    if (is_dir) { __add_entries(path); }
}

void __walk_loop() { while (!paths_empty()) { __walk(paths_pop()); }}

int main(int argc, char** argv) {
    paths_init();
    __parse_args(argc, argv);
    __walk_loop();
    paths_free();
    return EXIT_SUCCESS;
}
