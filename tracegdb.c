#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "trace.h"
#include "list.h"
#include "funcinfo.h"

typedef struct {
    void *from;
    void *to;
    int   readsyms;
    int   dbinfo;
    char *name;
} SharedLib;

static void shared_lib_free (SharedLib *lib) {
    if (!lib)
        return;
    free(lib->name);
    free(lib);
}

/* Looks through the list of libs and finds if the given address is between the
from and to addresses of the contained libraries.  */
static char *get_lib_name (List *libs, void *addr) {
    List *item;
    for (item = libs; item; item = item->next) {
        SharedLib *lib = item->data;
        if (lib->from <= addr && addr <= lib->to)
            return strdup(lib->name);
    }
    return NULL;
}

/* Parses gdb's stdout fd after requesting a backtrace. The result is stored
in a linked list of FuncInfo structures.  */
static List *parse_stack_trace (int gdb) {
    int i;
    int parentheses_are_off = 0;
    char c;
    char buf[4096];
    List *stack = NULL;
    FuncInfo *f;
    List *libs = NULL;
    SharedLib *lib;
    
    enum {
        NONE,
        ADDR,
        FUNC,
        ARGS,
        FILE,
        LINE,
        LIB,
        QUOTE,
        FROMADDR,
        TOADDR,
        SYMS,
        SONAME
    } state = NONE, prev_state = NONE;
    
    while (read(gdb, &c, 1)) {
        switch (state) {
        case NONE:
            if (c == '#') {
                f = calloc(sizeof (FuncInfo), 1);
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
            else if (!stack && c == '0') {
                lib = calloc(sizeof (SharedLib), 1);
                state = FROMADDR;
                buf[0] = c;
                i = 1;
            }
            else if (stack && c == '(') {
                read(gdb, buf, 3);
                if (!strncmp(buf, "gdb", 3))
                    goto done;
            }
            else if (!stack && c == 'N') {
                read(gdb, buf, 7);
                if (!strncmp(buf, "o stack", 7))
                    goto done;
            }
            break;
        case FROMADDR:
            if (!isspace(c)) {
                buf[i++] = c;
            }
            else {
                buf[i] = 0;
                i = 0;
                sscanf(buf, "%p", &lib->from);
                read(gdb, buf, 1);
                state = TOADDR;
            }
            break;
        case TOADDR:
            if (!isspace(c)) {
                buf[i++] = c;
            }
            else {
                buf[i] = 0;
                i = 0;
                sscanf(buf, "%p", &lib->to);
                read(gdb, buf, 1);
                state = SYMS;
            }
            break;
        case SYMS:
            if (!isspace(c)) {
                buf[i++] = c;
            }
            else {
                buf[i] = 0;
                i = 0;
                if (!strcmp(buf, "Yes"))
                    lib->readsyms = 1;
                read(gdb, buf, 3);
                if (strncmp(buf, "(*)", 3))
                    lib->dbinfo = 1;
                while (read(gdb, &c, 1))
                    if (!isspace(c)) {
                        buf[0] = c;
                        i = 1;
                        break;
                    }
                state = SONAME;
            }
            break;
        case SONAME:
            if (c != '\n') {
                buf[i++] = c;
            }
            else {
                buf[i] = 0;
                i = 0;
                lib->name = strdup(buf);
                libs = list_push(libs, lib);
                state = NONE;
            }
            break;
        case ADDR:
            if (!isspace(c)) {
                buf[i++] = c;
            }
            else {
                buf[i] = 0;
                i = 0;
                sscanf(buf, "%p", &f->addr);
                f->lib = get_lib_name(libs, f->addr);
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
                f->func = strdup(buf);
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
                f->args = strdup(buf);
                read(gdb, &c, 1);
                if (c == '\n') {
                    stack = list_push(stack, f);
                    state = NONE;
                    break;
                }
                read(gdb, buf, 3);
                if (!strncmp(buf, "at ", 3)) {
                    state = FILE;
                }
                else if (!strncmp(buf, "fro", 3)) {
                    state = LIB;
                    read(gdb, buf, 2);
                }
                else {
                    free(f);
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
                if (!f->lib)
                    f->lib = strdup(buf);
                stack = list_push(stack, f);
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
                f->file = strdup(buf);
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
                f->line = atoi(buf);
                stack = list_push(stack, f);
                state = NONE;
            }
            break;
        }
    }
    
    done:
    list_free(libs, shared_lib_free);
    return stack;
}

/* Spawns a gdb process and returns a linked list of the backtrace  */
List *get_stack_trace () {
    int in_fd[2], out_fd[2], status;
    pid_t pid;
    List *stack;
    
    if (pipe(in_fd) == -1 || pipe(out_fd) == -1) {
        perror("pipe()");
        return NULL;
    }
    
    write(in_fd[1], "info sharedlibrary\n", 19);
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
        exit(0);
    }
    else if (pid == -1) {
        perror("fork()");
        return NULL;
    }
    
    stack = parse_stack_trace(out_fd[0]);
    
    close(in_fd[0]);
    close(in_fd[1]);
    close(out_fd[0]);
    close(out_fd[1]);    
    waitpid(pid, &status, 0);

    return stack;
}
