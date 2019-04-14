#ifndef FIND_PATHS_STACK_H
#define FIND_PATHS_STACK_H

void paths_init();
void paths_free();

int paths_empty();

void paths_push(char* path);
char* paths_pop();

#endif //FIND_PATHS_STACK_H
