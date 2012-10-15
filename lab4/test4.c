#include "test4.h"
#include "interrupt.h"
#include "kbc.h"
#include "timer.h"

#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <stdio.h>

static int counter = 0;
static unsigned short scancode;

void keyboard_handler(void) {

    scancode = read_kbc();
    
    if (scancode == 0xE0)
        scancode = (scancode << 8) | read_kbc();
    
    if (scancode & 0x80) /* MSB means release code */
        printf("Breakcode: 0x%X", scancode);
    else
        printf("Makecode: 0x%X", scancode);
    printf("\n");

    if (scancode == 0x81) /* ESC release */
        int_stop_handler();
}

int test_scan(void) {
    int bit, res;

    int_init();
    bit = int_subscribe(KB_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &keyboard_handler);
    res = int_start_handler();
    int_unsubscribe(bit);

    return res;
}


static unsigned short* leds_s;
static unsigned short n_s;
static int bitmask = 0;

void timer_handler(void) {
    int i;

    if (counter == (n_s * 60))
        int_stop_handler();

    if (counter % 60 == 0) /* 1 second */
    {
        i = counter / 60;
        bitmask ^= BIT(leds_s[i]);

        printf("%d - %d\n", leds_s[i], bitmask);
        write_kbc(DATA_REG, LEDS_SWITCH);
        write_kbc(DATA_REG, bitmask);
    }

    counter++;
}

int test_leds(unsigned short n, unsigned short* leds) {
    int bit, res;

    /* timer_set_square(0, 60); /* 1 interrupt every 1/60 seconds */

    leds_s = leds;
    n_s = n;

    int_init();
    bit = int_subscribe(TIMER0_IRQ, IRQ_REENABLE, &timer_handler);
    res = int_start_handler();
    int_unsubscribe(bit);

    return res;
}
