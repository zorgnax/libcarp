#include <stdio.h>
#include <stdlib.h>
#include "funcinfo.h"

void func_info_print (FuncInfo *fi) {
    printf("fi: addr=%p, func=%s, args=%s\n"
           "    file=%s, line=%d, lib=%s\n",
           fi->addr,
           fi->func ? fi->func : "NULL",
           fi->args ? fi->args : "NULL",
           fi->file ? fi->file : "NULL",
           fi->line,
           fi->lib  ? fi->lib  : "NULL");
}

void func_info_free (FuncInfo *fi) {
    if (!fi)
        return;
    free(fi->func);
    free(fi->args);
    free(fi->file);
    free(fi->lib);
    free(fi);
}
