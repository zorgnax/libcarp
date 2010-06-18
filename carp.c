/* Based on Perl's Carp package and GLib's gbacktrace.c  */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "carp.h"
#include "list.h"
#include "funcinfo.h"
#include "trace.h"

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

static void vcarp_at_loc (int cluck, const char *file, const char *func, int line, int errnum, const char *mesg, va_list args) {
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
    list_free(st, func_info_free);
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


