#include "test4.h"
#include "interrupt.h"
#include "kbc.h"

#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>

#define KB_IRQ 0x09

static int counter = 0;

void keyboard_handler() {
    unsigned char scan_code = read_kbc();
    if (scan_code == 1)
        int_stop_handler();
    if (scan_code >> 7 == 1)
        printf("Releaase 0x%2x",scan_code);
    else
        printf("Press: 0x%2x",scan_code);
    printf("\n");
}

void timer_handler()
{
    counter++;
    if (counter == 60) int_stop_handler();
}

int test_scan(void) {
    /*int bit = int_subscribe(KB_IRQ, (IRQ_REENABLE | IRQ_EXCLUSIVE), &keyboard_handler);
    int res = int_start_handler();
    int_unsubscribe(bit);
    return res;*/
    int bit = int_subscribe(0, IRQ_REENABLE, &timer_handler);
    int res = int_start_handler();
    int_unsubscribe(bit);
}

int test_leds(unsigned short n, unsigned short *leds) {
    /* To be completed */
}
