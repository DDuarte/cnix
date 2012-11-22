#ifndef __TIMER_H
#define __TIMER_H
#include "priority_list.h"
#define timer_add_event_r(dur_f, callback, priority) _timer_add_event((dur_f),(callback), priority, 1)
#define timer_add_event(dur_f, callback, priority) _timer_add_event((dur_f),(callback), priority, 0)
#define timer_add_event_s_r(dur_f, callback, priority) _timer_add_event_s((dur_f),(callback), priority, 1)
#define timer_add_event_s(dur_f, callback, priority) _timer_add_event_s((dur_f),(callback), priority, 0)

typedef struct _event{
    unsigned int due_ticks;
    unsigned int duration;
    unsigned int recursive;
    int (*callback)(struct _event*);
} event_t;

priority_list_t* _events;
unsigned int ticks;

/** @defgroup timer timer
 * @{
 *
 * Functions for using the i8254 timers
 */

/**
 * @brief Configures a timer to generate a square wave
 *
 * @param timer Timer to configure. (Ranges from 0 to 2)
 * @param freq Frequency of the square wave to generate
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_set_square(unsigned long timer, unsigned long freq);

/**
 * @brief Subscribes and enables Timer 0 interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int timer_init(void );

/**
 * @brief Unsubscribes Timer 0 interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_terminate();

int _timer_add_event(unsigned int dur_f, int (*callback)(event_t*), unsigned int priority, unsigned int recursive);
int _timer_add_event_s(unsigned int dur_s, int (*callback)(event_t*), unsigned int priority, unsigned int recursive);

void event_reset(event_t* me);

/**
 * @brief Timer 0 interrupt handler
 *
 * Increments counter
 */
void timer_int_handler();

int timer_num_events();



#endif /* __TIMER_H */
