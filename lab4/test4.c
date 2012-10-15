#include "test4.h"
#include "interrupt.h"
#include "kbc.h"
#include "timer.h"

#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <stdio.h>

static int counter = 0;
static unsigned char scancode;

void keyboard_handler(void) {

    scancode = read_kbc();

    if (scancode == 0x81) /* ESC release */
        int_stop_handler();

    if (scancode & 0x8) /* MSB means release code */
        printf("Breakcode 0x%2x", scancode);
    else
        printf("Makecode: 0x%2x", scancode);

    printf("\n");
}

int test_scan(void) {
    int_init();
    int bit = int_subscribe(KB_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &keyboard_handler);
    int res = int_start_handler();
    int_unsubscribe(bit);

    return res;
}

static unsigned short* leds_s;
static unsigned short n_s;
static int bitmask = 0;

void timer_handler(void)
{
    if (counter == (n_s * 60))
        int_stop_handler();

    if (counter % 60 == 0)
    {
        bitmask ^= BIT(leds_s[counter / 60]);

        printf("%d - %d\n", leds_s[counter / 60], bitmask);
        write_kbc(DATA_REG, 0xED);
        write_kbc(DATA_REG, bitmask);
    }

    counter++;
}

int test_leds(unsigned short n, unsigned short* leds) {
    
    timer_set_square(0, 60);

    leds_s = leds;
    n_s = n;

    int_init();
    int bit = int_subscribe(TIMER0_IRQ, IRQ_REENABLE, &timer_handler);
    int res = int_start_handler();

    int_unsubscribe(bit);
}
