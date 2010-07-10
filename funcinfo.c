#include "carppriv.h"
#include <stdio.h>
#include <stdlib.h>

void func_info_print (FuncInfo *fi) {
    printf("fi: func=%s, args=%s\n"
           "    file=%s, line=%d, lib=%s\n",
           fi->func,
           fi->args,
           fi->file,
           fi->line,
           fi->lib);
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
