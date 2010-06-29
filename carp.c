/* Based on Perl's Carp package and GLib's gbacktrace.c  */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "carp.h"
#include "list.h"
#include "funcinfo.h"
#include "trace.h"

static void output_builtin (const char *mesg) {
    fputs(mesg, stderr);
}

static CarpOutputFunc output = output_builtin;

/* Appends a formatted string to a string. The result must be freed. */
static char *vappend (char *str, const char *fmt, va_list args) {
    int str_size = str ? strlen(str) : 0;
    int size = vsnprintf(NULL, 0, fmt, args) + str_size + 1;
    str = realloc(str, size);
    vsprintf(str + str_size, fmt, args);
    return str;
}

static char *append (char *str, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    str = vappend(str, fmt, args);
    va_end(args);
    return str;
}

typedef enum {
    CLUCK = 1 << 0,
    DIE   = 1 << 1
} CarpFlags;

/* TODO newline should disable the file line backtrace madness  */
static char *vcarp_message (const char *fmt, va_list args, int errnum, CarpFlags flags) {
    char *mesg = NULL;
    if (fmt && *fmt) {
        mesg = vappend(mesg, fmt, args);
        if (errnum)
            mesg = append(mesg, ": %s", strerror(errnum));
    }
    else if (errnum)
        mesg = append(mesg, "%s", strerror(errnum));
    else if (flags & DIE)
        mesg = append(mesg, "Died");
    else
        mesg = append(mesg, "Warning: something's wrong");
    return mesg;
}

static char *vswarn_at_loc (CarpFlags flags, const char *file, const char *func, int line, int errnum, const char *fmt, va_list args) {
    char *mesg = vcarp_message(fmt, args, errnum, flags);
    mesg = append(mesg, " at %s line %d\n", file, line);
    return mesg;
}

static void vwarn_at_loc (CarpFlags flags, const char *file, const char *func, int line, int errnum, const char *fmt, va_list args) {
    char *mesg = vswarn_at_loc(flags, file, func, line, errnum, fmt, args);
    output(mesg);
    free(mesg);
}

char *swarn_at_loc (const char *file, const char *func, int line, int errnum, const char *fmt, ...) {
    char *mesg;
    va_list args;
    va_start(args, fmt);
    mesg = vswarn_at_loc(0, file, func, line, errnum, fmt, args);
    va_end(args);
    return mesg;
}

void warn_at_loc (const char *file, const char *func, int line, int errnum, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vwarn_at_loc(0, file, func, line, errnum, fmt, args);
    va_end(args);
}

char *sdie_at_loc (const char *file, const char *func, int line, int errnum, const char *fmt, ...) {
    char *mesg;
    va_list args;
    va_start(args, fmt);
    mesg = vswarn_at_loc(DIE, file, func, line, errnum, fmt, args);
    va_end(args);
    return mesg;
}

void die_at_loc (const char *file, const char *func, int line, int errnum, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vwarn_at_loc(DIE, file, func, line, errnum, fmt, args);
    va_end(args);
    exit(255);
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

/* trims off carp library specifics and elements past main  */
static List *get_trimmed_stack_trace () {
    List *stack, *p;
    FuncInfo *f;
    
    stack = get_stack_trace();
    if (!stack)
        return NULL;

    for (p = stack; p; p = p->next) {
        f = (FuncInfo *) p->data;
        if (!strcmp(f->func, "vcarp_at_loc")) {
            List *old_stack = stack;
            /* Am I assuming too much?  */
            stack = p->next->next;
            p->next->next = NULL;
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

/* A strcmp that handles NULL values */
static int mystrcmp (char *a, char *b) {
    return a == b ? 0 : !a ? -1 : !b ? 1 : strcmp(a, b);
}

static FuncInfo *get_suspect (List *stack) {
    List *p;
    FuncInfo *occurs = stack->data;

    for (p = stack; p; p = p->next) {
        FuncInfo *f = p->data;
        if (mystrcmp(f->lib, occurs->lib) || mystrcmp(f->file, occurs->file))
            return f;
    }
    
    /* Without a suspect, default to where the error occured  */
    return occurs;
}

static char *vscarp_at_loc (CarpFlags flags, const char *file, const char *func, int line, int errnum, const char *fmt, va_list args) {
    List *stack;
    char *mesg;

    stack = get_trimmed_stack_trace();
    if (!stack)
        return vswarn_at_loc(flags, file, func, line, errnum, fmt, args);

    mesg = vcarp_message(fmt, args, errnum, flags);

    if (flags & CLUCK) {
        List *cur, *prev;
        
        for (prev = NULL, cur = stack; cur; prev = cur, cur = cur->next) {
            FuncInfo *prevf = prev ? prev->data : NULL;
            FuncInfo *curf = cur->data;
            
            if (prevf)
                mesg = append(mesg, "    %s() called", prevf->func);
            
            if (curf->file)
                mesg = append(mesg, " at %s line %d", curf->file, curf->line);
            else if (!prev)
                mesg = append(mesg, " at %s line %d", file, line);
            
            if (curf->lib)
                mesg = append(mesg, " from %s", curf->lib);
            
            mesg = append(mesg, "\n");
        }
    }
    else {
        FuncInfo *suspect = get_suspect(stack);
        if (!suspect || !suspect->file)
            mesg = append(mesg, " at %s line %d\n", file, line);
        else
            mesg = append(mesg, " at %s line %d\n", suspect->file,
                                                    suspect->line);
    }

    list_free(stack, func_info_free);
    return mesg;
}

static void vcarp_at_loc (CarpFlags flags, const char *file, const char *func, int line, int errnum, const char *fmt, va_list args) {
    char *mesg = vscarp_at_loc(flags, file, func, line, errnum, fmt, args);
    output(mesg);
    free(mesg);
}

char *scarp_at_loc (const char *file, const char *func, int line, int errnum, const char *fmt, ...) {
    char *mesg;
    va_list args;
    va_start(args, fmt);
    mesg = vscarp_at_loc(0, file, func, line, errnum, fmt, args);
    va_end(args);
    return mesg;
}

void carp_at_loc (const char *file, const char *func, int line, int errnum, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vcarp_at_loc(0, file, func, line, errnum, fmt, args);
    va_end(args);
}

char *scroak_at_loc (const char *file, const char *func, int line, int errnum, const char *fmt, ...) {
    char *mesg;
    va_list args;
    va_start(args, fmt);
    mesg = vscarp_at_loc(DIE, file, func, line, errnum, fmt, args);
    va_end(args);
    return mesg;
}

void croak_at_loc (const char *file, const char *func, int line, int errnum, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vcarp_at_loc(DIE, file, func, line, errnum, fmt, args);
    va_end(args);
    exit(255);
}

char *scluck_at_loc (const char *file, const char *func, int line, int errnum, const char *fmt, ...) {
    char *mesg;
    va_list args;
    va_start(args, fmt);
    mesg = vscarp_at_loc(CLUCK, file, func, line, errnum, fmt, args);
    va_end(args);
    return mesg;
}

void cluck_at_loc (const char *file, const char *func, int line, int errnum, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vcarp_at_loc(CLUCK, file, func, line, errnum, fmt, args);
    va_end(args);
}

char *sconfess_at_loc (const char *file, const char *func, int line, int errnum, const char *fmt, ...) {
    char *mesg;
    va_list args;
    va_start(args, fmt);
    mesg = vscarp_at_loc(CLUCK | DIE, file, func, line, errnum, fmt, args);
    va_end(args);
    return mesg;
}

void confess_at_loc (const char *file, const char *func, int line, int errnum, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vcarp_at_loc(CLUCK | DIE, file, func, line, errnum, fmt, args);
    va_end(args);
    exit(255);
}


