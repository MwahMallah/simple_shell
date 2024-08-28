#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include "list.h"
#include "special.h"
#include "lexer.h"
#include "analyzer.h"
#include "util.h"
#include "cmd_line.h"

enum {buf_size = 1024};

static void cd(list* lst)
{
    char* dst;
    if (lst->first->next && lst->first->next->next) { /* if >1 arguments */
        printf("Usage of cd: cd <dst>.\n");
        return;
    }

    if (!lst->first->next) {
        dst = getenv("HOME");
    } else {
        dst = lst->first->next->word;
    }
    chdir(dst);
}

static void wait_procs(int gid)
{
    int status;
    pid_t pid;

    while ((pid = waitpid(-gid, &status, 0)) >= 0) 
        {}   

    if (pid == -1 && errno != ECHILD) {
        perror("waitpid");
    }

    fflush(stdout);
}

static void rmv_zomb()
{
    while (waitpid(-1, NULL, WNOHANG) > 0) 
        {}
}

typedef struct {
    int bg;
    int cnt_prog;
    char* in;
    char* out;
    char* append;
} cmd_params;

static int get_cmd_info(cmd_params* params, list* lst)
{
    params->bg = in_bg(lst);
    if (params->bg == -1) {
        printf("Error: '&' should be put last\n");
        return -1;
    } 
    params->in  = input_red(lst);
    params->out = output_red(lst);
    params->append = append_red(lst);
    params->cnt_prog = cnt_progs(lst);

    if (params->append && params->out) {
        printf("Error: can't append and redirect output in the same time\n");
        return -1;
    }

    return 0;
}

static int redirect_stream(char* fn, int stream_fd, int flags, int file_mode) 
{
    int fd;
    if (fn) {
        fd = open(fn, flags, file_mode);
        if (fd == -1) {
            perror(fn);
            return 0;
        }
        dup2(fd, stream_fd);
        close(fd);
    }

    return 1;
}

static int redirect_out(cmd_params* params)
{
    if (!redirect_stream(params->out, 1, O_WRONLY|O_CREAT|O_TRUNC, 0777)) {
        return 0;
    }

    if (!redirect_stream(params->append, 1, O_WRONLY|O_CREAT|O_APPEND, 0777)) {
        return 0;
    }

    return 1;
}

static int redirect_in(cmd_params* params) {
    return redirect_stream(params->in, 0, O_RDONLY, 0);
}

static void exec(list* lst) 
{
    int len, pid, analyze_res, i;
    cmd_params params;
    int gid = -1;
    analyze_res = get_cmd_info(&params, lst);
    if (analyze_res == -1) 
        return;

    if (strcmp(lst->first->word, "cd") == 0) {
        cd(lst);
        return;
    }

    int fd[params.cnt_prog - 1][2];
    for (i = 0; i < params.cnt_prog; i++) {
        len = proc_len(lst);
        char* buf[len + 1];
        proc2buf(&lst, buf);
        buf[len] = 0;

        if (i < params.cnt_prog - 1) {  // No need to create pipe for the last command
            pipe(fd[i]);
        }

        pid = fork();
        if (pid == 0) {
            if (gid == -1) {
                setpgid(0, 0);
            } else {
                setpgid(0, gid);
            }

            close(fd[i][0]); //close current read descriptor for this process
            if (i > 0) {  // Set input from previous pipe if not the first command
                dup2(fd[i - 1][0], 0);
                close(fd[i - 1][0]);
            } else {
                if (!redirect_in(&params)) {
                    exit(1);
                }
            }

            if (i < params.cnt_prog - 1) {  // Set output to the next pipe if not the last command
                dup2(fd[i][1], 1);
                close(fd[i][1]);
            } else {
                if (!redirect_out(&params)) {
                    exit(1);
                }
            }

            execvp(buf[0], buf);
            perror(buf[0]);
            exit(1);
        }

        if (gid == -1) {
            gid = pid; //group id is equal to first child id
            setpgid(pid, gid);
        } 
        
        // Close pipe ends in the parent
        if (i > 0) {
            close(fd[i - 1][0]);  // Close the reading end of the previous pipe
        }
        if (i < params.cnt_prog - 1) {
            close(fd[i][1]);  // Close the writing end of the current pipe
        }
    }

    if (!params.bg) {
        signal(SIGTTOU, SIG_IGN);
        int grpid = tcgetpgrp(0);
        if (tcsetpgrp(0, gid) < 0) {
            perror("tcsetprgrp (foreground)");
        }

        wait_procs(gid);

        if (tcsetpgrp(0, grpid) < 0) {
            perror("tcsetprgrp (regain terminal)");
        }
    }
}

int main(void) 
{
    parse_res res;
    char buf[buf_size];
    signal(SIGCHLD, rmv_zomb);
    list* lst = malloc(sizeof(list));
    init_list(lst);

    while (get_cmd_string(buf, sizeof(buf))) {
        res = parse(lst, buf);
        if (res == UNCLOSED_QUOTE) {
            printf("Error, quotes are not closed.\n");
        } else if (res == SUCCESS) {
            exec(lst);
        } else if (res == STREAM_REDIRECTION) {
            printf("Error while redirecting stream.\n");
        }

        delete_list(lst);
    }

    free(lst);
    return 0;
}