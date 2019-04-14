#include "paths_stack.h"

#include <stdio.h>
#include <stdlib.h>

#define __PATHS_STACK_SIZE 1024

size_t __paths_max_size;
size_t __paths_size;
char** __paths;

void paths_init() {
    __paths_max_size = __PATHS_STACK_SIZE;
    __paths_size = 0;
    __paths = malloc(__paths_max_size * sizeof(char*));
    if (!__paths) {
        perror("find: allocation error");
        exit(EXIT_FAILURE);
    }
}

void __paths_increase_size() {
    __paths_max_size += __PATHS_STACK_SIZE;
    __paths = realloc(__paths, __paths_max_size * sizeof(char*));
    if (!__paths) {
        perror("find: allocation error");
        exit(EXIT_FAILURE);
    }
}

void paths_free() {
    free(__paths);
}

int paths_empty() {
    return __paths_size == 0;
}

void paths_push(char* path) {
    __paths[__paths_size] = path;
    ++__paths_size;
    if (__paths_size >= __paths_max_size) { __paths_increase_size(); }
}

char* paths_pop() {
    --__paths_size;
    if (__paths_size < 0) {
        perror("find: can't pop from empty paths stack");
        exit(EXIT_FAILURE);
    }
    char* res = __paths[__paths_size];
    __paths[__paths_size] = NULL;
    return res;
}
