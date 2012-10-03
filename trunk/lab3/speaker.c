#include <minix/syslib.h>
#include <minix/drivers.h>

#include "i8254.h"

int speaker_ctrl(unsigned char on) {

    if (on == 1) {
        sys_outb(SPEAKER_CTRL, 0x3);
    } else {
        sys_outb(SPEAKER_CTRL, 0x0);
    }

    return 0;
}

int speaker_test(unsigned long freq, unsigned long time) {

    if (speaker_ctrl(1) != 0) {
        printf("lab3/speaker_test: speaker_ctrl(1) failed\n");
        return 1;
    }

    if (timer_set_square(2, freq) != 0) {
        printf("lab3/speaker_test: timer_set_square failed\n");
        return 1;
    }

    if (timer_test_int(time * 60) != 0) {
        printf("lab3/speaker_test: timer_test_int failed\n");
        return 1;
    }

    if (speaker_ctrl(0) != 0) {
        printf("lab3/speaker_test: speaker_ctrl(0) failed\n");
        return 1;
    }

    return 0;
}

