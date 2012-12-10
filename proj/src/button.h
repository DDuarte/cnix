#ifndef BUTTON_H_
#define BUTTON_H_

typedef enum {
    MOUSE_NONE,
    MOUSE_OVER,
    MOUSE_OVER_CLICK
} button_mouse_st;

typedef struct bt{
    unsigned int location_x;
    unsigned int location_y;
    unsigned int size_x;
    unsigned int size_y;
    void (*draw)(struct bt*);
    void (*click_call)(struct bt*);
    int enabled;
    int selected;
    button_mouse_st mouse_st;
    int prev_mouse_left;
} button_t;

button_t* new_button(unsigned int locx, unsigned int locy, unsigned int szx, unsigned int szy, void(*drfunc)(button_t*), void (*clcallback)(button_t*), int enabled);

void button_update(button_t* btn, unsigned long mouse_x, unsigned long mouse_y, int mouse_left);

#endif