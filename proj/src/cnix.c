#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <minix/drivers.h>

#include "utilities.h"
#include "video_gr.h"
#include "timer.h"
#include "interrupt.h"
#include "window.h"

static window_t _window;

void parse_args(int argc, char const *argv[]);

void timer_handler(void);

int main(int argc, char const *argv[])
{
    int error;
    sef_startup();

    parse_args(argc, argv);

    /* start video mode, subscribe interrupts, allocate mem, etc. */
    error = window_init(&_window);
    if (error) {
        printf("main: window_init failed with error code %d.\n", error);
        return error;
    }

    /* initialize timers */
    error = timer_init(timer_handler);
    if (error < 0) {
        printf("main: timer_init failed with error code %d.\n", error);
        return error;
    }

    /* start interrupt loop */
    error = int_start_handler();
    if (error) {
        printf("main: int_start_handler failed with error code %d.\n", error);
        return error;
    }

    /* stop video mode and free required memory */
    error = window_destroy(&_window);
    if (error) {
        printf("main: window_destroy failed with error code %d.\n", error);
        return error;
    }

    return 0;
}

void timer_handler(void) {
    window_update(&_window);

    if (_window.redraw) {
        _window.redraw = 0;
        window_draw(&_window);
        vg_swap_buffer();
    }

    if (_window.done) {
        int_stop_handler();
    }
}

void parse_args(int argc, char const *argv[]) {
    /* defaults, optional args */
    _window.video_mode = 0x117;
    _window.draw = 1;

    if (argc == 2) {
        unsigned long temp_mode;
        temp_mode = parse_ulong(argv[1], 16);
        if (temp_mode != ULONG_MAX) {
            if (temp_mode == 0)
                _window.draw = 0;
            else
                _window.video_mode = (unsigned short)temp_mode;
        }
    }
}
