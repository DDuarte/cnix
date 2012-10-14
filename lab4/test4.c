#include "test4.h"
#include "interrupt.h"
#include "kbc.h"
#include "timer.h"

#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>

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
static unsigned short c_s;

void timer_handler(void)
{
    if (c_s == n_s)
        int_stop_handler();

    printf("%d\n", leds_s[c_s++]);
}

int test_leds(unsigned short n, unsigned short* leds) {
    
    timer_set_square(0, 60);
    
    leds_s = leds;
    n_s = n;
    c_s = 0;

    int_init();
    int bit = int_subscribe(TIMER0_IRQ, IRQ_REENABLE, &timer_handler);
    int res = int_start_handler();

    int_unsubscribe(bit);
}
