#include "test4.h"
#include "interrupt.h"
#include "kbc.h"

#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>

static int counter = 0;

void keyboard_handler(void) {

    int scan_code = read_kbc();

    if (scan_code == 1) /* ESC */
        int_stop_handler();
    if (scan_code & 0x8) /* MSB */
        printf("Release 0x%2x", scan_code);
    else
        printf("Press: 0x%2x", scan_code);

    printf("\n");
}

int test_scan(void) {
    int bit = int_subscribe(KB_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &keyboard_handler);
    int res = int_start_handler();
    int_unsubscribe(bit);

    return res;
}

int test_leds(unsigned short n, unsigned short* leds) {
    /* To be completed */
}
