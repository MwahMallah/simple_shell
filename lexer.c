#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int is_delim(char ch)
{
    return ch == ' ' || ch == '\n' || ch == '\t';
}

parse_res parse(list* lst, char* buf)
{
    char* word = NULL;
    int w_quote = 0, w_sep_arg = 0, cur_len = 0;

    for (; *buf; buf++) {
        if (is_delim(*buf) && !word)
            continue;

        if ((is_delim(*buf) && !w_quote) || (w_quote && (*buf == '\"'))) {
            if (cur_len > 0) {
                char* to_add = strndup(word, cur_len);
                enque(lst, to_add, CMD_NONE);
                word = NULL;
                cur_len = 0;
            }
            w_sep_arg = 0;
            w_quote = 0;
        } else {
            if (is_sep(buf) && !w_quote) { //if encountered separator, add prev word
                if (cur_len > 0) {
                    char* to_add = strndup(word, cur_len);
                    enque(lst, to_add, CMD_NONE);
                    word = NULL;
                    cur_len = 0;
                    w_quote = 0;
                }

                char* sep = get_sep(buf);
                buf += strlen(sep) - 1;
                enque(lst, sep, get_sep_type(sep));
                w_sep_arg = waits_arg(get_sep_type(sep));
            } else {
                if (!word) {
                    word = buf;
                    if (*word == '\"') {
                        cur_len--;
                        w_quote = 1;
                        word++;
                    }
                }
                cur_len++;
            }
        }
    }

    if (cur_len > 0) {
        char* to_add = strndup(word, cur_len);
        enque(lst, to_add, CMD_NONE);
    }

    if (is_empty(lst)) {
        return EMPTY_CMD;
    } else if (w_quote) {
        return UNCLOSED_QUOTE;
    } else if (w_sep_arg) {
        return STREAM_REDIRECTION;
    }

    return SUCCESS;
}