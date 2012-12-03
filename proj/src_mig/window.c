#include "window.h"
#include "video_gr.h"
#include "interrupt.h"
#include "timer.h"
#include "mouse.h"
#include "kbc.h"
#include "keyboard.h"
#include "utilities.h"

#include <minix/drivers.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static int mouse_interrupt;
void mouseCallback(void);

int window_init(window_t* window) {

    int error;

    if (window->draw) {
        /* initialize video */
        if (!vg_init(window->video_mode)) {
            printf("window_init: vg_init failed.\n");
            return -1;
        }
    }

    error = vg_init_FreeType();
    if (error) {
        printf("window_init: vg_init_FreeType failed with error code %d.\n", error);
        return error;
    }

    error = window_set_title(window, "cnix %s", "0.1");
    if (error) {
        printf("window_init: window_set_title failed with error code %d.\n", error);
        return error;
    }

    error = window_set_size(window, vg_get_h_res(), vg_get_v_res());
    if (error) {
        printf("window_init: window_set_size failed with error code %d.\n", error);
        return error;
    }

    window->redraw = 0;
    window->done = 0;

    /* initialize interrupt handlers */
    int_init();

    /* TODO: add keyboard, etc */

    error = window_install_mouse(window);
    if (error) {
        printf("window_init: window_install_mouse failed with error code %d.\n", error);
        return error;
    }
    
    keyboard_install();

    return 0;
}

int window_destroy(window_t* window) {

    int error;

    error = window_uninstall_mouse(window);
    if (error) {
        printf("window_destroy: window_uninstall_mouse failed with error code %d.\n", error);
        return error;
    }

    /* exit video mode */
    if (window->draw) {
        error = vg_exit();
        if (error) {
            printf("window_destroy: vg_exit failed with error code %d.\n", error);
            return error;
        }
    }

    return 0;
}

int window_update(window_t* window /* ... */) {

    static int a = 0;

    a++;

    if (mouse_state.up) {
        mouse_state.up = 0;
        window->redraw = 1;
        window->mouse_x += mouse_state.diffx;
        window->mouse_y += mouse_state.diffy;

        window->mouse_x = clamp(window->mouse_x, 0, window->width);
        window->mouse_y = clamp(window->mouse_y, 0, window->height);

        window_set_title(window, "x: %d, y: %d, w: %d, h: %d",
        window->mouse_x,
        window->mouse_y,
        window->width,
        window->height);
    }


    if (a == 600)
        window->done = 1;

    return 0;
}

int window_draw(window_t* window) {
    char buff[20];
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
    vg_draw_line(997, 8, 1011, 22,      vg_color_rgb(255, 255, 255));
    vg_draw_line(1011, 8, 997, 22,      vg_color_rgb(255, 255, 255));

    /* window title */
    if (window->title)
        vg_draw_string(window->title, 32, 5, 25, vg_color_rgb(0, 0, 0));

    /* draw mouse */
    vg_draw_circle(window->mouse_x, window->mouse_y, 5, vg_color_rgb(0, 0, 0));
    
    /* write key */
    sprintf(buff, "%lu", last_key_pressed);
        vg_draw_string(buff, 32, 50, 100, vg_color_rgb(0, 0, 0));
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
    return 0;

}

int window_install_mouse(window_t* window) {

    int error;

    mouse_state.up = 0;
    window->mouse_x = 0;
    window->mouse_y = 0;

    error = mouse_enable_stream_mode();
    if (error) {
        printf("window_install_mouse: mouse_enable_stream_mode failed with error code %d.\n", error);
        return error;
    }

    mouse_interrupt = int_subscribe(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouseCallback);

    return 0;
}

int window_uninstall_mouse(window_t* window) {
    int error;
    unsigned long temp;

    error = int_unsubscribe(mouse_interrupt);
    if (error) {
        printf("window_uninstall_mouse: int_unsubscribe failed with error code %d.\n", error);
        return error;
    }

    error = mouse_read(&temp); /* clear buffer */
    if (error) {
        printf("window_uninstall_mouse: mouse_read failed with error code %d.\n", error);
        return error;
    }

    return 0;
}

void mouseCallback(void) {
    //static mouse_state_m_t mouseState = MSI;
    static unsigned char packet[3] = { 0, 0, 0 };
    static short counter = 0;

    unsigned long datatemp;

    if (mouse_read(&datatemp) != 0) {
        printf("mouseCallback: mouse_read failed.\n");
    }

    packet[counter] = datatemp;

    if (counter == 0 && !bit_isset(packet[0], 3))
        return;

    counter = (counter + 1) % 3;

    if (counter != 0)
        return;

    mouse_state.ldown = !!bit_isset(packet[0], 0); // LB
    mouse_state.mdown = !!bit_isset(packet[0], 2); // MB
    mouse_state.rdown = !!bit_isset(packet[0], 1); // RB
    mouse_state.ovfx  = !!bit_isset(packet[0], 6); // XOV
    mouse_state.ovfy  = !!bit_isset(packet[0], 7); // YOV

    mouse_state.diffx = !bit_isset(packet[0], 4) ? packet[1] : (char)(packet[1]);
    mouse_state.diffy = !bit_isset(packet[0], 5) ? packet[2] : (char)(packet[2]);


/*
    mouse_state.diffx = packet[1];
    mouse_state.diffy = packet[2];

    if (bit_isset(packet[0], 4))
        mouse_state.diffx = -(256 - mouse_state.diffx);

    if (bit_isset(packet[0], 5))
        mouse_state.diffy = -(256 - mouse_state.diffy);

    mouse_state.diffy = -mouse_state.diffy; /* verify this */

    //switch (mouseState) {
    //    case MSI:
    //        if (mouse_state.ldown)
    //            mouseState = MSR;
    //        break;
    //    case MSR:
    //        if (!mouse_state.ldown || !mouse_state.rdown || mouse_state.mdown)
    //            mouseState = MSI;
    //        else
    //            mouseState = MSD;
    //        break;
    //    //case MSD:
    //    //    int_stop_handler();
    //    //    break;
    //}

    printf("B1=0x%02X B2=0x%02X B3=0x%02X LB=%d MB=%d RB=%d XOV=%d YOV=%d X=%04d Y=%04d\n",
        packet[0], // B1
        packet[1], // B2
        packet[2], // B3
        mouse_state.ldown, // LB
        mouse_state.mdown, // MB
        mouse_state.rdown, // RB
        mouse_state.ovfx, // XOV
        mouse_state.ovfy, // YOV
        mouse_state.diffx, // X
        mouse_state.diffy); // Y

    mouse_state.up = 1;
}
