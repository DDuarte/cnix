#ifndef BUTTON_H_
#define BUTTON_H_

#include "window.h"

/** @defgroup button button
 * @{
 * Functions for using clickable buttons
 */

/// Button mouse state
typedef enum {
    MOUSE_NONE, ///< Button is not over nor being clicked by mouse
    MOUSE_OVER, ///< Mouse is over the button
    MOUSE_OVER_CLICK ///< Mouse is over the button and clicked
} button_mouse_st;

/// Represents a clickable button
typedef struct bt {
    unsigned int location_x; ///< Upper left corner X position
    unsigned int location_y; ///< Upper left corner Y position
    unsigned int size_x; ///< Width
    unsigned int size_y; ///< Height
    void (*draw)(struct bt*); ///< Function that draws the button
    void (*click_call)(struct bt*); ///< Function called when button is pressed
    int enabled; ///< 1 if button should work, 0 if button responds to no actions
    int selected; ///< 1 if button is mouse overed, 0 otherwise
    button_mouse_st mouse_st; ///< State of the button
    int prev_mouse_left; ///< Previous state of the button
    window_t* wnd; ///< Pointer to main window
} button_t;

/**
 * @brief Creates a new button (uses malloc)
 *
 * @param locx Upper left corner X position
 * @param locy Upper left corner Y position
 * @param szx Width
 * @param szy Height
 * @param drfunc Function that draws the button
 * @param clcallback Function called when button is pressed
 * @param enabled 1 if button should work, 0 if button responds to no actions
 * @param wnd Pointer to main window
 * @return Non NULL pointer to button_t in case of success
 */
button_t* new_button(unsigned int locx, unsigned int locy, unsigned int szx, unsigned int szy, void(*drfunc)(button_t*), void (*clcallback)(button_t*), int enabled, window_t* wnd);

/**
 * @brief Updates an existing button
 *
 * @param btn button to update
 * @param mouse_x X position of the mouse
 * @param mouse_y Y position of the mouse
 * @param mouse_left 1 if left mouse button is pressed
 */
void button_update(button_t* btn, unsigned long mouse_x, unsigned long mouse_y, int mouse_left);

/**@}*/

#endif
