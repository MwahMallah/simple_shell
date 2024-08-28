#include "analyzer.h"

/* Helper functions */
static int is_bg(cmd_type type) 
{
    return type == CMD_BG;
}

static int is_input(cmd_type type)
{
    return type == CMD_IN;
}

static int is_output(cmd_type type) 
{
    return type == CMD_OUT;
}

static int is_append(cmd_type type)
{
    return type == CMD_APPEND;
}

static int is_pipe(cmd_type type)
{
    return type == CMD_PIPE;
}

static int never_stop(cmd_type type)
{
    return 0;
}

//read next word in input
static int next() 
{
    return 1;
}

/* Implementation */
int in_bg(list* lst) 
{
    return last(lst, &is_bg);
}

char* input_red(list* lst)
{
    return sep_arg(lst, &is_input);
}

char* output_red(list* lst)
{
    return sep_arg(lst, &is_output);
}

char* append_red(list* lst)
{
    return sep_arg(lst, &is_append);
}

int cnt_progs(list* lst)
{
    return 1 + length(lst, &is_pipe, &next, &never_stop);   
}