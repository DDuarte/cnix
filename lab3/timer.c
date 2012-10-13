#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/bitmap.h>

#include "i8254.h"

typedef struct {
    int hook_id;
    int bit;
} interrupt_data;

typedef struct {
    interrupt_data data;
    unsigned long counter;
    unsigned long freq;
} timer;

static timer timer0;

int timer_set_square(unsigned long timer, unsigned long freq) {
    if (timer > 2)
    {
        printf("lab3/timer_set_square: timer (%u) needs to be in the range 0-2\n", timer);
        return 1;
    }

    if (sys_outb(TIMER_CTRL, TIMER_BIN | TIMER_SQR_WAVE | TIMER_LSB_MSB | (timer << 6)) != 0) {
        printf("lab3/timer_set_square: sys_outb (TIMER_CTRL) failed\n");
        return 1;
    }

    if (sys_outb(timer + TIMER_0, (TIMER_FREQ / freq) &  0xFF) != 0) { /* LSB */
        printf("lab3/timer_set_square: sys_outb (timer + TIMER_0 LSB) failed\n");
        return 1;
    }

    if (sys_outb(timer + TIMER_0, (TIMER_FREQ / freq) >> 8) != 0) { /* MSB */
        printf("lab3/timer_set_square: sys_outb (timer + TIMER_0 LSB) failed\n");
        return 1;
    }

    return 0;
}

int timer_subscribe_int(void ) {

    timer0.data.bit = 0;
    timer0.data.hook_id = timer0.data.bit;

    if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer0.data.hook_id) != 0) {
        printf("lab3/timer_subscribe_int: sys_irqsetpolicy failed\n");
        return -1;
    }

    if (sys_irqenable(&timer0.data.hook_id) != 0) {
        printf("lab3/timer_subscribe_int: sys_irqenable failed\n");
        return -1;
    }

    timer0.counter = 0;

    return timer0.data.bit;
}

int timer_unsubscribe_int() {

    if (sys_irqdisable(&timer0.data.hook_id) != 0) {
        printf("lab3/timer_unsubscribe_int: sys_irqdisable failed\n");
        return 1;
    }

    if (sys_irqrmpolicy(&timer0.data.hook_id) != 0) {
        printf("lab3/timer_unsubscribe_int: sys_irqrmpolicy failed\n");
        return 1;
    }

    return 0;
}

void timer_int_handler() {

    timer0.counter++;
    if (timer0.counter % 60 == 0)
        printf("lab3/timer_int_handler: counter %u\n", timer0.counter / 60); /* debug code */
}

int timer_test_square(unsigned long freq) {

    if (timer_set_square(0, freq) != 0) {
        printf("lab3/timer_test_square: timer_set_square failed\n");
        return 1;
    }

    return 0;
}

int timer_test_int(unsigned long time) {

    message msg;
    int ipc_status;
    int r;

    if (timer_subscribe_int() < 0) {
        printf("lab3/timer_test_int: timer_subscribe_int failed\n");
        return 1;
    }

    while (timer0.counter <= time * 60) {
        r = driver_receive(ANY, &msg, &ipc_status); /* Get a request message. */
        if (r != 0) {
            printf("lab3/timer_test_int: driver_receive failed with: %d", r);
            continue;
        }

        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: /* hardware interrupt notification */
                    if (bit_isset(msg.NOTIFY_ARG, timer0.data.bit)) { /* subscribed interrupt */
                        timer_int_handler();
                    }
                    break;
                default:
                    break; /* no other notifications expected: do nothing */
            }
        } else { /* received a standard message, not a notification */
            /* no standard messages expected: do nothing */
        }
    }

    if (timer_unsubscribe_int() != 0) {
        printf("lab3/timer_test_int: timer_unsubscribe_int failed\n");
        return 1;
    }

    return 0;
}
