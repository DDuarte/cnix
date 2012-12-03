#include "list.h"

#include <stdlib.h>

static unsigned int defaultCmp(void* data1, void* data2) { return data1 == data2; }
static void* defaultCopy(void* data) { return data; }
static void  defaultDestroy(void* data) { }

list_t* list_new(unsigned int (*cmp)(void*,void*), void* (*cpy)(void*), void (*destroy)(void*)) {
    list_t* newlist = (list_t*)malloc(sizeof(list_t));
    if (!newlist)
        return NULL;

    newlist->root = NULL;
    newlist->last = NULL;
    newlist->size = 0;

    if (cmp == NULL)
        newlist->cmp = defaultCmp;
    else
        newlist->cmp = cmp;

    if (cpy == NULL)
        newlist->cpy = defaultCopy;
    else
        newlist->cpy = cpy;

    if (destroy == NULL)
        newlist->destroy = defaultDestroy;
    else
        newlist->destroy = destroy;

    return newlist;
}

unsigned int list_add_elem(list_t* me, void* val) {
    list_node_t* elem;

    if (!me || !val)
        return 1;

    elem = (list_node_t*)malloc(sizeof(list_node_t));

    if (!elem)
        return 1;


    elem->val = me->cpy(val);
    elem->next = NULL;

    if (me->root == NULL) {
        me->root = elem;
        elem->previous = NULL;
        me->last = me->root;
        me->size += 1;
    } else {
        me->last->next = elem;
        elem->previous = me->last;
        me->last = me->last->next;
        me->size += 1;
    }

    return 0;
}

list_node_t* list_find(list_t* me, void* data) {
    list_node_t* obj;

    if (!me || !data)
        return NULL;

    obj = me->root;
    while (obj != NULL && me->cmp(obj->val, data)) { obj = obj->next; };

    return obj;
}

list_node_t* list_remove(list_t* me, list_node_t* elem) {
    list_node_t* result;

    if (!me || !elem)
        return NULL;

    if (elem->previous == NULL) {
        me->root = elem->next;
    } else {
        elem->previous->next = elem->next;
    }

    result = elem->next;

    me->size -= 1;
    me->destroy(elem->val);
    free(elem);

    return result;
}

void list_remove_all(list_t* me) {
    list_node_t* obj = me->last;

    while (obj != NULL) {
        me->destroy(obj->val);
        if (obj->previous != NULL) {
            obj = obj->previous;
            free(obj->next);
            list_remove(me, obj->next);
        } else {
            free(obj);
            obj = NULL;
        }
    }
}

