#include "list.h"
#include <stdlib.h>
#include <stdio.h>

void init_list(list* lst)
{
    lst->first = NULL;
    lst->last = NULL;
}

void enque(list* lst, char* word, cmd_type type)
{
    item* itm = malloc(sizeof(item));
    itm->next = NULL;
    itm->word = word;
    itm->type = type;

    if (lst->first == NULL) {
        lst->first = lst->last = itm;
    } else {
        lst->last->next = itm;
        lst->last = itm;
    }
}

void delete_list(list* lst)
{
    item* curr = lst->first;

    while (curr) {
        item* tmp = curr;
        curr = curr->next;
        free(tmp->word);
        free(tmp);
    }

    lst->first = NULL;
    lst->last = NULL;
}

int length(const list* lst, int (*count)(cmd_type), int (*skip_amt)(cmd_type), int (*stop)(cmd_type))
{
    int cnt = 0;
    item* curr = lst->first;
    cmd_type type;
    while (curr && !stop(curr->type)) {
        type = curr->type;
        if (count(type))
            cnt++;
        for (int i = 0; i < skip_amt(type); i++) {
            curr = curr->next;
            if (!curr)
                break;
        }
    }
    return cnt;
}

void lst2arr(list** lst, char** arr, int (*add)(cmd_type), int (*skip_amt)(cmd_type), int (*stop)(cmd_type))
{
    item* curr = (*lst)->first;
    cmd_type type;
    int i = 0;
    while (curr && !stop(curr->type)) {
        type = curr->type;
        if (add(type)) 
            arr[i++] = curr->word;
        for (int j = 0; j < skip_amt(type); j++) {
            curr = curr->next;    
            if (!curr)
                break;
        }
    }    

    if (curr) { /*if stopped on on curr, add next word*/
        (*lst)->first = curr->next;
    }
}

int last(list* lst, int (*fun)(cmd_type))
{
    item* curr = lst->first;
    while (curr->next != NULL) {
        if (fun(curr->type))
            return -1;
        curr = curr->next;
    }
    
    return fun(lst->last->type);
}

char* sep_arg(list* lst, int (*fun)(cmd_type)) 
{
    item* curr = lst->first;
    while (curr) {
        if (fun(curr->type)) {
            return curr->next->word;
        }
        curr = curr->next;
    }    
    return NULL;
}

int is_empty(list* lst)
{
    return lst->first == NULL && lst->last == NULL;
}