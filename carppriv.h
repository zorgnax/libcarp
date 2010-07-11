#ifndef __CARPPRIV_H__
#define __CARPPRIV_H__

#include "carp.h"
#include <stdarg.h>

/* handy  */
int   mystrcmp  (const char *a, const char *b);
int   getintenv (const char *var);
char *vappend   (char *str, const char *fmt, va_list args);
char *append    (char *str, const char *fmt, ...);

#define eq(a, b) (!mystrcmp(a, b))
#define ne(a, b) (mystrcmp(a, b))

/* list  */
typedef struct _List List;

struct _List {
    void *data;
    List *next;
};

List *list_push (List *list, void *data);
void  list_free (List *list, void (*func) ());

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
extern int             strip;
extern List           *trusted_files;
extern List           *trusted_libs;
extern CarpOutputFunc  output;

void init ();

/* trace  */
List *get_stack_trace         ();
void  dump                    (List *stack);
List *get_trimmed_stack_trace (int dump_stack);

#endif

