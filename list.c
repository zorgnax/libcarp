#include "carppriv.h"
#include <stdlib.h>

List *
list_push (List *list, void *data) {
    List *new_list, *last;
    new_list = (List *) calloc(sizeof (List), 1);
    new_list->data = data;
    if (!list)
        return new_list;
    for (last = list; last->next; last = last->next)
        ;
    last->next = new_list;
    return list;
}

void
list_free (List *list, void (*func) ()) {
    List *item, *next;
    for (item = list; item; item = next) {
        next = item->next;
        if (func)
            func(item->data);
        free(item);
    }
}

