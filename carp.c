#include "carp.h"
#include "carppriv.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

static char *vscarp_at_loc ();

typedef enum {
    CLUCK = 1 << 0,
    DIE   = 1 << 1
} CarpFlags;

static char *
vcarp_message (CarpFlags flags, int errnum, const char *fmt, va_list args) {
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

static char *
vswarn_at_loc (CarpFlags flags, const char *file, const char *func, int line,
               int errnum, const char *fmt, va_list args)
{
    char *mesg;
    init();
    if (verbose)
        return vscarp_at_loc(flags, file, func, line, errnum, fmt, args);
    mesg = vcarp_message(flags, errnum, fmt, args);
    mesg = append(mesg, " at %s line %d\n", strip(file), line);
    return mesg;
}

static void
vwarn_at_loc (CarpFlags flags, const char *file, const char *func, int line,
              int errnum, const char *fmt, va_list args)
{
    char *mesg = vswarn_at_loc(flags, file, func, line, errnum, fmt, args);
    output(mesg);
    free(mesg);
    if (flags & DIE)
        exit(255);
}

/* Returns the part of the library file name that is important, doesn't modify
or allocate anything so it will find its location and how long it is.  */
static const char *
canonical_lib (const char *f, int *n) {
    const char *fs = nstrip(f, 0, 1);
    *n = 0;
    if (!fs)
        return NULL;
    if (!mystrncmp(fs, "lib", 3))
        fs += 3;
    if (strstr(fs, "."))
        *n = strstr(fs, ".") - fs;
    return fs;
}

/* Compares the name of one lib with another. So, carp would match
/usr/lib/libcarp.so.1.3, Keeping in mind that the name of the library is
the full path on linux, but a name on windows, so long and short names can go
both ways. And it is preferable that the user specify a short name in
trusted_libs. */
static int
libcmp (const char *a, const char *b) {
    int an, bn;
    const char *as = canonical_lib(a, &an);
    const char *bs = canonical_lib(b, &bn);
    return mystrncmp(as, bs, MAX(an, bn));
}

static FuncInfo *
get_suspect (List *stack) {
    List *p;
    FuncInfo *occurs = stack->data;
    for (p = stack; p; p = p->next) {
        FuncInfo *f = p->data;
        if (ne(f->lib, occurs->lib) &&
            !list_find(trusted_libs, f->lib, libcmp))
            return f;
    }
    /* Without a suspect, default to where the error occured  */
    return occurs;
}

static char *
vscarp_at_loc (CarpFlags flags, const char *file, const char *func, int line,
               int errnum, const char *fmt, va_list args)
{
    List *stack;
    char *mesg;
    init();
    if (muzzled)
        return vswarn_at_loc(flags, file, func, line, errnum, fmt, args);
    mesg = vcarp_message(flags, errnum, fmt, args);
    stack = get_trimmed_stack_trace(dump_stack);
    if (!stack)
        return append(mesg, " at %s line %d\n", strip(file), line);
    if (verbose || flags & CLUCK) {
        List *cur, *prev;
        for (prev = NULL, cur = stack; cur; prev = cur, cur = cur->next) {
            FuncInfo *prevf = prev ? prev->data : NULL;
            FuncInfo *curf = cur->data;
            if (prevf) {
                mesg = append(mesg, "    %s()", prevf->func);
                if (curf->file || !prev || curf->lib)
                    mesg = append(mesg, " called");
            }
            if (curf->file)
                mesg = append(mesg, " at %s line %d", strip(curf->file),
                                                      curf->line);
            else if (!prev)
                mesg = append(mesg, " at %s line %d", strip(file), line);
            else if (curf->lib)
                mesg = append(mesg, " from %s", curf->lib);
            mesg = append(mesg, "\n");
        }
    }
    else {
        FuncInfo *suspect = get_suspect(stack);
        if (!suspect || !suspect->file)
            mesg = append(mesg, " at %s line %d\n", strip(file), line);
        else
            mesg = append(mesg, " at %s line %d\n", strip(suspect->file),
                                                    suspect->line);
    }
    list_free(stack, func_info_free);
    return mesg;
}

static void
vcarp_at_loc (CarpFlags flags, const char *file, const char *func, int line,
              int errnum, const char *fmt, va_list args)
{
    char *mesg = vscarp_at_loc(flags, file, func, line, errnum, fmt, args);
    output(mesg);
    free(mesg);
    if (flags & DIE)
        exit(255);
}

#define DEFINE_CARP_FUNC(name, intermsof, flags)                    \
    void                                                            \
    name (const char *file, const char *func, int line, int errnum, \
          const char *fmt, ...)                                     \
    {                                                               \
        va_list args;                                               \
        va_start(args, fmt);                                        \
        intermsof(flags, file, func, line, errnum, fmt, args);      \
        va_end(args);                                               \
    }

#define DEFINE_SCARP_FUNC(name, intermsof, flags)                     \
    char *                                                            \
    name (const char *file, const char *func, int line, int errnum,   \
          const char *fmt, ...)                                       \
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

