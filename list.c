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
list_free (List *list, void (*freefunc) ()) {
    List *item, *next;
    for (item = list; item; item = next) {
        next = item->next;
        if (freefunc)
            freefunc(item->data);
        free(item);
    }
}

List *
list_remove (List *list, void *data, int (*cmpfunc) (), void (*freefunc) ()) {
    List *tmp, *prev;
    for (prev = NULL, tmp = list; tmp; prev = tmp, tmp = tmp->next) {
        if (cmpfunc ? cmpfunc(tmp->data, data) : (tmp->data != data))
            continue;
        if (prev)
            prev->next = tmp->next;
        else
            list = tmp->next;
        tmp->next = NULL;
        list_free(tmp, freefunc);
        tmp = prev;
        if (!tmp)
            break;
    }
    return list;
}

List *
list_find (List *list, void *data, int (*cmpfunc) ()) {
    List *item;
    for (item = list; item; item = item->next) {
        if (cmpfunc ? !cmpfunc(item->data, data) : (item->data == data))
            return item;
    }
    return NULL;
}

