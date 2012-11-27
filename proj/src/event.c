#include "event.h"

#include <stdlib.h>

void* event_copy(void* data) {
    event_t* newEvent = (event_t*)malloc(sizeof(event_t));

    event_t* oldEvent = (event_t*)data;

    if (!newEvent)
        return NULL;

    *newEvent = *oldEvent;

    return newEvent;
}

void event_destroy(void* data) {
    free((event_t*)data);
}

void event_reset(event_t* me, unsigned int ticks) {
    me->due_ticks = me->duration + ticks;
}
