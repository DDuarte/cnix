#include "window.h"
#include "video_gr.h"
#include "interrupt.h"
#include "timer.h"
#include "mouse.h"
#include "kbc.h"
#include "utilities.h"
#include "keyboard.h"
#include "button.h"

#include <minix/drivers.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static int mouse_interrupt;
void mouseCallback(void);

button_t* close_btn;

void close_btn_draw(button_t* btn);
void close_btn_click(button_t* btn);
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

    error = window_install_mouse(window);
    if (error) {
        printf("window_init: window_install_mouse failed with error code %d.\n", error);
        return error;
    }

    error = keyboard_install();
    if (error) {
        printf("window_init_ keyboard_install failed with error code %d.\n", error);
        return error;
    }

    close_btn = new_button(994, 5, 20, 20, close_btn_draw, close_btn_click, 1);
    
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

    if (mouse_state.up) {
        mouse_state.up = 0;
        window->redraw = 1;
        window->mouse_x += mouse_state.diffx;
        window->mouse_y -= mouse_state.diffy;

        window->mouse_x = clamp(window->mouse_x, 0, window->width);
        window->mouse_y = clamp(window->mouse_y, 0, window->height);

        window_set_title(window, "x: %d, y: %d, w: %d, h: %d",
            window->mouse_x,
            window->mouse_y,
            window->width,
            window->height);
        
        button_update(close_btn, window->mouse_x, window->mouse_y, mouse_state.ldown);
    }

    window->redraw = 1;

    return 0;
}

int append(char* s, char c)
{
        int len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
        return len;
}

int deappend(char* s)
{
        int len = strlen(s);
        s[len - 1] = '\0';
        return len;
}

int window_draw(window_t* window) {

    static char buff[20][200];
    static unsigned int last_key = -1;
    static int curline = 0;

    static int initedBuff = 0;
    if (!initedBuff) {
        int i;
        for (i = 0; i < 20; ++i)
            memset(buff[i], 0, 200 * sizeof(char));
        initedBuff = 1;
    }

    /* background */
    vg_fill(vg_color_rgb(255, 255, 255));

    /* window title bar */
    vg_draw_rectangle(0, 0, 1024, 30, vg_color_rgb(90, 90, 90));

    /* borders */
    vg_draw_rectangle(0, 763, 1024, 768,    vg_color_rgb(90, 90, 90));
    vg_draw_rectangle(0, 30, 5, 763,        vg_color_rgb(90, 90, 90));
    vg_draw_rectangle(1019, 30, 1024, 763,  vg_color_rgb(90, 90, 90));

    close_btn->draw(close_btn);

    /* window title */
    if (window->title)
        vg_draw_string(window->title, 32, 5, 25, vg_color_rgb(0, 0, 0));
    
    /* new button */
    
    vg_draw_circle(879, 15, 10, vg_color_rgb(230, 0, 0));
    vg_draw_rectangle(871, 13, 887, 17, vg_color_rgb(255, 255, 255));
    vg_draw_rectangle(877, 7, 881, 23, vg_color_rgb(255, 255, 255));
    
    /* open button */
    vg_draw_rectangle(896, 5, 904, 8, vg_color_rgb(255, 201, 14));
    vg_draw_rectangle(894, 8, 914, 25, vg_color_rgb(255, 201, 14));
    vg_draw_line(894, 14, 914, 14, vg_color_rgb(100, 100, 100));

    /* Save Button */
    vg_draw_rectangle(919, 5, 939, 25, vg_color_rgb(50, 50, 50));
    vg_draw_rectangle(924, 5, 934, 11, vg_color_rgb(255, 255, 255));
    vg_draw_rectangle(925, 6, 926, 10, vg_color_rgb(0, 0, 0));
    vg_draw_circle(929, 18, 2, vg_color_rgb(200, 200, 200));
    vg_set_pixel(929, 18, vg_color_rgb(0, 0, 0));
    
    /* Run Button */
    vg_draw_rectangle(969, 5, 989, 25, vg_color_rgb(200, 100, 100));
    vg_draw_line(974, 10, 984, 15, vg_color_rgb(0, 0, 0));
    vg_draw_line(984, 15, 974, 20, vg_color_rgb(0, 0, 0));
    vg_draw_line(974, 10, 974, 20, vg_color_rgb(0, 0, 0));
    
    /* draw mouse */
    vg_draw_circle(window->mouse_x, window->mouse_y, 5, vg_color_rgb(0, 0, 0));

    /* write key */ // *NOTE*: this is test code, will be removed.
    if (last_key != last_key_pressed) {
        if (last_key_pressed > 0 && last_key_pressed < LAST_KEY) {

            if (last_key_pressed == KEY_ENTER)
                curline++;
            else {
                if (last_key_pressed == KEY_BKSP) {
                    int l = deappend(buff[curline]);

                    if (l == 1)
                        curline--;

                    if (curline < 0)
                        curline = 0;
                }
                else
                    append(buff[curline], key_to_char(last_key_pressed));
            }
        }
    }
    last_key = last_key_pressed;
    last_key_pressed = -1;

    int i;
    for (i = 0; i < 20; ++i)
        vg_draw_string(buff[i], 32, 50, 100 + i*25, vg_color_rgb(0, 0, 0));

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
    static unsigned char packet[] = { 0, 0, 0 };
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
    mouse_state.rdown = !!bit_isset(packet[0], 1); // RB
    mouse_state.mdown = !!bit_isset(packet[0], 2); // MB
    mouse_state.diffx = !bit_isset(packet[0], 4) ? packet[1] : (char)(packet[1]);
    mouse_state.diffy = !bit_isset(packet[0], 5) ? packet[2] : (char)(packet[2]);
    mouse_state.ovfx  = !!bit_isset(packet[0], 6); // XOV
    mouse_state.ovfy  = !!bit_isset(packet[0], 7); // YOV

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

void close_btn_draw(button_t* btn) {
    vg_draw_rectangle(btn->location_x, btn->location_y, btn->location_x + 20, btn->location_y + 20, vg_color_rgb(230, 0, 0));
    vg_draw_line(btn->location_x + 3, btn->location_y + 3, btn->location_x + 17, btn->location_y + 17, vg_color_rgb(255, 255, 255));
    vg_draw_line(btn->location_x + 17, btn->location_y + 3, btn->location_x + 3, btn->location_y + 17, vg_color_rgb(255, 255, 255));
}

void close_btn_click(button_t* btn) {
    int_stop_handler();
}
