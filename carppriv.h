#ifndef __CARPPRIV_H__
#define __CARPPRIV_H__

#include "carp.h"
#include <stdarg.h>

/* handy  */
int   mystrcmp  (const char *a, const char *b);
int   mystrncmp (const char *a, const char *b, size_t n);
int   getintenv (const char *var);
char *vappend   (char *str, const char *fmt, va_list args);
char *append    (char *str, const char *fmt, ...);

#define eq(a, b) (!mystrcmp(a, b))
#define ne(a, b) (mystrcmp(a, b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

/* list  */
typedef struct _List List;

struct _List {
    void *data;
    List *next;
};

List *list_push   (List *list, void *data);
void  list_free   (List *list, void (*func) ());
List *list_remove (List *list, void *data, int (*cmpfunc) (),
                   void (*freefunc) ());
List *list_find   (List *list, void *data, int (*cmpfunc) ());

/* funcinfo  */
typedef struct {
    void *addr;
    char *func;
    char *args;
    char *file;
    int   line;
    char *lib;
} FuncInfo;

void func_info_print (FuncInfo *fi);
void func_info_free  (FuncInfo *fi);

/* config  */
extern int             verbose;
extern int             muzzled;
extern int             dump_stack;
extern int             strip_off;
extern int             strip_to;
extern List           *trusted_libs;
extern CarpOutputFunc  output;

void        init   ();
const char *strip  (const char *file);
const char *nstrip (const char *file, int off, int to);

/* trace  */
List *get_stack_trace         ();
void  dump                    (List *stack);
List *get_trimmed_stack_trace (int dump_stack);

#endif

