#include <stdio.h>
#include <stdlib.h>
#include <minix/drivers.h>

#include "utilities.h"
#include "video_gr.h"
#include "timer.h"
/* Variables */

static struct {
    char* video_mem;
    unsigned short video_mode;
} myApplication;

/* Functions */

void parse_args();
int init();
void draw();

/* Callbacks */

void AppClose(event_t* ev);

/* Impementation */

int main(int argc, char const *argv[])
{
    int error;
    
    parse_args(argc, argv);
    
    if (init() != 0) {
        return 1;
    }
    
    draw();

    int_start_handler();

    if (error = vg_exit())
    {
        printf("vg_exit failed with error code %i.\n", error);
        return 1;
    }

    return 0;
}

void parse_args(int argc, char const *argv[]) {
    myApplication.video_mode = 0x117;
    if (argc == 2) {
        unsigned long temp_mode;
        temp_mode = parse_ulong(argv[1], 16);
        if (temp_mode != ULONG_MAX)
            myApplication.video_mode = (unsigned short)temp_mode;
    }
}

int init() {
    sef_startup();
    
    /* initialize graphics card in video mode */
    myApplication.video_mem = vg_init(myApplication.video_mode);
    if (!myApplication.video_mem)
    {
        printf("main: vg_init failed.\n");
        return 1;
    }
    
    int_init();
    
    timer_init();
    
    timer_add_event_s(5, AppClose);
    
    return 0;
}

void draw() {
    /* background */
    vg_fill(vg_color_rgb(255, 255, 255));
    
    /* menu bar background */
    vg_draw_rectangle(0, 0, 1024, 30, vg_color_rgb(90, 90, 90));
    
    /* borders */
    vg_draw_rectangle(0, 763, 1024, 768,    vg_color_rgb(90, 90, 90));
    vg_draw_rectangle(0, 30, 5, 763,        vg_color_rgb(90, 90, 90));
    vg_draw_rectangle(1019, 30, 1024, 763,  vg_color_rgb(90, 90, 90));
    
    /* close button, cross */
    vg_draw_rectangle(994, 5, 1014, 25, vg_color_rgb(230, 0, 0));
    vg_draw_line(997, 8, 1011, 22, vg_color_rgb(255, 255, 255));
    vg_draw_line(1011, 8, 997, 22, vg_color_rgb(255, 255, 255));
    
    /* must be last line */
    vg_swap_buffer();
}

void AppClose(event_t* ev) {
    int_stop_handler();
}