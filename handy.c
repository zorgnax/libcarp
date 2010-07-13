#include "carppriv.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

/* A strcmp that handles NULL values */
int
mystrcmp (const char *a, const char *b) {
    return a == b ? 0 : !a ? -1 : !b ? 1 : strcmp(a, b);
}

/* A strncmp that handles NULL values */
int
mystrncmp (const char *a, const char *b, size_t n) {
    if (!n)
        return mystrcmp(a, b);
    return a == b ? 0 : !a ? -1 : !b ? 1 : strncmp(a, b, n);
}

int
getintenv (const char *var) {
    char *val = getenv(var);
    return val ? atoi(val) : 0;
}

/* Appends a formatted string to a string. The result must be freed.  */
char *
vappend (char *str, const char *fmt, va_list args) {
    int str_size = str ? strlen(str) : 0;
    int size = vsnprintf(NULL, 0, fmt, args) + str_size + 1;
    str = realloc(str, size);
    vsprintf(str + str_size, fmt, args);
    return str;
}

char *
append (char *str, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    str = vappend(str, fmt, args);
    va_end(args);
    return str;
}

