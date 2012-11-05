#ifndef MOUSE_H_
#define MOUSE_H_

#define WRITE_BYTE_TO_MOUSE 0xD4
#define ENABLE_SEND_PACKETS 0xF4
#define GET_MOUSE_STATUS 0xE9

int mouse_enable_stream_mode();
int mouse_write(unsigned char command);
int mouse_read(unsigned long* value);

typedef struct {
    int ldown;
    int mdown;
    int rdown;
    int diffx;
    int diffy;
    int ovfx;
    int ovfy;
} mouse_state_t;

typedef struct {
    int mode;
    int enable;
    int scaling;
    int ldown;
    int mdown;
    int rdown;
    int resolution;
    int rate;
} mouse_status_t; // config

/* globals */
mouse_state_t mouse_state;
mouse_status_t mouse_status;

#endif // MOUSE_H_