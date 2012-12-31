#ifndef WINDOW_H
#define WINDOW_H

#define TAB_COUNT 12
#define TAB_CONSOLE TAB_COUNT - 1
#include "tab.h"

typedef enum {
    WIN_STATE_NORMAL,
    WIN_STATE_OPEN_ASK_NAME,
    WIN_STATE_SAVE_ASK_NAME,
    WIN_STATE_OPEN_FILE,
    WIN_STATE_SAVE_FILE
} window_state_t;

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

    int prev_current_tab;
    int current_tab;
    tab_t* tabs[TAB_COUNT];

    char* date;
    
    window_state_t state;
    
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
int window_mouse_release(window_t* window);

#endif /* WINDOW_H */
