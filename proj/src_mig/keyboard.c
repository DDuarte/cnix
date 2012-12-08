#include "keyboard.h"
#include "kbc.h"
#include "interrupt.h"

#include <minix/drivers.h>
#include <minix/bitmap.h>

static unsigned int bit;

static void keyboard_handler(void);

int keyboard_install(void) {
    int res;
    memset(keys, 0, LAST_KEY * sizeof(unsigned char));
    bit = int_subscribe(KB_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &keyboard_handler);
    last_key_pressed = -1;
}

static void keyboard_handler(void) {
    unsigned long long scancode = read_kbc();
  
    if (scancode == -1) {
        return;
    }

    if (scancode == 0xE0) /* 2 byte scancode */
        scancode = (scancode << 8) | read_kbc();
        
    else if (scancode == 0xE1) { /* 6 byte scancode (only pause) */
        int i;
        for (i = 0; i < 5; ++i) {
            scancode = (scancode << 8) | read_kbc();
        }
        last_key_pressed = -1;
        return;
    }
    if (scancode & 0x80) /* MSB means release code */
        {
        //last_key_pressed = scancode;
        printf("%d\n", scancode);
    }
    else {
        
        last_key_pressed = scancode;
        printf("%d\n", scancode);
    }

    if (scancode == KEY_RELEASE(KEY_ESC)) /* ESC release */
        int_stop_handler();
}

int keyboard_destroy(void) {
    int_unsubscribe(bit);
}