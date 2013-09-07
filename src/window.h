#ifndef WINDOW_H
#define WINDOW_H

/** @defgroup window window
 * @{
 * Functions to manage a window, see window_t
 */

#define TAB_COUNT 12 ///< Number of available tabs
#define TAB_CONSOLE TAB_COUNT - 1 ///< Number of the tab assigned to console
#include "tab.h"

/// Window state
typedef enum {
    WIN_STATE_NORMAL,
    WIN_STATE_OPEN_ASK_NAME,
    WIN_STATE_SAVE_ASK_NAME,
    WIN_STATE_OPEN_FILE,
    WIN_STATE_SAVE_FILE
} window_state_t;

/// Represents window with multiple tabs, buttons, title, etc.
typedef struct {
    char* title; ///< window title bar
    char* log_message; ///< log message in status bar
    int width; ///< screen total width
    int height; ///< screen total height

    unsigned short video_mode; ///< video mode
    int redraw; ///< 1 if window_draw needs to be called
    int done; ///< 1 if program should be stopped
    int draw; ///< 1 if video mode should be enabled

    int mouse_x; ///< Absolute mouse position X
    int mouse_y; ///< Absolute mouse position Y

    int prev_current_tab; ///< Previous tab used
    int current_tab; ///< Tab currently being used
    tab_t* tabs[TAB_COUNT]; ///< Stores all tabs

    char* date; ///< Time and date from RTC

    window_state_t state; ///< State of window

} window_t;


int window_init(window_t* window); ///< Initializes a window
int window_destroy(window_t* window); ///< De-allocates whatever the window required
int window_update(window_t* window /* ... */); ///< Updates window and all its "children" elements (tabs, buttons, etc)
int window_draw(window_t* window); ///< Function called to draw window and all its "children" elements (tabs, buttons, etc)
int window_set_title(window_t* window, const char* format, ...); ///< Changes title of window, printf style
int window_set_log_message(window_t* window, const char* format, ...); ///< Changes log message, printf style
int window_set_size(window_t* window, int width, int height); ///< Sets size of window
int window_install_mouse(window_t* window); ///< Installs mouse
int window_uninstall_mouse(window_t* window); ///< Unnistalls mouse
int window_key_press(window_t* window, KEY key); ///< Key has been pressed
int window_mouse_press(window_t* window); ///< Mouse key has been pressed
int window_mouse_release(window_t* window); ///< Mouse key has been released
int window_remove_tab(window_t* window, int tab); ///< Destroy given tab

/**@}*/

#endif /* WINDOW_H */
