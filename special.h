#ifndef __SPECIAL_H
#define __SPECIAL_H

typedef enum {CMD_NONE, CMD_BG, CMD_IN, 
              CMD_OUT, CMD_APPEND, CMD_AND, 
              CMD_PIPE} cmd_type;

cmd_type get_sep_type(char* word);
int is_special(cmd_type type);
int is_sep(char* buf);
char* get_sep(char* buf);
int waits_arg(cmd_type type);
int args_amt(cmd_type type);

#endif