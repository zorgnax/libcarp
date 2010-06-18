#ifndef __FUNCINFO_H__
#define __FUNCINFO_H__

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

#endif
