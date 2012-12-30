#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/bitmap.h>

#include "i8254.h"
#include "interrupt.h"
#include "timer.h"
#include "utilities.h"

static int _interrupt = -1;

int timer_set_square(unsigned long timer, unsigned long freq) { LOG
    /* change Timer timer to mode 3 */
    if (timer > 2)
        return 1;

    sys_outb(TIMER_CTRL, TIMER_BIN | TIMER_SQR_WAVE | TIMER_LSB_MSB | (timer << 6));

    sys_outb(timer + TIMER_0, (TIMER_FREQ / freq) &  0xFF); /* LSB */
    sys_outb(timer + TIMER_0, (TIMER_FREQ / freq) >> 8); /* MSB */

    return 0;
}

int timer_init(void (*callback)()) { LOG
    /* timer_set_square(0, 60); */

    _interrupt = int_subscribe(TIMER0_IRQ, IRQ_REENABLE, callback ? callback : timer_int_handler);
	return _interrupt;
}

int timer_terminate(void) { LOG

    int_unsubscribe(_interrupt);
    _interrupt = -1;
	return 0;
}

void timer_int_handler(void) { LOG
    printf("timer_int_handler\n");
}
