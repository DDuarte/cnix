#ifndef LIST_H_
#define LIST_H_

typedef struct node {
    void* val;
     struct node* next;
     struct node* previous;
} list_node_t;

typedef struct {
    list_node_t* root;
    list_node_t* last;
    unsigned int size;
    unsigned int (*cmp)(void*, void*);
    void* (*cpy)(void*);
    void (*destroy)(void*);
} list_t;

list_t* list_new(unsigned int (*cmp)(void*,void*), void* (*cpy)(void*), void (*destroy)(void*));
unsigned int list_add_elem(list_t* me, void* newElem);
list_node_t* list_find(list_t* me, void* data);
list_node_t* list_remove(list_t* me, list_node_t* elem);
void list_remove_all(list_t* me);

#endif
