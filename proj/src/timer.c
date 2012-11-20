#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/bitmap.h>

#include "i8254.h"
#include "list.h"
#include "interrupt.h"
#include "timer.h"



static list_t* _events;
static int interrupt = -1;

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
    
    _events = list_new(NULL);
    
    interrupt = int_subscribe(TIMER0_IRQ, IRQ_REENABLE, timer_int_handler);
    if (interrupt == -1)
        return interrupt;
    
	return interrupt;
}

int timer_terminate() {

    int_unsubscribe(interrupt);
    
    interrupt = -1;
    
	return 0;
}

void timer_int_handler() {
    list_node_t* elem = _events->root;
    
    while(elem != NULL) {
        ((event_t*)(elem->val))->time_left--;
        elem = elem->next;
    }
    
    elem = _events->root;
    
    while(elem != NULL){
        event_t* event = (event_t*)(elem->val);
        if (event->time_left == 0) {
            event->callback(event);
            if (event->time_left == 0) {
                elem = list_remove(_events, elem);
                continue;
            }
        }
        elem = elem->next;
    }
}

int timer_add_event(unsigned int dur_f, void (*callback)(event_t*)) {
    event_t* event = (event_t*)malloc(sizeof(event_t));
    
    if (!event)
        return 1;
    
    event->duration = dur_f;
    event->time_left = dur_f;
    event->callback = callback;
    
    if (!list_add_elem(_events, (void*)event))
        return 1;
        
    return 0;
}

int timer_add_event_s(unsigned int dur_s, void (*callback)(event_t*)) {
    return timer_add_event(dur_s * 60, callback);
}

void event_reset(event_t* me) {
    me->time_left = me->duration;
}