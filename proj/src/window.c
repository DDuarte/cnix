#include "window.h"
#include "video_gr.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

int window_draw(window_t* window) {

    /* background */
    vg_fill(vg_color_rgb(255, 255, 255));

    /* window title bar */
    vg_draw_rectangle(0, 0, 1024, 30, vg_color_rgb(90, 90, 90));

    /* borders */
    vg_draw_rectangle(0, 763, 1024, 768,    vg_color_rgb(90, 90, 90));
    vg_draw_rectangle(0, 30, 5, 763,        vg_color_rgb(90, 90, 90));
    vg_draw_rectangle(1019, 30, 1024, 763,  vg_color_rgb(90, 90, 90));

    /* close button, cross */
    vg_draw_rectangle(994, 5, 1014, 25, vg_color_rgb(230, 0, 0));
    vg_draw_line(997, 8, 1011, 22, vg_color_rgb(255, 255, 255));
    vg_draw_line(1011, 8, 997, 22, vg_color_rgb(255, 255, 255));

    /* window title */
    if (window->title)
        vg_draw_string(window->title, 32, 5, 25, vg_color_rgb(0, 0, 0));

    return 0;
}

int window_set_title(window_t* window, const char* format, ...) {

    if (window->title) {
        free(window->title);
    }

    char buffer[256];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);

    window->title = (char*)malloc(strlen(buffer) + 1);
    strcpy(window->title, buffer);

    return 0;

}

int window_set_size(window_t* window, int width, int height) {

    window->width = width;
    window->height = height;

}
