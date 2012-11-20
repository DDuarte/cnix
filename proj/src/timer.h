#ifndef __TIMER_H
#define __TIMER_H



typedef struct _event{
    unsigned int time_left;
    unsigned int duration;
    void (*callback)(struct _event*);
} event_t;

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

int timer_add_event(unsigned int dur_f, void (*callback)(event_t*));
int timer_add_event_s(unsigned int dur_s, void (*callback)(event_t*));

void event_reset(event_t* me);

/**
 * @brief Timer 0 interrupt handler
 *
 * Increments counter
 */
void timer_int_handler();



#endif /* __TIMER_H */
