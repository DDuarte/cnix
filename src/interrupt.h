#ifndef INTERRUPT_H_
#define INTERRUPT_H_

/** @defgroup interrupt interrupt
 * @{
 * Functions for managing system interrupts
 */

#define NUM_OF_INTERRUPTS 32 ///< Max number of interrupts

void int_init(void); ///< Needs to be called before subscribing any interrupt


/**
 * @brief Subscribes a system interrupt
 *
 * @param irq_line IRQ
 * @param policy Subscribe policy
 * @param callback Function called when this interrupt is received
 * @return Less than 0 if an error occured, otherwise it's the value assigned to this subscribe
 *          received
 */
int  int_subscribe(int irq_line, int policy, void (*callback)());

/**
 * @brief Unsubscribes a system interrupt
 *
 * @param bit Value returned by int_subscribe
 * @return 0 if operation is successful, non-0 otherwise
 */
int  int_unsubscribe(int bit);

/**
 * @brief Blocks the program and waits for interrupts
 *
 * @return 0 if operation is successful, non-0 otherwise
 */
int  int_start_handler(void);

void int_stop_handler(void); ///< Stop handling interrupts

void _int_reset_interrupt(int bit); ///< (Internal function, do not use) Clears assigned callback

void int_enable_system(void); ///< Enable system interrupts
void int_disable_system(void); ///< Disable system interrupts

/**@}*/

#endif /* INTERRUPT_H_ */
