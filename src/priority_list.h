#ifndef PRIORITY_LIST_H_
#define PRIORITY_LIST_H_

/** @defgroup priority_list priority_list
 * @{
 * Functions to manage priority (queue) list ADT
 */

/// Priority list node
typedef struct node {
    void* val;
    struct node* next;
    struct node* previous;
    unsigned int priority;
} priority_list_node_t;

/// Priority list
typedef struct {
    priority_list_node_t* root;
    priority_list_node_t* last;
    unsigned int size;
    unsigned int (*cmp)(void*, void*);
    void* (*cpy)(void*);
    void (*destroy)(void*);
} priority_list_t;

priority_list_t* priority_list_new(unsigned int (*cmp)(void*,void*), void* (*cpy)(void*), void (*destroy)(void*)); ///< Create new list
unsigned int priority_list_add_elem(priority_list_t* me, void* newElem, unsigned int priority); ///< Add element to list (with priority)
priority_list_node_t* priority_list_find(priority_list_t* me, void* data); ///< Find element in list
priority_list_node_t* priority_list_remove(priority_list_t* me, priority_list_node_t* elem); ///< Remove element from list
void priority_list_remove_all(priority_list_t* me); ///< Remove all elements from list

/**@}*/

#endif
