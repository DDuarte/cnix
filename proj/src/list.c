#include "list.h"

#include <stdlib.h>

list_t* list_new(unsigned int (*cmp)(void*,void*)) {
    list_t* newlist = (list_t*)malloc(sizeof(list_t));
    if (!newlist)
        return NULL;
        
    newlist->root = NULL;
    newlist->last = NULL;
    newlist->size = 0;
    newlist->cmp = cmp;
}

unsigned int list_add_elem(list_t* me, void* val) {
    list_node_t* elem; 
    
    if (!me || !val)
        return 1;
    
    elem = (list_node_t*)malloc(sizeof(list_node_t));
    
    if (!elem)
        return 1;
    
    elem->val = val;
    elem->next = NULL;
    
    if (me->root == NULL) {
        me->root = elem;
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
    
    if (!me || !data || !me->cmp)
        return NULL;
        
    obj = me->root;
    while (obj != NULL && (me->cmp(obj->val, data) == 0)) { obj = obj->next; };
    
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
    free(elem);
    
    return result;
}

