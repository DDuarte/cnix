#ifndef WINDOW_H
#define WINDOW_H

#define TAB_COUNT 11

#include "tab.h"

typedef struct {
    char* title; /* window title bar */

    int width; /* screen total width */
    int height; /* screen total height */

    unsigned short video_mode; /* video mode */
    int redraw; /* 1 if window_draw needs to be called */
    int done; /* 1 if program should be stopped */
    int draw; /* 1 if video mode should be enabled */

    int mouse_x;
    int mouse_y;

    int current_tab;
    tab_t* tabs[TAB_COUNT];

    char* date;

} window_t;

int window_init(window_t* window);
int window_destroy(window_t* window);
int window_update(window_t* window /* ... */);
int window_draw(window_t* window);
int window_set_title(window_t* window, const char* format, ...);
int window_set_size(window_t* window, int width, int height);
int window_install_mouse(window_t* window);
int window_uninstall_mouse(window_t* window);
int window_key_press(window_t* window, KEY key);
int window_mouse_press(window_t* window);

#endif /* WINDOW_H */
