#ifndef INTERRUPT_H_
#define INTERRUPT_H_

int  int_subscribe(int irq_line, int policy, void (*callback)());
int  int_unsubscribe(int bit);
int  int_start_handler();
void int_stop_handler();
#endif /* LAB4_H_ */