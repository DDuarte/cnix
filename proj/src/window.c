#include "window.h"
#include "cnix.h"
#include "video_gr.h"
#include "interrupt.h"
#include "timer.h"
#include "mouse.h"
#include "kbc.h"
#include "utilities.h"
#include "keyboard.h"
#include "tab.h"
#include "button.h"
#include "rtc.h"
#include "file.h"

#include <minix/drivers.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define WIN_LOG(args) window_set_log_message(window, args)

static int mouse_interrupt;
void mouseCallback(void);

button_t* new_btn;
button_t* open_btn;
button_t* save_btn;
button_t* make_btn;
button_t* run_btn;
button_t* close_btn;

void new_btn_draw(button_t* btn);
void open_btn_draw(button_t* btn);
void save_btn_draw(button_t* btn);
void make_btn_draw(button_t* btn);
void run_btn_draw(button_t* btn);
void close_btn_draw(button_t* btn);

void new_btn_click(button_t* btn);
void open_btn_click(button_t* btn);
void save_btn_click(button_t* btn);
void make_btn_click(button_t* btn);
void run_btn_click(button_t* btn);
void close_btn_click(button_t* btn);

void window_set_tab(window_t* window, int tab) {
    if (tab < 0 || tab > TAB_CONSOLE) {
        window_set_log_message(window, "invalid tab");
    return;
    }
    
    if (!window->tabs[tab]) {
window_set_log_message(window, "Tab doesn't exist");     
     return;
    }
        
    window->prev_current_tab = window->current_tab;
    window->current_tab = tab;
}

void window_set_previous_tab(window_t* window) {
    int temp = window->prev_current_tab;
    window->prev_current_tab = window->current_tab;
    window->current_tab = temp;
}

int window_init(window_t* window) { LOG

    int error;
    int i;

    if (window->draw) {
        /* initialize video */
        if (!vg_init(window->video_mode)) {
            printf("window_init: vg_init failed.\n");
            return -1;
        }
    }

    window->redraw = 0;
    window->done = 0;

    /* init font system */
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
    
    error = window_set_log_message(window, "LOG_MESSAGE");
    if (error) {
        printf("window_init: window_set_log_message failed with error code %d.\n", error);
        return error;
    }

    error = window_set_size(window, window->draw ? vg_get_h_res() : 1024, window->draw ? vg_get_v_res() : 768);
    if (error) {
        printf("window_init: window_set_size failed with error code %d.\n", error);
        return error;
    }

    /* set up tabs */
    window->current_tab = -1;
    for (i = 0; i < TAB_COUNT; ++i)
        window->tabs[i] = NULL;

    //window->tabs[0]  = tab_create("#1");
    //window->tabs[1]  = tab_create("#2");
    //window->tabs[2]  = tab_create("#3");
    //window->tabs[3]  = tab_create("#4");
    //window->tabs[4]  = tab_create("#5");
    //window->tabs[5]  = tab_create("#6");
    //window->tabs[6]  = tab_create("#7");
    //window->tabs[7]  = tab_create("#8");
    //window->tabs[8]  = tab_create("#9");
    //window->tabs[9]  = tab_create("#10");
    //window->tabs[10] = tab_create("#11");
    window->tabs[11] = tab_create("#console");
    window->current_tab = TAB_CONSOLE;
    window->prev_current_tab = window->current_tab;

    window->date = NULL;

    /* initialize interrupt handlers */
    int_init();

    /* install mouse */
    error = window_install_mouse(window);
    if (error) {
        printf("window_init: window_install_mouse failed with error code %d.\n", error);
        return error;
    }
    printf("window_install: mouse installed with success.\n");

    /* install keyboard */
    error = keyboard_install();
    if (error) {
        printf("window_init: keyboard_install failed with error code %d.\n", error);
        return error;
    }
    printf("window_init: keyboard installed with success.\n");

    /* set up buttons */
    new_btn = new_button(869, 5, 20, 20, new_btn_draw, new_btn_click, 1, window);
    open_btn = new_button(894, 5, 20, 20, open_btn_draw, open_btn_click, 1, window);
    save_btn = new_button(919, 5, 20, 20, save_btn_draw, save_btn_click, 1, window);
    make_btn = new_button(944, 5, 20, 20, make_btn_draw, make_btn_click, 1, window);
    run_btn = new_button(969, 5, 20, 20, run_btn_draw, run_btn_click, 1, window);
    close_btn = new_button(994, 5, 20, 20, close_btn_draw, close_btn_click, 1, window);

    window->state = WIN_STATE_NORMAL;
    return 0;
}

int window_destroy(window_t* window) { LOG

    int error, i;

    /* remove mouse */
    error = window_uninstall_mouse(window);
    if (error) {
        printf("window_destroy: window_uninstall_mouse failed with error code %d.\n", error);
        return error;
    }

    /* remove keyboard */
    error = keyboard_destroy();
    if (error) {
        printf("window_destroy: keyboard_destroy failed with error code %d.\n", error);
        return error;
    }

    /* remove tabs */
    for (i = 0; i < TAB_COUNT; ++i) {
        if (window->tabs[i]) {
            error = tab_destroy(window->tabs[i]); 
            if (error) {
                printf("window_destroy: tab_destroy(%d) failed with error code %d.\n", i, error);
                return error;
            }
        }
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

int window_update(window_t* window) { LOG

    static unsigned int previous_key = -1;
    static mouse_state_t prev_mouse_state;
    int error;

    /* Mouse */
    if (mouse_state.updated) {
        mouse_state.updated = 0;

        window->mouse_x += mouse_state.diffx;
        window->mouse_y -= mouse_state.diffy; /* inverted y axis */

        window->mouse_x = clamp(window->mouse_x, 0, window->width);
        window->mouse_y = clamp(window->mouse_y, 0, window->height);

        error = window_set_title(window, "x: %d, y: %d, w: %d, h: %d",
            window->mouse_x,
            window->mouse_y,
            window->width,
            window->height);
        if (error) {
            printf("window_update: window_set_title failed with error code %d.\n", error);
            return error;
        }

        if (mouse_state.ldown || mouse_state.mdown || mouse_state.rdown)
            window_mouse_press(window);

        if ((prev_mouse_state.ldown && !mouse_state.ldown) ||
            (prev_mouse_state.mdown && !mouse_state.mdown) ||
            (prev_mouse_state.rdown && !mouse_state.rdown))
            window_mouse_release(window);

        window->redraw = 1;
        prev_mouse_state = mouse_state;
    }

    /* Keyboard */
    if (last_key_pressed != -1) {
        error = window_key_press(window, last_key_pressed);
        if (!error) {
            switch (window->state) {
                case WIN_STATE_OPEN_ASK_NAME: {
                    if (window->current_tab != TAB_CONSOLE) {
                        window->state = WIN_STATE_NORMAL;
                    }
                    else if (last_key_pressed == KEY_ENTER || last_key_pressed == KEY_NUM_ENTER) {
                        int i;
                        size_t size;
                        char* fileName = tab_to_file(window->tabs[TAB_CONSOLE]);
                        char* fileBuffer = NULL;
                        tab_t* tab = NULL;
                        
                        for (i = 0; i < (TAB_COUNT - 1); i++) {
                            if (!window->tabs[i]) {
                                tab = window->tabs[i];
                                break;
                            }
                        }

                        if (i >= (TAB_COUNT - 1)) {
                            printf("Open: No tabs available to open file!");
                            window->state = WIN_STATE_NORMAL;
                            break;
                        }

                        if (!File_Load(fileName, &fileBuffer, &size)) {
                            window->state = WIN_STATE_NORMAL;
                            break;
                        }

                        window->tabs[i] = tab_create_from_file(fileName, fileBuffer);
                        window->state = WIN_STATE_NORMAL;
                    }
                    break;
                }
                case WIN_STATE_SAVE_ASK_NAME: {
                    if (window->current_tab != TAB_CONSOLE) {
                        window->state = WIN_STATE_NORMAL;
                    } else if (last_key_pressed == KEY_ENTER || last_key_pressed == KEY_NUM_ENTER) {
                        char* fileName = tab_to_file(window->tabs[TAB_CONSOLE]);
                        char* fileBuffer = tab_to_file(window->tabs[window->prev_current_tab]);

                        if (strlen(fileName) != 0) {
                            File_Save(fileName, fileBuffer, strlen(fileBuffer));
                        }
                        window->state = WIN_STATE_NORMAL;
                    }
                    break;
                }
                default: break;
            }

            previous_key = last_key_pressed;
            last_key_pressed = -1;
            window->redraw = 1;
        } else {
            printf("window_update: window_key_press failed with error code %d.\n", error);
            last_key_pressed = -1;
            window->redraw = 1;
        }
    }

    /* RTC */
    char* date = rtc_get_date();
    if (date) {
        printf("DEBUG: %s, %d\n", __FUNCTION__, __LINE__);
        if (window->date)
            free(window->date);
        window->date = date;
        window->redraw = 1;
    }

    return 0;
}

int window_draw(window_t* window) { LOG
    int error, i;

    /* background */
    error = vg_fill(vg_color_rgb(255, 255, 255));
    if (error) {
        printf("window_draw: vg_fill failed with error code %d.\n", error);
        return error;
    }

    /* window title bar */
    error = vg_draw_rectangle(0, 0, 1024, 30, vg_color_rgb(90, 90, 90));
    if (error) {
        printf("window_draw: vg_draw_rectangle (1) failed with error code %d.\n", error);
        return error;
    }

    /* borders */
    error = vg_draw_rectangle(0, 763, 1024, 768, vg_color_rgb(90, 90, 90));
    if (error) {
        printf("window_draw: vg_draw_rectangle (2) failed with error code %d.\n", error);
        return error;
    }
    error = vg_draw_rectangle(0, 30, 5, 763, vg_color_rgb(90, 90, 90));
    if (error) {
        printf("window_draw: vg_draw_rectangle (3) failed with error code %d.\n", error);
        return error;
    }
    error = vg_draw_rectangle(1019, 30, 1024, 763, vg_color_rgb(90, 90, 90));
    if (error) {
        printf("window_draw: vg_draw_rectangle (4) failed with error code %d.\n", error);
        return error;
    }

    /* window title */
    if (window->title) {
        error = vg_draw_string(window->title, 32, 5, 25, vg_color_rgb(0, 0, 0));
        if (error) {
            printf("window_draw: vg_draw_string (1) failed with error code %d.\n", error);
            return error;
        }
    }

    new_btn->draw(new_btn); /* new button */
    open_btn->draw(open_btn); /* open button */
    save_btn->draw(save_btn); /* save button */
    make_btn->draw(make_btn); /* make button */
    run_btn->draw(run_btn); /* run button */
    close_btn->draw(close_btn); /* close button */

    /* draw tabs */
    for (i = 0; i < TAB_COUNT; ++i) {
        tab_t* tab = window->tabs[i];
        if (tab) {
            error = tab_draw(tab,
                             i, /* tab_num */
                             window->current_tab == i || (window->current_tab == TAB_CONSOLE && window->prev_current_tab == i), /* selected */
                             window->current_tab == i || i == TAB_CONSOLE || (window->current_tab == TAB_CONSOLE && window->prev_current_tab == i), /* drawText */
                             window->current_tab == i); /* HasFocus */
            if (error) {
                printf("window_draw: tab_draw failed with error code %d.\n", error);
                return error;
            }
        }
    }

    /* draw cmd tab*/
    error = vg_draw_rectangle(5, 733, 1019, 763, vg_color_rgb(90, 90, 90));
    if (error) {
        printf("window_draw: vg_draw_rectangle (5) failed with error code %d.\n", error);
        return error;
    }
    error = vg_draw_rectangle(5, 698, 1019, 703, vg_color_rgb(90, 90, 90));
    if (error) {
        printf("window_draw: vg_draw_rectangle (6) failed with error code %d.\n", error);
        return error;
    }

    /* draw time and date */
    error = vg_draw_string(window->date, 16, 5, 760, vg_color_rgb(0, 0, 0));
    if (error) {
        printf("window_draw: vg_draw_string (2) failed with error code %d.\n", error);
        return error;
    }
    
    /* window log message */
    if (window->log_message) {
        error = vg_draw_string(window->log_message, 16, 512, 760, vg_color_rgb(0, 0, 0));
        if (error) {
            printf("window_draw: vg_draw_string (1) failed with error code %d.\n", error);
            return error;
        }
    }

    /* draw mouse */
    error = vg_draw_circle(window->mouse_x, window->mouse_y, 5, vg_color_rgb(0, 0, 0));
    if (error) {
        printf("window_draw: vg_draw_circle failed with error code %d.\n", error);
        return error;
    }

    return 0;
}

int window_set_title(window_t* window, const char* format, ...) { LOG

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

int window_set_log_message(window_t* window, const char* format, ...) { LOG

    if (window->log_message) {
        free(window->log_message);
    }

    char buffer[256];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);

    window->log_message = (char*)malloc(strlen(buffer) + 1);
    strcpy(window->log_message, buffer);

    return 0;
}

int window_set_size(window_t* window, int width, int height) { LOG

    window->width = width;
    window->height = height;
    return 0;

}

int window_install_mouse(window_t* window) { LOG

    int error;

    mouse_state.updated = 0;
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

int window_uninstall_mouse(window_t* window) { LOG
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

void mouseCallback(void) { LOG
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

    mouse_state.updated = 1;
}

int window_key_press(window_t* window, KEY key) { LOG
    if (key >= KEY_F1 && key <= KEY_F10)
        window_set_tab(window, key - KEY_F1);
    else if (key == KEY_F11) /* F10 is 0x44 but F11 is not 0x45 */
        window_set_tab(window, TAB_COUNT - 2);
    else if (key == KEY_F12) {
        window_set_tab(window, TAB_CONSOLE);
    }

    if (window->current_tab == TAB_CONSOLE && (key == KEY_ENTER || key == KEY_NUM_ENTER))
        return 0;
    else
        return tab_key_press(window->tabs[window->current_tab], key);
}

int window_mouse_press(window_t* window) { LOG

    /* tab labels */
    if (window->mouse_y > 30 && window->mouse_y < 60)
        if (window->mouse_x > 5 && window->mouse_x < (window->width - 5))
            window_set_tab(window, (window->mouse_x  - 5) / 92); /* dividing by size of label */

    /* console box */
    if (window->mouse_y > 703 && window->mouse_y < 733)
        if (window->mouse_x > 5 && window->mouse_x < (window->width - 5))
            window_set_tab(window, TAB_CONSOLE);

    /* tab box */
    if (window->current_tab == TAB_CONSOLE)
        if (window->mouse_y > 5 && window->mouse_y < 698)
            if (window->mouse_x > 5 && window->mouse_x < (window->width - 5))
                window_set_previous_tab(window);

    button_update(new_btn, window->mouse_x, window->mouse_y, mouse_state.ldown);
    button_update(open_btn, window->mouse_x, window->mouse_y, mouse_state.ldown);
    button_update(save_btn, window->mouse_x, window->mouse_y, mouse_state.ldown);
    button_update(make_btn, window->mouse_x, window->mouse_y, mouse_state.ldown);
    button_update(run_btn, window->mouse_x, window->mouse_y, mouse_state.ldown);
    button_update(close_btn, window->mouse_x, window->mouse_y, mouse_state.ldown);

    printf("close_btn: %p\n", close_btn);

    return tab_mouse_press(window->tabs[window->current_tab], window->mouse_x, window->mouse_y);
}

int window_mouse_release(window_t* window) { LOG

    button_update(new_btn, window->mouse_x, window->mouse_y, mouse_state.ldown);
    button_update(open_btn, window->mouse_x, window->mouse_y, mouse_state.ldown);
    button_update(save_btn, window->mouse_x, window->mouse_y, mouse_state.ldown);
    button_update(make_btn, window->mouse_x, window->mouse_y, mouse_state.ldown);
    button_update(run_btn, window->mouse_x, window->mouse_y, mouse_state.ldown);
    button_update(close_btn, window->mouse_x, window->mouse_y, mouse_state.ldown);

    printf("close_btn: %p\n", close_btn);

    return 1;
}

void new_btn_draw(button_t* btn) { LOG
    vg_draw_circle(btn->location_x + 10, btn->location_y + 10, 10, vg_color_rgb(230, 0, 0));
    vg_draw_rectangle(btn->location_x + 3, btn->location_y + 8, btn->location_x + 17, btn->location_y + 12, vg_color_rgb(255, 255, 255));
    vg_draw_rectangle(btn->location_x + 8, btn->location_y + 3, btn->location_x + 12, btn->location_y + 17, vg_color_rgb(255, 255, 255));
}

void open_btn_draw(button_t* btn) { LOG
    vg_draw_rectangle(btn->location_x + 2, btn->location_y, btn->location_x + 10, btn->location_y + 3, vg_color_rgb(255, 201, 14));
    vg_draw_rectangle(btn->location_x, btn->location_y + 3, btn->location_x + 20, btn->location_y + 20, vg_color_rgb(255, 201, 14));
    vg_draw_line(btn->location_x, btn->location_y + 9, btn->location_x + 20, btn->location_y + 9, vg_color_rgb(100, 100, 100));
}

void save_btn_draw(button_t* btn) { LOG
    vg_draw_rectangle(btn->location_x, btn->location_y, btn->location_x + 20, btn->location_y + 20, vg_color_rgb(50, 50, 50));
    vg_draw_rectangle(btn->location_x + 5, btn->location_y, btn->location_x + 15, btn->location_y + 6, vg_color_rgb(255, 255, 255));
    vg_draw_rectangle(btn->location_x + 6, btn->location_y + 1, btn->location_x + 7, btn->location_y + 5, vg_color_rgb(0, 0, 0));
    vg_draw_circle(btn->location_x + 10, btn->location_y + 13, 2, vg_color_rgb(200, 200, 200));
    vg_set_pixel(btn->location_x + 10, btn->location_y + 13, vg_color_rgb(0, 0, 0));
}

void make_btn_draw(button_t* btn) { LOG
    vg_draw_rectangle(btn->location_x, btn->location_y, btn->location_x + 20, btn->location_y + 20, vg_color_rgb(200, 100, 100));
    vg_draw_line(btn->location_x +  2, btn->location_y + 17, btn->location_x + 2 , btn->location_y + 14, vg_color_rgb(0, 0, 0));
    vg_draw_line(btn->location_x +  2, btn->location_y + 14, btn->location_x + 9 , btn->location_y + 7 , vg_color_rgb(0, 0, 0));
    vg_draw_line(btn->location_x +  9, btn->location_y + 7 , btn->location_x + 9 , btn->location_y + 4 , vg_color_rgb(0, 0, 0));
    vg_draw_line(btn->location_x +  9, btn->location_y + 4 , btn->location_x + 11, btn->location_y + 2 , vg_color_rgb(0, 0, 0));
    vg_draw_line(btn->location_x + 11, btn->location_y + 2 , btn->location_x + 13, btn->location_y + 2 , vg_color_rgb(0, 0, 0));
    vg_draw_line(btn->location_x + 13, btn->location_y + 2 , btn->location_x + 14, btn->location_y + 3 , vg_color_rgb(0, 0, 0));
    vg_draw_line(btn->location_x + 14, btn->location_y + 3 , btn->location_x + 12, btn->location_y + 5 , vg_color_rgb(0, 0, 0));
    vg_draw_line(btn->location_x + 12, btn->location_y + 5 , btn->location_x + 12, btn->location_y + 8, vg_color_rgb(0, 0, 0));
    vg_draw_line(btn->location_x + 12, btn->location_y + 8, btn->location_x + 15, btn->location_y + 8 , vg_color_rgb(0, 0, 0));
    vg_draw_line(btn->location_x + 15, btn->location_y + 8 , btn->location_x + 17, btn->location_y + 6 , vg_color_rgb(0, 0, 0));
    vg_draw_line(btn->location_x + 17, btn->location_y + 6 , btn->location_x + 18, btn->location_y + 7 , vg_color_rgb(0, 0, 0));
    vg_draw_line(btn->location_x + 18, btn->location_y + 7 , btn->location_x + 18, btn->location_y + 9 , vg_color_rgb(0, 0, 0));
    vg_draw_line(btn->location_x + 18, btn->location_y + 9 , btn->location_x + 16, btn->location_y + 11, vg_color_rgb(0, 0, 0));
    vg_draw_line(btn->location_x + 16, btn->location_y + 11, btn->location_x + 13, btn->location_y + 11, vg_color_rgb(0, 0, 0));
    vg_draw_line(btn->location_x + 13, btn->location_y + 11, btn->location_x +  6, btn->location_y + 18, vg_color_rgb(0, 0, 0));
    vg_draw_line(btn->location_x +  6, btn->location_y + 18, btn->location_x +  3, btn->location_y + 18, vg_color_rgb(0, 0, 0));
    vg_draw_line(btn->location_x +  3, btn->location_y + 18, btn->location_x +  2, btn->location_y + 17, vg_color_rgb(0, 0, 0));
}

void run_btn_draw(button_t* btn) { LOG
    vg_draw_rectangle(btn->location_x, btn->location_y, btn->location_x + 20, btn->location_y + 20, vg_color_rgb(200, 100, 100));
    vg_draw_line(btn->location_x + 5, btn->location_y + 5, btn->location_x + 15, btn->location_y + 10, vg_color_rgb(0, 0, 0));
    vg_draw_line(btn->location_x + 15, btn->location_y + 10, btn->location_x + 5, btn->location_y + 15, vg_color_rgb(0, 0, 0));
    vg_draw_line(btn->location_x + 5, btn->location_y + 5, btn->location_x + 5, btn->location_y + 15, vg_color_rgb(0, 0, 0));
}

void close_btn_draw(button_t* btn) { LOG
    vg_draw_rectangle(btn->location_x, btn->location_y, btn->location_x + 20, btn->location_y + 20, vg_color_rgb(230, 0, 0));
    vg_draw_line(btn->location_x + 3, btn->location_y + 3, btn->location_x + 17, btn->location_y + 17, vg_color_rgb(255, 255, 255));
    vg_draw_line(btn->location_x + 17, btn->location_y + 3, btn->location_x + 3, btn->location_y + 17, vg_color_rgb(255, 255, 255));
}

void new_btn_click(button_t* btn) { LOG
    tab_t** tab = NULL;
    window_t* window = btn->wnd;
    int i;
    
    for (i = 0; i < (TAB_COUNT - 1); i++) {
        if (!window->tabs[i]) {
            tab = &window->tabs[i];
            break;
        }
    }

    if (i >= (TAB_COUNT - 1)) { 
        window_set_log_message(window, "New: No tabs available!");
        return;
    }

    *tab = tab_create("new tab");
    
    if (*tab) {
        window_set_log_message(window, "New: Tab created with success");
    }
}

void open_btn_click(button_t* btn) { LOG
    window_t* window = btn->wnd;
    if (window->state == WIN_STATE_NORMAL) {
        window_set_tab(window, TAB_CONSOLE);
        /* Empty console */
        window->state = WIN_STATE_OPEN_ASK_NAME;
    }
}

void save_btn_click(button_t* btn) { LOG
    window_t* window = btn->wnd;
    if (window->state == WIN_STATE_NORMAL) {
        window_set_tab(window, TAB_CONSOLE);
        /* Empty console */
        /* Write current tab name to console */
        window->state = WIN_STATE_SAVE_ASK_NAME;
    }
}

void make_btn_click(button_t* btn) { LOG
}

void run_btn_click(button_t* btn) { LOG

}

void close_btn_click(button_t* btn) { LOG
    printf("Exiting...\n");
    int_stop_handler();
}
