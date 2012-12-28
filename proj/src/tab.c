#include "tab.h"
#include "video_gr.h"
#include <string.h>

char_screen char_screen_create(char character, int color, int size) {
    char_screen res;
    res.character = character;
    res.color = color;
    res.size = color;
    return res;
}

int char_screen_set_color(char_screen* cs, int color) {
    cs->color = color;
    return 0;
}

int char_draw(char_screen* cs, int x, int y) {
    int error;

    error = vg_draw_char(cs->character, cs->size, x, y, cs->color);
    if (error) {
        printf("char_draw: vg_draw_char failed with error code %d.\n", error);
        return error;
    }

    return 0;
}

tab_t* tab_create(char* file_name) {
    tab_t* tab;

    tab = malloc(sizeof(tab_t));

    if (!tab)
        return NULL;

    tab->file_name = file_name;
    strncpy(tab->short_file_name, tab->file_name, 9);

    tab->current_column = 0;
    tab->current_line = 0;

    return tab;
}

int tab_draw_label(tab_t* tab, int tab_num, int selected) {

    const int label_size_x = 92;
    const int label_size_y = 30;

    char name[10];
    strncpy(name, tab->file_name, 9);

    vg_draw_rectangle(6 + tab_num * label_size_x, 31,
                      6 + tab_num * label_size_x + label_size_x, 31 + label_size_y,
                      vg_color_rgb(200 - selected * 20, 200 - selected * 20, 200 - selected * 20));

    vg_draw_string(tab->short_file_name, 18, 7 + tab_num * label_size_x, 30 + label_size_y, vg_color_rgb(255, 255, 255));

    return 0;
}

int tab_draw_text(tab_t* tab) {
    //static int drawCaret = 50;
    //--drawCaret;
    //
    //if (drawCaret == 0) {
    //    drawCaret = 50;
    //
    //    vg_draw_line(50, 50, 100, 100, vg_color_rgb(0, 0, 0));
    //}

    vg_draw_string(tab->file_name, 40, 400, 400, vg_color_rgb(rand()%255, rand()%255, rand()%255));

    return 0;
}

int tab_draw(tab_t* tab, int tab_num, int selected) {

    tab_draw_label(tab, tab_num, selected);

    if (selected)
        tab_draw_text(tab);

    return 0;
}

int tab_key_press(tab_t* tab, KEY key) {

    switch (key) {
        case KEY_ARR_UP:
            tab->current_line--;
            break;
        case KEY_ARR_DOWN:
            tab->current_line++;
            break;
        case KEY_ARR_LEFT:
            tab->current_column--;
            break;
        case KEY_ARR_RIGHT:
            tab->current_column++;
            break;
        case KEY_ENTER:
        case KEY_NUM_ENTER:
            tab->current_line++;
            break;
        case KEY_HOME:
            tab->current_column = 0;
            break;
        case KEY_END:
            // tab->current_column = last character in this line;
            break;
    }

    if (tab->current_line < 0) tab->current_line = 0;
    if (tab->current_column < 0) tab->current_column = 0;
    //if (tab->current_column > EOL) tab->current_column = EOL;
    //if (tab->current_line > EOF) tab->current_column = EOF;

    // TODO: write characters to the buffer at current_line and current_column
    // TODO: handle

    return 0;
}

int tab_mouse_press(tab_t* tab, unsigned long x, unsigned long y) {
    // TODO: calculate current_column and current_line based on x and y
    // TODO: if dragging mouse do text selection (possibly)
    return 0;
}
