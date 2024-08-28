#include "util.h"

/* Helper functions */
static int skip_sep(cmd_type type)
{
    int special = is_special(type);
    return !special;
}

static int skip_amt(cmd_type type)
{
    return 1 + args_amt(type);
}

static int stop_on_pipe(cmd_type type)
{
    return type == CMD_PIPE;
}

/* Implementation */
int proc_len(list* lst)
{
    return length(lst, &skip_sep, &skip_amt, &stop_on_pipe);
}

void proc2buf(list** lst, char** arr)
{
    lst2arr(lst, arr, &skip_sep, &skip_amt, &stop_on_pipe);
}