#include "tab.h"
#include "video_gr.h"
#include "utilities.h"
#include "window.h"
#include <string.h>
#include <assert.h>
#include <ctype.h>

char_screen char_screen_create(char character, int color, int size) { LOG
    char_screen res;
    res.character = character;
    res.color = color;
    res.size = size;
    return res;
}

int char_screen_set_color(char_screen* cs, int color) { LOG
    cs->color = color;
    return 0;
}

int char_draw(char_screen* cs, int x, int y) { LOG
    int error;

    error = vg_draw_char(cs->character, cs->size, x, y, cs->color);
    if (error) {
        printf("char_draw: vg_draw_char failed with error code %d.\n", error);
        return error;
    }

    return 0;
}

tab_t* tab_create(char* file_name) { LOG
    tab_t* tab;

    tab = malloc(sizeof(tab_t));

    if (!tab)
        return NULL;

    tab->file_name = file_name;
    strncpy(tab->short_file_name, tab->file_name, 9);

    tab->current_column = 0;
    tab->current_line = 0;

    vector_new(&tab->lines);

    return tab;
}

int tab_destroy(tab_t* tab) { LOG
    int i, j;
    for (i = 0; i < vector_size(&tab->lines); ++i) {
        for (j = 0; j < vector_size(vector_get(&tab->lines, i)); ++j) {
            free(vector_get(vector_get(&tab->lines, i), j));
        }
        vector_free((vector*)vector_get(&tab->lines, i));
        free(vector_get(&tab->lines, i));
    }

    vector_free(&tab->lines);
    free(tab);

    return 0;
}

tab_t* tab_create_from_file(char* file_name, char* file_buffer) { LOG
    assert(file_name);
    assert(file_buffer);

    tab_t* tab = tab_create(file_name);

    int i;

    for (i = 0; file_buffer[i]; ++i) {
        tab_add_char(tab, file_buffer[i]);
    }

    return tab;
}

char* tab_to_file(tab_t* tab) { LOG
    char* buffer = (char*)malloc(sizeof(char)*5);
    strcpy(buffer, "test");

    return buffer;
}

int tab_draw_label(tab_t* tab, int tab_num, int selected) { LOG
    const int label_size_x = 92;
    const int label_size_y = 30;

    if (tab_num == 11) // console special tab
        return 0;

    vg_draw_rectangle(6 + tab_num * label_size_x, 31,
                      6 + tab_num * label_size_x + label_size_x, 31 + label_size_y,
                      vg_color_rgb(200 - selected * 20, 200 - selected * 20, 200 - selected * 20));

    vg_draw_string(tab->short_file_name, 18, 7 + tab_num * label_size_x, 30 + label_size_y, vg_color_rgb(255, 255, 255));

    return 0;
}

int tab_draw_text(tab_t* tab, int tab_num, int selected) { LOG   
    static unsigned int caret_on = 1;
    int x = 15;
    int y = (tab_num == TAB_CONSOLE ? 730 : 100);
    
    if (selected)
        if (caret_on) {
            vg_draw_line(tab->current_column * 17 + x, tab->current_line * 25 + y,
                    tab->current_column * 17 + x, tab->current_line * 25 + y - 17,
                    vg_color_rgb(0, 0, 0));
            caret_on = 0;
        } else {
            caret_on = 1;
        }

    // char_size = 17
    // size between lines = 25
     
    int i, j;
    for (i = 0; i < vector_size(&tab->lines); ++i) {
        for (j = 0; j < vector_size(vector_get(&tab->lines, i)); ++j) {
            char_screen* cs = vector_get(vector_get(&tab->lines, i), j);
            char_draw(cs, j * 17 + x, i * 25 + y);
        }
    }

    return 0;
}

int tab_draw(tab_t* tab, int tab_num, int selected) { LOG
    tab_draw_label(tab, tab_num, selected);

    if (selected || tab_num == TAB_CONSOLE)
        tab_draw_text(tab, tab_num, selected);

    return 0;
}

int tab_add_char(tab_t* tab, char character) { LOG

    if (!vector_size(&tab->lines)) {
        vector* line = (vector*) malloc(sizeof(vector));
        vector_new(line);
        vector_push_back(&tab->lines, line);
    }

    vector* current_line = (vector*)vector_get(&tab->lines, tab->current_line);

    if (character == '\n') // add new line
    {
        vector* new_line = (vector*)malloc(sizeof(vector));
        vector_new(new_line);

        if (vector_size(current_line) > tab->current_column) { // see if current line has more characters after the cursor
            int i;
            int new_line_size = vector_size(current_line) - tab->current_column;

            for (i = 0; i < new_line_size; ++i) { // copy chars to new line
                vector_push_back(new_line, vector_get(current_line, tab->current_column + i));
            }

            for (i = 0; i < new_line_size; ++i) { // remove from current line
                free(vector_get(current_line, tab->current_column));
                vector_erase(current_line, tab->current_column); // erase reduces the number of elements, no +i
            }
        }

        if (vector_size(&tab->lines) == tab->current_line + 1)
            vector_push_back(&tab->lines, new_line);
        else
            vector_insert(&tab->lines, new_line, tab->current_line + 1);

        // put cursor at beginning of line and move to next line
        tab->current_column = 0;
        tab->current_line++;
    } else { // add character
        char_screen* cs = (char_screen*)malloc(sizeof(char_screen));
        *cs = char_screen_create(character, vg_color_rgb(0, 0, 0), 32);

        if (vector_size(current_line) == tab->current_column)
            vector_push_back(current_line, cs);
        else
            vector_insert(current_line, cs, tab->current_column);

        tab->current_column++;
    }

    return 0;
}

int tab_remove_char(tab_t* tab) { LOG
    // TODO
    return 0;
}

int tab_key_press(tab_t* tab, KEY key) { LOG
    if (key <= 0 && key >= LAST_KEY) {
        return 1;
    }

    int last_column_this_line;
    if (!vector_size(&tab->lines))
        last_column_this_line = 0;
    else
        last_column_this_line = vector_size(vector_get(&tab->lines, tab->current_line));

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
            tab_add_char(tab, '\n');
            break;
        case KEY_BKSP:
            tab_remove_char(tab);
            break;
        case KEY_HOME:
            tab->current_column = 0;
            break;
        case KEY_END:
            tab->current_column = last_column_this_line;
            break;
        default: {
            char c = key_to_char(key);
            if (c)
                tab_add_char(tab, c);
            break;
        }
    }

    if (tab->current_line < 0) tab->current_line = 0;
    if (tab->current_column < 0) tab->current_column = 0;
    //if (tab->current_column > last_column_this_line) tab->current_column = last_column_this_line - 1;
    //if (tab->current_line > EOF) tab->current_column = EOF;

    return 0;
}

int tab_mouse_press(tab_t* tab, unsigned long x, unsigned long y) { LOG
    // TODO: calculate current_column and current_line based on x and y
    // TODO: if dragging mouse do text selection (possibly)
    return 0;
}
