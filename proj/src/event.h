#ifndef EVENT_H_
#define EVENT_H_

typedef struct _event{
    unsigned int due_ticks;
    unsigned int duration;
    unsigned int recursive;
    int (*callback)(struct _event*);
} event_t;


void* event_copy(void* data);

void event_destroy(void* data);

void event_reset(event_t* me, unsigned int ticks);

#endif
