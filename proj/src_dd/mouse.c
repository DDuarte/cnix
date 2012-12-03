#include "mouse.h"

#include <stdio.h>
#include <minix/syslib.h>

int mouse_write(unsigned char command) {

    if (write_kbc(CMD_REG, WRITE_BYTE_TO_MOUSE) != 0) {/* Write Byte to Mouse */
        printf("mouse_write: write_kbc CMD failed.\n");
        return 1;
    }

    if (write_kbc(DATA_REG, command) != 0) {
        printf("mouse_write: write_kbc DATA failed.\n");
        return 1;
    }

    return 0;
}

int mouse_read(unsigned long* value) {
    return sys_inb(DATA_REG, value);
}

int mouse_enable_stream_mode() {

    unsigned long res;

    if (mouse_write(ENABLE_SEND_PACKETS) != 0) {
        printf("mouse_enable_stream_mode: mouse_write failed.\n");
        return 1;
    }

    if (mouse_read(&res) != 0) {
        printf("mouse_enable_stream_mode: mouse_read failed.\n");
        return 1;
    }

    /*if (res != ACK) {
        printf("mouse_enable_stream_mode: did not receive acknowledge byte (0x%X), exiting\n.", res);
        return 1;
    }*/

    return 0;
}
