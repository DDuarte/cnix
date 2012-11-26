#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <minix/drivers.h>

//#define NGRAPHICS /* disables graphic mode */

#include "utilities.h"
#include "video_gr.h"
#include "timer.h"
#include "interrupt.h"
/* Variables */


#define EXECUTION_TIME 20
static struct {
    char* video_mem;
    unsigned short video_mode;
    unsigned int x;
    unsigned int redraw;
} myApplication;

/* Functions */

void parse_args();
int init();

/* Callbacks */

int AppClose(event_t* ev);
int draw(event_t * dummie);
int update(event_t * dummie);

/* Impementation */

int main(int argc, char const *argv[])
{
    int error;

    time_t start;
    struct tm * ptm;

    parse_args(argc, argv);

    error = init();

    if (error) {
        printf("main: init failed with error code %i.\n", error);
        return error;
    }

    time(&start);
    error = int_start_handler();
    if (error) {
        printf("main: int_start_handler failed with error code %i.\n", error);
        return error;
    }
    start = time(NULL)-start;

    #ifndef NGRAPHICS
    error = vg_exit();
    if (error)
    {
        printf("main: vg_exit failed with error code %i.\n", error);
        return error;
    }
    #endif

    ptm = gmtime(&start);

    printf("%2d:%02d:%02d\n", ptm->tm_hour, ptm->tm_min, ptm->tm_sec );

    return 0;
}

void parse_args(int argc, char const *argv[]) {
    myApplication.video_mode = 0x117; /* default */
    if (argc == 2) {
        unsigned long temp_mode;
        temp_mode = parse_ulong(argv[1], 16);
        if (temp_mode != ULONG_MAX)
            myApplication.video_mode = (unsigned short)temp_mode;
    }
}

int init() {
    int error;

    sef_startup();

    #ifndef NGRAPHICS
    /* initialize graphics card in video mode */
    myApplication.video_mem = vg_init(myApplication.video_mode);
    if (!myApplication.video_mem)
    {
        printf("init: vg_init failed.\n");
        return 1;
    }
    #endif

    int_init();

    error = timer_init();

    if (error < 0) {
        printf("init: timer_init failed with error code %i.\n", error);
        return error;
    }

    timer_add_event_r(1, draw, 90001);
    timer_add_event_s(EXECUTION_TIME, AppClose, 0);
    timer_add_event_r(1, update, 9000);


    myApplication.x = 10;
    myApplication.redraw = 0;

    return 0;
}

int draw(event_t * dummie) {
    int numev = timer_num_events();

    //printf("%d ", numev);

    if (myApplication.redraw && (numev == 2)) {
        //printf("draw %d", myApplication.redraw);

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

        //vg_draw_circle(100, 100, myApplication.x, vg_color_rgb(0,0,0));

        /* must be last line */
        vg_swap_buffer();
        myApplication.redraw = 0;
    }

    return 1;
}

int AppClose(event_t* ev) {
    //printf("AppClose");
    int_stop_handler();
    return 0;
}

int update(event_t * dummie) {
    //printf("update");
    myApplication.x++;
    myApplication.redraw = 1;
    return 1;
}
