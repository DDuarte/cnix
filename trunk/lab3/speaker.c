#include <minix/syslib.h>
#include <minix/drivers.h>

#include "i8254.h"

int speaker_ctrl(unsigned char on) {

    if (on == TRUE) {
        if (sys_outb(SPEAKER_CTRL, 3) != 0) { /* first 2 bits enabled - 11*/
            printf("lab3/speaker_ctrl: sys_outb on failed\n");
            return 1;
        }
    } else {
        if (sys_outb(SPEAKER_CTRL, 0) != 0) { /* all bits disabled */
            printf("lab3/speaker_ctrl: sys_outb off failed\n");
            return 1;
        }
    }

    return 0;
}

int speaker_test(unsigned long freq, unsigned long time) {

    if (speaker_ctrl(TRUE) != 0) {
        printf("lab3/speaker_test: speaker_ctrl(TRUE) failed\n");
        return 1;
    }

    if (timer_set_square(2, freq) != 0) {
        printf("lab3/speaker_test: timer_set_square failed\n");
        return 1;
    }

    if (timer_test_int(time * 60) != 0) { /* in seconds */
        printf("lab3/speaker_test: timer_test_int failed\n");
        return 1;
    }

    if (speaker_ctrl(FALSE) != 0) {
        printf("lab3/speaker_test: speaker_ctrl(FALSE) failed\n");
        return 1;
    }

    return 0;
}
