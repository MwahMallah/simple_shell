#define _DEFAULT_SOURCE

#include "cmd_line.h"
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

enum {bufsize = 4096, terminal_width = 80};

static int is_executable(const char* path) {
    struct stat st;
    return stat(path, &st) && (st.st_mode & S_IXUSR);
}

static char* start_with(const char* prefix, char* word) {
    char* curr = word;
    while (*prefix) {
        if (*curr != *prefix)
            return NULL;
        prefix++;
        curr++;
    }
    
    return word;
}

static char* find_file(const char* wstart) {
    DIR* dir = opendir(".");
    struct dirent *dp;

    while (dir) {
        if ((dp = readdir(dir))) {
            if (start_with(wstart, dp->d_name)) {
                closedir(dir);
                return strndup(dp->d_name, strlen(dp->d_name));
            }
        } else {
            closedir(dir);
        }
    }

    return NULL;
}

static char* find_cmd(char* wstart) {
    struct dirent* dir;
    char* path_env = getenv("PATH");
    
    char* path = strdup(path_env);
    char* dpath = strtok(path, ":");

    while (dpath) {
        DIR* d = opendir(dpath);
        while ((dir = readdir(d))) {
            if (start_with(wstart, dir->d_name)) {
                char fullpath[1024];
                snprintf(fullpath, sizeof(fullpath), "%s%s", dpath, dir->d_name);
                if (is_executable(fullpath)) {
                    closedir(d);
                    free(path);
                    return strndup(dir->d_name, strlen(dir->d_name));
                } 
            }
        }

        closedir(d);
        dpath = strtok(NULL, ":");
    }
    
    free(path);
    return NULL;
}

static char* diff_from(char* pat, char* src) {
    while (*src) {
        if (*pat != *src) 
            return NULL;
        pat++;
        src++;
    }
    
    return pat;
}

static char* last_word(char* buf) {
    char* last = buf;
    while (*buf) {
        if (*buf == ' ') {
            last = buf + 1;
        }
        buf++;
    }
    
    return *last == ' ' ? NULL : last;
}

static void set_noncanon(struct termios* tsorig) {
    struct termios ts;
    tcgetattr(0, &ts);
    memcpy(tsorig, &ts, sizeof(ts));

    ts.c_lflag &= ~(ICANON | ECHO | IEXTEN);
    tcsetattr(0, TCSANOW, &ts);
}

static void change_cursor(int* cursor, int endbuf, int code) {
    switch (code) {
        case 68:
            if (*cursor > 0) 
                (*cursor)--;
            break;
        case 67:
            if (*cursor < endbuf) 
                (*cursor)++;
            break;
        default:
            break;
    }

}

static void move_cursor(int cursor, int width) {
    printf("\033[%dG", cursor % width + 3); // Move cursor to column
    fflush(stdout);
}

static void insert_char(char* buf, char ch, int cursor, int* endbuf) {
    for (int i = *endbuf; i > cursor; i--) {
        buf[i] = buf[i - 1];
    }
    buf[cursor] = ch;
    (*endbuf)++;
    buf[*endbuf] = '\0';

    // Move the cursor to the new position and update the terminal
    move_cursor(cursor, terminal_width);
    printf("%s", buf + cursor);
    // Move the cursor back to it's position
    move_cursor(cursor + 1, terminal_width);
    fflush(stdout);
}

static void shift_left(char* buf, int* cursor, int* endbuf) {
    for (int i = *cursor - 1; i < *endbuf - 1; i++) {
        buf[i] = buf[i + 1];
    }
    (*endbuf)--;
    buf[*endbuf] = '\0';
    if (*cursor > 0)
        (*cursor)--;

    printf("\033[2K\r"); // Clear current line
    printf("> %s", buf);
    move_cursor(*cursor, terminal_width);
    fflush(stdout);
}

static void autocomplete(char* wstart, char* to_complete, char* buf, int* cursor, int* endbuf) {
    char* to_add = diff_from(wstart, to_complete); 
    printf("%s", to_add);
    fflush(stdout);

    for (int j = 0; j < strlen(to_add); j++) {
        buf[(*endbuf)++] = to_add[j];
        (*cursor)++;
    }
    free(wstart);                    
}

int get_cmd_string(char* output_buf, int bufsize) {
    struct termios tsorig;
    int rc, cursor = 0, endbuf = 0, wcmd = 1;
    char input_buf[4];

    printf("> ");
    fflush(stdout);
    set_noncanon(&tsorig);
    
    while ((rc = read(0, input_buf, sizeof(input_buf)))) {
        for (int i = 0; i < rc; i++) {
            if (input_buf[i] == 27) {
                if (input_buf[++i] != 91) {
                    printf("Unknown code: %d\n", input_buf[i - 1]);
                    continue;
                }

                int code = input_buf[++i];
                change_cursor(&cursor, endbuf, code);
                move_cursor(cursor, terminal_width);
            } else if (input_buf[i] == '\t') {
                char* last = last_word(output_buf);
                char* found = wcmd? find_cmd(last) : find_file(last);

                if (found) {
                    autocomplete(found, last, output_buf, &cursor, &endbuf);
                    wcmd = 0;
                }
            } else if (input_buf[i] == '\b' || input_buf[i] == 127) {
                printf("\b \b");
                fflush(stdout);

                shift_left(output_buf, &cursor, &endbuf);
            } else if (input_buf[i] == '\n') {
                putchar(input_buf[i]);
                fflush(stdout);
                goto end;
            } else {
                if (input_buf[i] == ' ')
                    wcmd = 0;
                else if (input_buf[i] == '|')
                    wcmd = 1;
                insert_char(output_buf, input_buf[i], cursor, &endbuf);
                fflush(stdout);
                cursor++;
            }
        }
    }

end:
    output_buf[endbuf++] = '\n';
    output_buf[endbuf] = '\0';
    tcsetattr(0, TCSANOW, &tsorig);
    return 1;
}