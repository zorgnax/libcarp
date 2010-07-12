#include "carp.h"
#include "carppriv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
output_builtin (const char *mesg) {
    fputs(mesg, stderr);
}

int             verbose         = 0;
int             muzzled         = 0;
int             dump_stack      = 0;
int             strip_off       = 0;
int             strip_to        = 0;
List           *trusted_libs    = NULL;
CarpOutputFunc  output          = output_builtin;

void
init () {
    static int init = 0;
    if (init++)
        return;
    carp_set(
        "verbose",      getintenv("CARP_VERBOSE"),
        "muzzled",      getintenv("CARP_MUZZLED"),
        "dump-stack",   getintenv("CARP_DUMP_STACK"),
        "strip",        getintenv("CARP_STRIP"),
        "strip-to",     getintenv("CARP_STRIP_TO"),
        "trusted-libs", getenv   ("CARP_TRUSTED_LIBS"),
        NULL
    );
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
may be "default" to set the output back to the builtin output func.

"output-func" func
A CarpOutputFunc to output the error message.

"strip" int
The number of items to remove from the file names.
Defaults to environment var CARP_STRIP

"strip-to" int
The number of items to keep from the file names. If strip-to is 1,
foo/bar/baz.c becomes baz.c.
Defaults to environment var CARP_STRIP_TO

"suspected-libs" char*
Comma separated list of lib names to remove from the list of trusted libs.

"trusted-libs" char*
Comma separated list of lib names to be trusted.
Defaults to environment var CARP_TRUSTED_LIBS

*/
void
carp_set (const char *key, ...) {
    va_list args;
    init();
    va_start(args, key);
    for (; key; key = va_arg(args, const char *)) {
        if (eq(key, "verbose")) {
            verbose = va_arg(args, int);
            if (verbose)
                muzzled = 0;
        }
        else if (eq(key, "muzzled")) {
            muzzled = va_arg(args, int);
            if (muzzled)
                verbose = 0;
        }
        else if (eq(key, "dump-stack")) {
            dump_stack = va_arg(args, int);
        }
        else if (eq(key, "output")) {
            const char *value = va_arg(args, const char *);
            if (!value || eq(value, "default"))
                output = output_builtin;
            else
                croak("Unknown output builtin '%s'", value);
        }
        else if (eq(key, "output-func")) {
            output = va_arg(args, CarpOutputFunc);
        }
        else if (eq(key, "strip")) {
            strip_off = va_arg(args, int);
            strip_off = MAX(0, strip_off);
            if (strip_off)
                strip_to = 0;
        }
        else if (eq(key, "strip-to")) {
            strip_to = va_arg(args, int);
            strip_to = MAX(0, strip_to);
            if (strip_to)
                strip_off = 0;
        }
        else if (eq(key, "suspected-libs") || eq(key, "trusted-libs")) {
            char *libs = va_arg(args, char *);
            char *lib;
            if (!libs)
                continue;
            libs = strdup(libs);
            lib = strtok(libs, ",");
            while (lib) {
                if (eq(key, "trusted-libs"))
                    trusted_libs = list_push(trusted_libs, strdup(lib));
                else
                    trusted_libs = list_remove(trusted_libs, lib, strcmp, free);
                lib = strtok(NULL, ",");
            }
            free(libs);
        }
        else {
            croak("Unknown setting name '%s'", key);
        }
    }
    va_end(args);
}

/* Strips a file name to a specified number of path elements.  */
const char *
nstrip (const char *file, int off, int to) {
    const char *p;
    int n = 0;
    if (!file)
        return NULL;
    for (p = file; *p; p++) {
        if (*p == '/' || *p == '\\') {
            if (++n == off)
                return p + 1;
        }
    }
    for (p = file; *p; p++) {
        if (*p == '/' || *p == '\\') {
            if (--n + 1 == to)
                return p + 1;
        }
    }
    return file;
}

const char *
strip (const char *file) {
    return nstrip(file, strip_off, strip_to);
}
