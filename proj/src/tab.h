#ifndef TAB_H
#define TAB_H

#include "keyboard.h"
#include "vector.h"

typedef struct {

    int size;
    int color;
    char character;

} char_screen;

char_screen char_screen_create(char character, int color, int size);
int char_screen_set_color(char_screen* cs, int color);
int char_draw(char_screen* cs, int x, int y);

typedef struct {

    char* file_name;
    char short_file_name[9];
    vector lines;

    int current_line;
    int current_column;

} tab_t;

tab_t* tab_create_from_file(char* file_name, char* file_buffer); // tab filled with contents of file name
char* tab_to_file(tab_t* tab);

tab_t* tab_create(char* file_name); // empty tab
int tab_draw(tab_t* tab, int tab_num, int selected);
int tab_destroy(tab_t* tab);

int tab_key_press(tab_t* tab, KEY key);
int tab_mouse_press(tab_t* tab, unsigned long x, unsigned long y);

int tab_add_char(tab_t* tab, char character);
int tab_remove_char(tab_t* tab);

#endif
