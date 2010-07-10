#include "carp.h"
#include "carppriv.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

static void output_builtin (const char *mesg) {
    fputs(mesg, stderr);
}

static int             verbose         = 0;
static int             muzzled         = 0;
static int             dump_stack      = 0;
static CarpOutputFunc  output          = output_builtin;
static int             strip           = 0;
static List           *trusted_files   = NULL;
static List           *trusted_libs    = NULL;

static void  init          ();
static char *vscarp_at_loc ();

/* A strcmp that handles NULL values */
static int mystrcmp (const char *a, const char *b) {
    return a == b ? 0 : !a ? -1 : !b ? 1 : strcmp(a, b);
}

/*
carp settings

"verbose" int
All macros show the full stack trace. This makes warn and carp the same as a
cluck, die and croak the same as a confess.
Defaults to environment var CARP_VERBOSE

"muzzled" int
Stack trace is never performed. This makes carp and cluck the same as warn,
croak and confess the same as die.
Defaults to environment var CARP_MUZZLED

"dump-stack" int
Print as much info as you can about the stack.
Defaults to environment var CARP_DUMP_STACK

"output" char*
may be "default" or "color" as builtin output funcs.
Defaults to environment var CARP_OUTPUT

"output-func" func
A CarpOutputFunc to output the error message.

"strip" int
The number of items to remove from the file names.
Defaults to environment var CARP_STRIP

"suspected-files" char*
Comma separated list of file names to remove from the list of trusted files.

"suspected-libs" char*
Comma separated list of lib names to remove from the list of trusted libs.

"trusted-files" char*
Comma separated list of file names to be trusted.
Defaults to environment var CARP_TRUSTED_FILES

"trusted-libs" char*
Comma separated list of lib names to be trusted.
Defaults to environment var CARP_TRUSTED_LIBS

*/
void carp_set (const char *key, ...) {
    va_list args;
    init();
    va_start(args, key);
    for (; key; key = va_arg(args, const char *)) {
        if (!strcmp(key, "verbose")) {
            verbose = va_arg(args, int);
            if (verbose)
                muzzled = 0;
        }
        else if (!strcmp(key, "muzzled")) {
            muzzled = va_arg(args, int);
            if (muzzled)
                verbose = 0;
        }
        else if (!strcmp(key, "dump-stack")) {
            dump_stack = va_arg(args, int);
        }
        else if (!strcmp(key, "output")) {
            const char *value = va_arg(args, const char *);
            if (!value || !mystrcmp(value, "default"))
                output = output_builtin;
            else if (!mystrcmp(value, "color"))
                croak("Color output has not been implemented yet");
            else
                croak("Unknown output builtin '%s'", value);
        }
        else if (!strcmp(key, "output-func")) {
            output = va_arg(args, CarpOutputFunc);
        }
        else if (!strcmp(key, "strip")) {
            strip = va_arg(args, int);
        }
        else if (!strcmp(key, "suspected-files")) {
            trusted_files = NULL;
            /* TODO  */
        }
        else if (!strcmp(key, "suspected-libs")) {
            trusted_libs = NULL;
            /* TODO  */
        }
        else if (!strcmp(key, "trusted-files")) {
            trusted_files = NULL;
            /* TODO  */
        }
        else if (!strcmp(key, "trusted-libs")) {
            trusted_libs = NULL;
            /* TODO  */
        }
        else {
            croak("Unknown setting name '%s'", key);
        }
    }
    va_end(args);
}

static int getintenv (const char *var) {
    char *val = getenv(var);
    return val ? atoi(val) : 0;
}

static void init () {
    static int init = 0;
    
    if (init++)
        return;

    carp_set(
        "output",         getenv   ("CARP_OUTPUT"),
        "verbose",        getintenv("CARP_VERBOSE"),
        "muzzled",        getintenv("CARP_MUZZLED"),
        "dump-stack",     getintenv("CARP_DUMP_STACK"),
        "strip",          getintenv("CARP_STRIP"),
        NULL
    );
}

/* Appends a formatted string to a string. The result must be freed.  */
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
static char *vcarp_message (const char *fmt,
                            va_list     args,
                            int         errnum,
                            CarpFlags   flags)
{
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

static char *vswarn_at_loc (CarpFlags   flags,
                            const char *file,
                            const char *func,
                            int         line,
                            int         errnum,
                            const char *fmt,
                            va_list     args)
{
    char *mesg;
    init();
    if (verbose)
        return vscarp_at_loc(flags, file, func, line, errnum, fmt, args);
    mesg = vcarp_message(fmt, args, errnum, flags);
    mesg = append(mesg, " at %s line %d\n", file, line);
    return mesg;
}

static void vwarn_at_loc (CarpFlags   flags,
                          const char *file,
                          const char *func,
                          int         line,
                          int         errnum,
                          const char *fmt,
                          va_list     args)
{
    char *mesg = vswarn_at_loc(flags, file, func, line, errnum, fmt, args);
    output(mesg);
    free(mesg);
    if (flags & DIE)
        exit(255);
}

static void dump (List *stack) {
    List *p;
    printf("---\n");
    for (p = stack; p; p = p->next) {
        func_info_print(p->data);
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

    if (dump_stack)
        dump(stack);

    for (p = stack; p; p = p->next) {
        f = p->data;
        if (!strcmp(f->func, "vscarp_at_loc") ||
            !strcmp(f->func, "vswarn_at_loc"))
        {
            List *old_stack = stack;
            FuncInfo *nextf = p->next->data;
            if (!strcmp(nextf->func, "vcarp_at_loc") ||
                !strcmp(nextf->func, "vwarn_at_loc"))
            {
                stack = p->next->next->next;
                p->next->next->next = NULL;
            }
            else {
                stack = p->next->next;
                p->next->next = NULL;
            }
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

static FuncInfo *get_suspect (List *stack) {
    List *p;
    FuncInfo *occurs = stack->data;

    for (p = stack; p; p = p->next) {
        FuncInfo *f = p->data;
        if (mystrcmp(f->lib, occurs->lib))
            return f;
    }
    
    /* Without a suspect, default to where the error occured  */
    return occurs;
}

static char *vscarp_at_loc (CarpFlags   flags,
                            const char *file,
                            const char *func,
                            int         line,
                            int         errnum,
                            const char *fmt,
                            va_list     args)
{
    List *stack;
    char *mesg;
    init();
    if (muzzled)
        return vswarn_at_loc(flags, file, func, line, errnum, fmt, args);
    mesg = vcarp_message(fmt, args, errnum, flags);
    stack = get_trimmed_stack_trace();
    if (!stack)
        return append(mesg, " at %s line %d\n", file, line);
    if (verbose || flags & CLUCK) {
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

static void vcarp_at_loc (CarpFlags   flags,
                          const char *file,
                          const char *func,
                          int         line,
                          int         errnum,
                          const char *fmt,
                          va_list     args)
{
    char *mesg = vscarp_at_loc(flags, file, func, line, errnum, fmt, args);
    output(mesg);
    free(mesg);
    if (flags & DIE)
        exit(255);
}

#define DEFINE_CARP_FUNC(name, intermsof, flags)               \
    void name (const char *file,                               \
               const char *func,                               \
               int         line,                               \
               int         errnum,                             \
               const char *fmt, ...)                           \
    {                                                          \
        va_list args;                                          \
        va_start(args, fmt);                                   \
        intermsof(flags, file, func, line, errnum, fmt, args); \
        va_end(args);                                          \
    }

#define DEFINE_SCARP_FUNC(name, intermsof, flags)                     \
    char *name (const char *file,                                     \
                const char *func,                                     \
                int         line,                                     \
                int         errnum,                                   \
                const char *fmt, ...)                                 \
    {                                                                 \
        char *mesg;                                                   \
        va_list args;                                                 \
        va_start(args, fmt);                                          \
        mesg = intermsof(flags, file, func, line, errnum, fmt, args); \
        va_end(args);                                                 \
        return mesg;                                                  \
    }

DEFINE_CARP_FUNC   (warn_at_loc,     vwarn_at_loc,  0)
DEFINE_CARP_FUNC   (die_at_loc,      vwarn_at_loc,  DIE)
DEFINE_CARP_FUNC   (carp_at_loc,     vcarp_at_loc,  0)
DEFINE_CARP_FUNC   (croak_at_loc,    vcarp_at_loc,  DIE)
DEFINE_CARP_FUNC   (cluck_at_loc,    vcarp_at_loc,  CLUCK)
DEFINE_CARP_FUNC   (confess_at_loc,  vcarp_at_loc,  CLUCK | DIE)
DEFINE_SCARP_FUNC  (swarn_at_loc,    vswarn_at_loc, 0)
DEFINE_SCARP_FUNC  (sdie_at_loc,     vswarn_at_loc, DIE)
DEFINE_SCARP_FUNC  (scarp_at_loc,    vscarp_at_loc, 0)
DEFINE_SCARP_FUNC  (scroak_at_loc,   vscarp_at_loc, DIE)
DEFINE_SCARP_FUNC  (scluck_at_loc,   vscarp_at_loc, CLUCK)
DEFINE_SCARP_FUNC  (sconfess_at_loc, vscarp_at_loc, CLUCK | DIE)

