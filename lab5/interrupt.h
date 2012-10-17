#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#define NUM_OF_INTERRUPTS 32

void int_init(void);
int  int_subscribe(int irq_line, int policy, void (*callback)());
int  int_unsubscribe(int bit);
int  int_start_handler(void);
void int_stop_handler(void);
void _int_resetInterrupt(int bit);

#endif /* INTERRUPT_H_ */
