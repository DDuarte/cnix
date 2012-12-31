#include "button.h"
#include "utilities.h"
#include <stdlib.h>

button_t* new_button(unsigned int locx, unsigned int locy, unsigned int szx, unsigned int szy, void(*drfunc)(button_t*), void (*clcallback)(button_t*), int enabled, window_t* wnd) { LOG
    button_t* newbtn = (button_t*)malloc(sizeof(button_t));

    newbtn->location_x = locx;
    newbtn->location_y = locy;
    newbtn->size_x = szx;
    newbtn->size_y = szy;
    newbtn->draw = drfunc;
    newbtn->click_call = clcallback;
    newbtn->enabled = enabled;
    newbtn->selected = 0;
    newbtn->mouse_st = MOUSE_NONE;
    newbtn->prev_mouse_left = 0;
    newbtn->wnd = wnd;

    return newbtn;
}

void button_update(button_t* btn, unsigned long mouse_x, unsigned long mouse_y, int mouse_left) { LOG
    btn->selected = 0;
    if (mouse_x >= btn->location_x && mouse_x <= btn->location_x + btn->size_x)
        if (mouse_y >= btn->location_y && mouse_y <= btn->location_y + btn->size_y)
            btn->selected = 1;

    switch (btn->mouse_st) {
        case MOUSE_NONE:
            if (btn->selected) {
                btn->mouse_st = MOUSE_OVER_CLICK;
            }
            break;
        case MOUSE_OVER:
            if (btn->selected) {
                if (mouse_left && !btn->prev_mouse_left) {
                    btn->mouse_st = MOUSE_OVER_CLICK;
                }
            } else
                btn->mouse_st = MOUSE_NONE;
            break;
        case MOUSE_OVER_CLICK:
            if (!mouse_left  && btn->prev_mouse_left ) {
                btn->mouse_st = MOUSE_OVER;
                if (btn->click_call && btn->selected) {
                    btn->click_call(btn);
                } else if (!btn->selected) {
                    btn->mouse_st = MOUSE_NONE;
                }
            }

            break;
    }

    btn->prev_mouse_left = mouse_left;
}
