#include <minix/syslib.h>
#include <minix/drivers.h>

#include "i8254.h"

int timer_set_square(unsigned long timer, unsigned long freq) {
    /* change Timer timer to mode 3 */
    if (timer > 2)
        return 1;

    sys_outb(TIMER_CTRL, TIMER_BIN | TIMER_SQR_WAVE | TIMER_LSB_MSB | timer << 6);

    sys_outb(TIMER_0, (TIMER_FREQ / freq) & 0xFF); /* LSB */
    sys_outb(TIMER_0, (TIMER_FREQ / freq) >> 8); /* MSB */

    return 0;
}

int timer_subscribe_int(void ) {

    return 1;
}

int timer_unsubscribe_int() {

    return 1;
}

void timer_int_handler() {

}

int timer_test_square(unsigned long freq) {
    timer_set_square(0, freq);
    return 1;
}

int timer_test_int(unsigned long time) {

    return 1;
}
