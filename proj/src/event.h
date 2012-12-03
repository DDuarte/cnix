#ifndef EVENT_H
#define EVENT_H

enum {
    EVENT_KEY_DOWN,
    EVENT_KEY_UP,

    EVENT_MOUSE_MOVE,
    EVENT_MOUSE_BUTTON_DOWN,
    EVENT_MOUSE_BUTTON_UP,

    EVENT_TIMER
};

typedef struct {

    int keycode;
    char key;

} keyboard_event_t;

typedef struct {

    int diffx;
    int diffy;
    int button;

} mouse_event_t;

typedef struct {
    /* TODO */
} timer_event_t;

typedef union {

    int type;

    keyboard_event_t keyboard;
    mouse_event_t mouse;
    timer_event_t timer;

} event_t;

#endif
