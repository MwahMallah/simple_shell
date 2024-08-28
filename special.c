#include "special.h"
#include <string.h>

static int eq(char* str1, char* str2)
{
    return strcmp(str1, str2) == 0;
}

cmd_type get_sep_type(char* word) 
{
    if (eq(word, "&"))
        return CMD_BG;
    if (eq(word, ">"))
        return CMD_OUT;
    if (eq(word, "<"))
        return CMD_IN;
    if (eq(word, ">>"))
        return CMD_APPEND;
    if (eq(word, "&&"))
        return CMD_AND; 
    if (eq(word, "|"))
        return CMD_PIPE;

    return CMD_NONE;
}

int waits_arg(cmd_type type)
{
    return type == CMD_OUT || type == CMD_IN || type == CMD_APPEND;
}

int is_sep(char* buf)
{
    if ((buf[0] == '&' && buf[1] == '&') || (buf[0] == '>' && buf[1] == '>')) {
        return 1;
    }

    if (buf[0] == '&' || buf[0] == '>' || buf[0] == '<' || buf[0] == '|') {
        return 1;
    }

    return 0;
}

char* get_sep(char* buf) 
{
    char* sep;
    int cc = 1;
    if (buf[0] == '&' && buf[1] == '&')
        cc++;
    else if (buf[0] == '|' && buf[1] == '|')
        cc++;
    else if (buf[0] == '>' && buf[1] == '>')
        cc++;

    sep = strndup(buf, cc);
    return sep;
}

int is_special(cmd_type type)
{
    return type != CMD_NONE;
}

int args_amt(cmd_type type)
{
    if (type == CMD_APPEND || type == CMD_IN || type == CMD_OUT)
        return 1;
    
    return 0;
}
