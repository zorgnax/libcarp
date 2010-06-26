/* Based on Perl's Carp package and GLib's gbacktrace.c  */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "carp.h"
#include "list.h"
#include "funcinfo.h"
#include "trace.h"

/* TODO
    die(), die(""), warn(), warn("")
    should return something like:
    Died at ...
    Warning: something's wrong at ...
    
    newline disables the line file info
*/
static void vwarn_at_loc (const char *file, const char *func, int line, int errnum, const char *mesg, va_list args) {
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

void warn_at_loc (const char *file, const char *func, int line, int errnum, const char *mesg, ...) {
    va_list args;
    va_start(args, mesg);
    vwarn_at_loc(file, func, line, errnum, mesg, args);
    va_end(args);
}

void die_at_loc (const char *file, const char *func, int line, int errnum, const char *mesg, ...) {
    va_list args;
    va_start(args, mesg);
    vwarn_at_loc(file, func, line, errnum, mesg, args);
    va_end(args);
    exit(255);
}

/* trims off carp library specifics and elements past main  */
static List *get_trimmed_stack_trace () {
    List *stack, *p;
    FuncInfo *f;
    
    stack = get_stack_trace();

    for (p = stack; p; p = p->next) {
        f = (FuncInfo *) p->data;
        if (!strcmp(f->func, "get_stack_trace")) {
            List *old_stack = stack;
            stack = p->next->next->next;
            p->next->next->next = NULL;
            list_free(old_stack, func_info_free);
            break;
        }
    }
    
    for (p = stack; p->next; p = p->next) {
        f = (FuncInfo *) p->data;
        if (!strcmp(f->func, "main") || !strcmp(f->func, "WinMain")) {
            list_free(p->next, func_info_free);
            p->next = NULL;
            break;
        }
    }
    
    return stack;
}

void stack_dump (List *stack) {
    List *p;
    
    printf("---\n");
    for (p = stack; p; p = p->next) {
        FuncInfo *f = (FuncInfo *) p->data;
        func_info_print(f);
    }
    printf("...\n");
}

static void vcarp_at_loc (int cluck, const char *file, const char *func, int line, int errnum, const char *mesg, va_list args) {
    List *stack, *l, *occurs = NULL;
    FuncInfo *f, *next, *suspect = NULL;

    stack = get_trimmed_stack_trace();

    if (mesg && mesg[0]) {
        vfprintf(stderr, mesg, args);
        if (errnum)
            fprintf(stderr, ": ");
    }
    if (errnum) {
        fprintf(stderr, "%s", strerror(errnum));
    }
    
    for (l = stack; l; l = l->next) {
        f = (FuncInfo *) l->data;
        if ((f->func && strcmp(f->func, func) == 0) ||
            (f->file && strcmp(f->file, file) == 0 && f->line == line)) {
            occurs = l;
        }
        if (occurs && !suspect && f->file && strcmp(f->file, file)) {
            suspect = (FuncInfo *) l->data;
        }
    }
    if (cluck || !suspect) {
        fprintf(stderr, " at %s line %d\n", file, line);
        for (l = occurs; l; l = l->next) {
            f = (FuncInfo *) l->data;
            if (f->func)
                fprintf(stderr, "    %s%s", f->func, "()");
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
    list_free(stack, func_info_free);
}

void carp_at_loc (const char *file, const char *func, int line, int errnum, const char *mesg, ...) {
    va_list args;
    va_start(args, mesg);
    vcarp_at_loc(0, file, func, line, errnum, mesg, args);
    va_end(args);
}

void croak_at_loc (const char *file, const char *func, int line, int errnum, const char *mesg, ...) {
    va_list args;
    va_start(args, mesg);
    vcarp_at_loc(0, file, func, line, errnum, mesg, args);
    va_end(args);
    exit(255);
}

void cluck_at_loc (const char *file, const char *func, int line, int errnum, const char *mesg, ...) {
    va_list args;
    va_start(args, mesg);
    vcarp_at_loc(1, file, func, line, errnum, mesg, args);
    va_end(args);
}

void confess_at_loc (const char *file, const char *func, int line, int errnum, const char *mesg, ...) {
    va_list args;
    va_start(args, mesg);
    vcarp_at_loc(1, file, func, line, errnum, mesg, args);
    va_end(args);
    exit(255);
}

