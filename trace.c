#include "carppriv.h"
#include <string.h>
#include <stdio.h>

void
dump (List *stack) {
    List *p;
    printf("---\n");
    for (p = stack; p; p = p->next) {
        func_info_print(p->data);
    }
    printf("...\n");
}

/* Trims off carp library specifics and elements past main. Outside of this
file a stack refers to a trimmed stack starting at the frame that through the
error (with carp, cluck whatever...) till the main frame (and no furthur)  */
List *
get_trimmed_stack_trace (int dump_stack) {
    List *stack, *p;
    FuncInfo *f;
    stack = get_stack_trace();
    if (!stack)
        return NULL;
    if (dump_stack)
        dump(stack);
    for (p = stack; p; p = p->next) {
        f = p->data;
        if (eq(f->func, "vscarp_at_loc") ||
            eq(f->func, "vswarn_at_loc"))
        {
            List *old_stack = stack;
            FuncInfo *nextf = p->next->data;
            if (eq(nextf->func, "vcarp_at_loc") ||
                eq(nextf->func, "vwarn_at_loc"))
            {
                stack = p->next->next->next;
                p->next->next->next = NULL;
            }
            else {
                stack = p->next->next;
                p->next->next = NULL;
            }
            list_free(old_stack, func_info_free);
            break;
        }
    }
    for (p = stack; p->next; p = p->next) {
        f = (FuncInfo *) p->data;
        if (eq(f->func, "main") || eq(f->func, "WinMain")) {
            list_free(p->next, func_info_free);
            p->next = NULL;
            break;
        }
    }
    return stack;
}

