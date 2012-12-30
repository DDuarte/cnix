#include "priority_list.h"
#include "utilities.h"

#include <stdlib.h>

static unsigned int defaultCmp(void* data1, void* data2) { return data1 == data2; }
static void* defaultCopy(void* data) { return data; }
static void  defaultDestroy(void* data) { }

priority_list_t* priority_list_new(unsigned int (*cmp)(void*,void*), void* (*cpy)(void*), void (*destroy)(void*)) { LOG
    priority_list_t* newlist = (priority_list_t*)malloc(sizeof(priority_list_t));
    if (!newlist)
        return NULL;

    newlist->root = NULL;
    newlist->last = NULL;
    newlist->size = 0;

    newlist->cmp = cmp ? cmp : defaultCmp;
    newlist->cpy = cpy ? cpy : defaultCopy;
    newlist->destroy = destroy ? destroy : defaultDestroy;

    return newlist;
}

unsigned int priority_list_add_elem(priority_list_t* me, void* val, unsigned int priority) { LOG
    priority_list_node_t* elem;

    if (!me || !val)
        return 1;

    elem = (priority_list_node_t*)malloc(sizeof(priority_list_node_t));

    if (!elem)
        return 1;

    elem->val = me->cpy(val);
    elem->next = NULL;

    if (me->root == NULL) {
        me->root = elem;
        elem->previous = NULL;
        me-> last = me->root;
        me->size += 1;
    } else {
        priority_list_node_t* obj = me->root;
        while(obj->next != NULL && obj->next->priority <= priority){
            obj = obj->next;
        }
        if (obj->priority > priority) {
            //inserir antes de obj
            elem->previous = obj->previous;
            if (elem->previous != NULL)
                elem->previous->next = elem;
            elem->next = obj;
            obj->previous = elem;
            if (elem->previous == NULL)
                me->root = elem;
        } else {

            //inserir a seguir de obj
            elem->previous = obj;
            elem->next = obj->next;
            if (elem->next != NULL)
                elem->next->previous = elem;
            obj->next = elem;
            if (elem->next == NULL)
                me->last = elem;
        }

        if (elem->next == NULL)
            me->last = elem;
        me->size += 1;
    }

    return 0;
}

priority_list_node_t* priority_list_find(priority_list_t* me, void* data) { LOG
    priority_list_node_t* obj;

    if (!me || !data)
        return NULL;

    obj = me->root;
    while (obj != NULL && me->cmp(obj->val, data)) { obj = obj->next; };

    return obj;
}

priority_list_node_t* priority_list_remove(priority_list_t* me, priority_list_node_t* elem) { LOG
    priority_list_node_t* result;

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

void priority_list_remove_all(priority_list_t* me) { LOG
    priority_list_node_t* obj = me->last;

    while (obj != NULL) {
        if (obj->previous != NULL) {
            obj = obj->previous;
            priority_list_remove(me, obj->next);
        } else {
            priority_list_remove(me, obj);
            obj = NULL;
        }
    }
}
