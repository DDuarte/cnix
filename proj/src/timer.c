#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/bitmap.h>

#include "i8254.h"
#include "priority_list.h"
#include "interrupt.h"
#include "timer.h"




static int interrupt = -1;


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

int timer_set_square(unsigned long timer, unsigned long freq) {
    /* change Timer timer to mode 3 */
    if (timer > 2)
        return 1;

    sys_outb(TIMER_CTRL, TIMER_BIN | TIMER_SQR_WAVE | TIMER_LSB_MSB | (timer << 6));

    sys_outb(timer + TIMER_0, (TIMER_FREQ / freq) &  0xFF); /* LSB */
    sys_outb(timer + TIMER_0, (TIMER_FREQ / freq) >> 8); /* MSB */

    return 0;
}

int timer_init(void) {    
    _events = priority_list_new(NULL, event_copy, event_destroy);
    
    timer_set_square(0, 60);
    
    ticks = 0;
    
    interrupt = int_subscribe(TIMER0_IRQ, IRQ_REENABLE, timer_int_handler);
    if (interrupt == -1)
        return interrupt;
    
	return interrupt;
}

int timer_terminate() {

    int_unsubscribe(interrupt);
    priority_list_remove_all(_events);
    interrupt = -1;
	return 0;
}

void timer_int_handler() {

    priority_list_node_t* elem;
    
    ticks++;
    
    elem = _events->root;
    
    while(elem != NULL){
        event_t* event = (event_t*)(elem->val);
        if (event->due_ticks == ticks) {
            //printf("DEBUG: %s, %d, ticks: %d - ", __FILE__, __LINE__, ticks);
            if (!event->callback(event)) {
                timer_terminate();
                return;
            }
            if (!(event->recursive)) {
                elem = priority_list_remove(_events, elem);
                continue;
            } else {
                event_reset(event);
            }
        }
        elem = elem->next;
    }
}

int _timer_add_event(unsigned int dur_f, int (*callback)(event_t*), unsigned int priority, unsigned int recursive) {
    
    event_t event = { dur_f + ticks, dur_f, recursive, callback };
    
    if (!priority_list_add_elem(_events, (void*)(&event), priority))
        return 1;
    
    return 0;
}

int _timer_add_event_s(unsigned int dur_s, int (*callback)(event_t*), unsigned int priority, unsigned int recursive) {
    return _timer_add_event(dur_s * 60, callback, priority, recursive);
}

void event_reset(event_t* me) {
    me->due_ticks = me->duration + ticks;
}

int timer_num_events() {
    priority_list_node_t* elem = _events->root;
    int num = 0;
    
    while (elem != NULL) {
        event_t* event = (event_t*)(elem->val);
        if (event->due_ticks == ticks)
            num++;
        elem = elem->next;
    }
    
    return num;
}