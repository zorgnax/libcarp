#ifndef __LIST_H__
#define __LIST_H__

typedef struct _List List;

struct _List {
    void *data;
    List *next;
};

List *list_push (List *list, void *data);
void list_free (List *list, void (*func) ());

#endif
