/* Based on Perl's Carp package and GLib's gbacktrace.c  */

#include "carp.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

typedef struct {
    void *addr;
    char *func;
    char *args;
    char *file;
    int   line;
    char *lib;
} FuncInfo;

typedef struct _List List;

struct _List {
    void *data;
    List *next;
};

void func_info_print(FuncInfo *fi) {
    printf("fi: addr=%p, func=%s, args=%s\n"
           "    file=%s, line=%d, lib=%s\n",
           fi->addr,
           fi->func ? fi->func : "NULL",
           fi->args ? fi->args : "NULL",
           fi->file ? fi->file : "NULL",
           fi->line,
           fi->lib  ? fi->lib  : "NULL");
}

void func_info_free(FuncInfo *fi) {
    if (!fi)       return;
    if (fi->func)  free(fi->func);
    if (fi->args)  free(fi->args);
    if (fi->file)  free(fi->file);
    if (fi->lib)   free(fi->lib);
    free(fi);
}

static List *list_push(List *list, void *data) {
    List *new_list, *last;
    
    new_list = (List *) calloc(sizeof (List), 1);
    new_list->data = data;
    
    if (!list)
        return new_list;
    
    for (last = list; last->next; last = last->next);
    last->next = new_list;
    return list;
}

/* Parses gdb's stdout fd after requesting a backtrace. The result is stored
in a linked list of FuncInfo structures.  */
static List *create_stack_trace (int gdb) {
    char c, buf[4096];
    int i, parentheses_are_off = 0;
    FuncInfo *fi;
    List *st = NULL;
    
    enum {
        NONE,
        ADDR,
        FUNC,
        ARGS,
        FILE,
        LINE,
        LIB,
        QUOTE
    } state = NONE, prev_state = NONE;
    
    while (read(gdb, &c, 1)) {
        switch (state) {
        case NONE:
            if (c == '#') {
                fi = (FuncInfo *) calloc(sizeof (FuncInfo), 1);
                state = ADDR;
                while (read(gdb, &c, 1))
                    if (!isdigit(c))
                        break;
                while (read(gdb, &c, 1))
                    if (!isspace(c)) {
                        buf[0] = c;
                        i = 1;
                        break;
                    }
            }
            else if (st && c == '(') {
                read(gdb, buf, 3);
                if (strncmp(buf, "gdb", 3) == 0)
                    goto done;
            }
            else if (!st && c == 'N') {
                read(gdb, buf, 7);
                if (strncmp(buf, "o stack", 7) == 0)
                    goto done;
            }
            break;
        case ADDR:
            if (!isspace(c)) {
                buf[i++] = c;
            }
            else {
                buf[i] = 0;
                i = 0;
                sscanf(buf, "%p", &fi->addr);
                read(gdb, buf, 3);
                state = FUNC;
            }
            break;
        case FUNC:
            if (!isspace(c)) {
                buf[i++] = c;
            }
            else {
                buf[i] = 0;
                i = 0;
                fi->func = strdup(buf);
                state = ARGS;
            }
            break;
        case ARGS:
            buf[i++] = c;
            if (c == '(')
                parentheses_are_off++;
            else if (c == ')') {
                parentheses_are_off--;
                if (parentheses_are_off)
                    continue;
                buf[i] = 0;
                i = 0;
                fi->args = strdup(buf);
                read(gdb, &c, 1);
                if (c == '\n') {
                    st = list_push(st, (void *) fi);
                    state = NONE;
                    break;
                }
                read(gdb, buf, 3);
                if (strncmp(buf, "at ", 3) == 0) {
                    state = FILE;
                }
                else if (strncmp(buf, "fro", 3) == 0) {
                    state = LIB;
                    read(gdb, buf, 2);
                }
                else {
                    free(fi);
                    state = NONE;
                }
            }
            else if (c == '"') {
                prev_state = ARGS;
                state = QUOTE;
            }
            break;
        case QUOTE:
            buf[i++] = c;
            if (c == '\\') {
                read(gdb, &c, 1);
                buf[i++] = c;
            }
            else if (c == '"')
                state = prev_state;
            break;
        case LIB:
            if (!isspace(c)) {
                buf[i++] = c;
            }
            else {
                buf[i] = 0;
                i = 0;
                fi->lib = strdup(buf);
                st = list_push(st, (void *) fi);
                state = NONE;
            }
            break;
        case FILE:
            if (c != ':') {
                buf[i++] = c;
            }
            else {
                buf[i] = 0;
                i = 0;
                fi->file = strdup(buf);
                state = LINE;
            }
            break;
        case LINE:
            if (isdigit(c)) {
                buf[i++] = c;
            }
            else {
                buf[i] = 0;
                i = 0;
                fi->line = atoi(buf);
                st = list_push(st, (void *) fi);
                state = NONE;
            }
            break;
        }
    }
    
    done:
    return st;
}

/* Spawns a gdb process and returns a linked list of the backtrace  */
static List *get_stack_trace () {
    int in_fd[2], out_fd[2], status;
    pid_t pid;
    List *st;
    
    if (pipe(in_fd) == -1 || pipe(out_fd) == -1) {
        perror("pipe()");
        return NULL;
    }
    
    write(in_fd[1], "backtrace\n", 10);
    write(in_fd[1], "quit\n", 5);
    
    pid = fork();
    if (pid == 0) {
        char pidstr[16];
        
        dup2(in_fd[0], 0);
        dup2(out_fd[1], 1);
        dup2(out_fd[1], 2);
        snprintf(pidstr, sizeof pidstr, "%d", getppid());
        execlp("gdb", "gdb", "--nx", "--pid", pidstr, NULL);
        perror("execlp()");
        _exit(0);
    }
    else if (pid == -1) {
        perror("fork()");
        return NULL;
    }
    
    st = create_stack_trace(out_fd[0]);
    
    close(in_fd[0]);
    close(in_fd[1]);
    close(out_fd[0]);
    close(out_fd[1]);    
    waitpid(pid, &status, 0);

    return st;
}

static void vwarn_at_loc(const char *file, const char *func, int line, int errnum, const char *mesg, va_list args) {
    if (mesg && mesg[0]) {
        vfprintf(stderr, mesg, args);
        if (errnum)
            fprintf(stderr, ": ");
    }
    if (errnum) {
        fprintf(stderr, "%s", strerror(errnum));
    }
    fprintf(stderr, " at %s line %d\n", file, line);
}

void warn_at_loc(const char *file, const char *func, int line, int errnum, const char *mesg, ...) {
    va_list args;
    va_start(args, mesg);
    vwarn_at_loc(file, func, line, errnum, mesg, args);
    va_end(args);
}

void die_at_loc(const char *file, const char *func, int line, int errnum, const char *mesg, ...) {
    va_list args;
    va_start(args, mesg);
    vwarn_at_loc(file, func, line, errnum, mesg, args);
    va_end(args);
    _exit(255);
}

static void vcarp_at_loc(int cluck, const char *file, const char *func, int line, int errnum, const char *mesg, va_list args) {
    List *st, *l, *occurs = NULL;
    FuncInfo *fi, *next, *suspect = NULL;
    if (mesg && mesg[0]) {
        vfprintf(stderr, mesg, args);
        if (errnum)
            fprintf(stderr, ": ");
    }
    if (errnum) {
        fprintf(stderr, "%s", strerror(errnum));
    }
    st = get_stack_trace();
    for (l = st; l; l = l->next) {
        fi = (FuncInfo *) l->data;
        if ((fi->func && strcmp(fi->func, func) == 0) ||
            (fi->file && strcmp(fi->file, file) == 0 && fi->line == line)) {
            occurs = l;
        }
        if (occurs && !suspect && fi->file && strcmp(fi->file, file) != 0) {
            suspect = (FuncInfo *) l->data;
        }
    }
    if (cluck || !suspect) {
        fprintf(stderr, " at %s line %d\n", file, line);
        for (l = occurs; l; l = l->next) {
            fi = (FuncInfo *) l->data;
            if (fi->func)
                fprintf(stderr, "    %s%s", fi->func, "()");
            next = (FuncInfo *) l->next ? l->next->data : NULL;
            if (next && next->file)
                fprintf(stderr, " called at %s line %d", next->file, next->line);
            if (next && next->lib)
                fprintf(stderr, " from %s", next->lib);
            fprintf(stderr, "\n");
        }
    }
    else if (!suspect) {
        fprintf(stderr, " at %s line %d\n", file, line);
    }
    else {
        fprintf(stderr, " at %s line %d\n", suspect->file, suspect->line);
    }
    for (l = st; l;) {
        List *n = l->next;
        FuncInfo *fi = (FuncInfo *) l->data;
        func_info_free(fi);
        free(l);
        l = n;
    }
}

void carp_at_loc(const char *file, const char *func, int line, int errnum, const char *mesg, ...) {
    va_list args;
    va_start(args, mesg);
    vcarp_at_loc(0, file, func, line, errnum, mesg, args);
    va_end(args);
}

void croak_at_loc(const char *file, const char *func, int line, int errnum, const char *mesg, ...) {
    va_list args;
    va_start(args, mesg);
    vcarp_at_loc(0, file, func, line, errnum, mesg, args);
    va_end(args);
    _exit(255);
}

void cluck_at_loc(const char *file, const char *func, int line, int errnum, const char *mesg, ...) {
    va_list args;
    va_start(args, mesg);
    vcarp_at_loc(1, file, func, line, errnum, mesg, args);
    va_end(args);
}

void confess_at_loc(const char *file, const char *func, int line, int errnum, const char *mesg, ...) {
    va_list args;
    va_start(args, mesg);
    vcarp_at_loc(1, file, func, line, errnum, mesg, args);
    va_end(args);
    _exit(255);
}


