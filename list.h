#ifndef __LIST_H
#define __LIST_H

#include "special.h"

typedef struct item_t {
    char* word;
    cmd_type type;
    struct item_t* next;
} item;

typedef struct list_t {
    item* first;
    item* last;
} list;

void init_list(list* lst);
void enque(list* lst, char* word, cmd_type type);
void delete_list(list* lst);
int length(const list* lst, int (*count)(cmd_type), int (*skip_amt)(cmd_type), int (*stop)(cmd_type));
void lst2arr(list** lst, char** arr, int (*add)(cmd_type), int (*skip_amt)(cmd_type), int (*stop)(cmd_type));
char* sep_arg(list* lst, int (*fun)(cmd_type));
int last(list* lst, int (*fun)(cmd_type));
int is_empty(list* lst);

#endif