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
int             strip           = 0;
List           *trusted_files   = NULL;
List           *trusted_libs    = NULL;
CarpOutputFunc  output          = output_builtin;

void
init () {
    static int init = 0;
    if (init++)
        return;
    carp_set(
        "output",     getenv   ("CARP_OUTPUT"),
        "verbose",    getintenv("CARP_VERBOSE"),
        "muzzled",    getintenv("CARP_MUZZLED"),
        "dump-stack", getintenv("CARP_DUMP_STACK"),
        "strip",      getintenv("CARP_STRIP"),
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
void
carp_set (const char *key, ...) {
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

