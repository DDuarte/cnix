#ifndef MOUSE_H_
#define MOUSE_H_

/** @defgroup mouse mouse
 * @{
 * Constants and functions for managing mouse (higher level layer over kbc)
 */

#define WRITE_BYTE_TO_MOUSE 0xD4 ///< Write byte to mouse command
#define ENABLE_SEND_PACKETS 0xF4 ///< Enable send packets command
#define GET_MOUSE_STATUS 0xE9 ///< Get mouse status command

int mouse_enable_stream_mode(); ///< Enables stream mode
int mouse_write(unsigned char command); ///< Write byte to mouse
int mouse_read(unsigned long* value); ///< Read byte from mouse

/// Mouse state
typedef struct {
    int ldown;
    int mdown;
    int rdown;
    int diffx;
    int diffy;
    int ovfx;
    int ovfy;
    int updated;
} mouse_state_t;

/// Mouse status (config)
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
mouse_state_t mouse_state; ///< Global mouse state
mouse_status_t mouse_status; ///< Global mouse status

/**@}*/

#endif // MOUSE_H_
