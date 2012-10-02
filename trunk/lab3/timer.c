#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include "i8254.h"

/*#define BIT(n) (0x1 << (n))*/

static int timer_hook_id;


int timer_set_square(unsigned long timer, unsigned long freq) {
    /* change Timer timer to mode 3 */
    if (timer > 2)
        return 1;

    sys_outb(TIMER_CTRL, TIMER_BIN | TIMER_SQR_WAVE | TIMER_LSB_MSB | (timer << 6));

    sys_outb(TIMER_0, (TIMER_FREQ / freq) &  0xFF); /* LSB */
    sys_outb(TIMER_0, (TIMER_FREQ / freq) >> 8); /* MSB */

    return 0;
}

int timer_subscribe_int(void ) {

    sys_irqsetpolicy(TIMER0_IRQ, IRQ_ENABLE, &timer_hook_id);
    sys_irqenable(&timer_hook_id);
    
	return 0;
}

int timer_unsubscribe_int() {

    sys_irqdisable(&timer_hook_id);
    /*sys_irqmpolicy(&timer_hook_id);*/
    
	return 0;
}

void timer_int_handler() {
    printf("Hello!\n");
}

int timer_test_square(unsigned long freq) {
    timer_set_square(0, freq);
    return 0;
}

int timer_test_int(unsigned long time) {
    
    message msg;
    int ipc_status;
    int r, cnt = 0;
    
    timer_subscribe_int();
    
    while( cnt <= time ) {
        r =  driver_receive(ANY, &msg, &ipc_status); /* Get a request message. */
        if ( r != 0 ) { 
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: /* hardware interrupt notification */				
                    if (msg.NOTIFY_ARG & BIT(timer_hook_id)) { /* subscribed interrupt */
                        timer_int_handler();
                        cnt++;
                    }
                    break;
                default:
                    break; /* no other notifications expected: do nothing */	
            }
        } else { /* received a standard message, not a notification */
            /* no standard messages expected: do nothing */
        }
    }
    
    timer_unsubscribe_int();
    return 0;
}
