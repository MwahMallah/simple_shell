#ifndef __LEXER_H
#define __LEXER_H

#include "list.h"

typedef enum {EMPTY_CMD , UNCLOSED_QUOTE, STREAM_REDIRECTION, SUCCESS} parse_res;
parse_res parse(list* lst, char* buf); 

#endif