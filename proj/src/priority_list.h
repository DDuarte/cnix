#ifndef PRIORITY_LIST_H_
#define PRIORITY_LIST_H_

typedef struct node {
    void* val;
    struct node* next;
    struct node* previous;
    unsigned int priority;
} priority_list_node_t;

typedef struct {
    priority_list_node_t* root;
    priority_list_node_t* last;
    unsigned int size;
    unsigned int (*cmp)(void*, void*);
    void* (*cpy)(void*);
    void (*destroy)(void*);
} priority_list_t;

priority_list_t* priority_list_new(unsigned int (*cmp)(void*,void*), void* (*cpy)(void*), void (*destroy)(void*));
unsigned int priority_list_add_elem(priority_list_t* me, void* newElem, unsigned int priority);
priority_list_node_t* priority_list_find(priority_list_t* me, void* data);
priority_list_node_t* priority_list_remove(priority_list_t* me, priority_list_node_t* elem);
void priority_list_remove_all(priority_list_t* me);

#endif
