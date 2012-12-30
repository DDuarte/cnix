#include "keyboard.h"
#include "kbc.h"
#include "interrupt.h"
#include "utilities.h"

#include <ctype.h>
#include <minix/drivers.h>
#include <minix/bitmap.h>

static unsigned int bit;
static KEY_MOD modifiers = KEY_MOD_NONE;

static void keyboard_handler(void);

char key_to_char(unsigned int scancode) { LOG

    int up = 0;
    int shift = 0;
    //int alt = 0;
    int altgr = 0;
    //int ctrl = 0;
    char res = 0;

    // return 0 with invalid scancodes

    if (scancode == -1)
        return res;

    if (scancode > LAST_KEY)
        return res;

    // detect modifier keys

    if (modifiers & KEY_MOD_CAPS)
        up ^= 1; // toggle

    if (modifiers & (KEY_MOD_L_SHIFT | KEY_MOD_R_SHIFT)) {
        up ^= 1; // toggle
        shift = 1;
    }

    if (modifiers & KEY_MOD_ALT_GR) {
        altgr = 1;
    }

    //if (modifiers & (KEY_MOD_L_CTRL | KEY_MOD_R_CTRL)) {
    //    ctrl = 1;
    //}

    //if (modifiers & KEY_MOD_ALT) {
    //    alt = 1;
    //}

    // get the actual character based on the modifiers

    if (altgr)
        res = key_c[scancode][KEY_IDX_ALTGR];
    else if (up || shift) {
        if (isalpha((int)(key_c[scancode][KEY_IDX_NONE]))) {
            if (up)
                res = key_c[scancode][KEY_IDX_UP];
            else if (shift)
                res = key_c[scancode][KEY_IDX_SHT];
        } else {
            if (shift)
                res = key_c[scancode][KEY_IDX_SHT];
            else if (up)
                res = key_c[scancode][KEY_IDX_UP];
        }
    }
    else
        res = key_c[scancode][KEY_IDX_NONE];

    return res;
}

int keyboard_install(void) { LOG

    bit = int_subscribe(KB_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &keyboard_handler);
    if (bit == -1) {
        printf("keyboard_install: int_subscribe failed.\n");
        return -1;
    }

    last_key_pressed = -1;

    return 0;
}

static void keyboard_handler(void) { LOG
    unsigned long long scancode = read_kbc();

    if (scancode == -1)
        return;

    if (scancode == 0xE0) /* 2 byte scancode */
        scancode = (scancode << 8) | read_kbc();
    else if (scancode == 0xE1) { /* 6 byte scancode (only pause key?) */
        int i;
        for (i = 0; i < 5; ++i)
            scancode = (scancode << 8) | read_kbc();
        last_key_pressed = -1;
        return;
    }

    if (scancode & 0x80) { /* MSB means release code */

        switch (scancode) {
            case KEY_RELEASE(KEY_L_SHIFT): modifiers &= ~KEY_MOD_L_SHIFT; break;
            case KEY_RELEASE(KEY_R_SHIFT): modifiers &= ~KEY_MOD_R_SHIFT; break;
            case KEY_RELEASE(KEY_L_CTRL):  modifiers &= ~KEY_MOD_L_CTRL;  break;
            case KEY_RELEASE(KEY_R_CTRL):  modifiers &= ~KEY_MOD_R_CTRL;  break;
            case KEY_RELEASE(KEY_ALT):     modifiers &= ~KEY_MOD_ALT;     break;
            case KEY_RELEASE(KEY_ALT_GR):  modifiers &= ~KEY_MOD_ALT_GR;  break;
        }

    } else {

        switch (scancode) {
            case KEY_PRESS(KEY_L_SHIFT): modifiers |= KEY_MOD_L_SHIFT; break;
            case KEY_PRESS(KEY_R_SHIFT): modifiers |= KEY_MOD_R_SHIFT; break;
            case KEY_PRESS(KEY_L_CTRL):  modifiers |= KEY_MOD_L_CTRL;  break;
            case KEY_PRESS(KEY_R_CTRL):  modifiers |= KEY_MOD_R_CTRL;  break;
            case KEY_PRESS(KEY_ALT):     modifiers |= KEY_MOD_ALT;     break;
            case KEY_PRESS(KEY_ALT_GR):  modifiers |= KEY_MOD_ALT_GR;  break;
            case KEY_PRESS(KEY_CAPS):    modifiers ^= KEY_MOD_CAPS;    break; /* toggle */
        }

        last_key_pressed = scancode;
    }

    if (scancode == KEY_RELEASE(KEY_ESC)) /* ESC release */
        int_stop_handler();

    printf("DEBUG: Keyboard scancode : 0x%X\n", scancode);
}

int keyboard_destroy(void) { LOG
    int error;

    error = int_unsubscribe(bit);
    if (error != 0) {
        printf("keyboard_destroy: int_unsubscribe failed with error code %d.\n", error);
        return -1;
    }

    return 0;
}
