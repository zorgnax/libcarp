#ifndef __CARPPRIV_H__
#define __CARPPRIV_H__

/* list  */
typedef struct _List List;

struct _List {
    void *data;
    List *next;
};

List *list_push (List *list, void *data);
void list_free (List *list, void (*func) ());

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

/* trace  */
List *get_stack_trace ();

#endif

