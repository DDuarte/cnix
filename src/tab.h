#ifndef TAB_H
#define TAB_H

#include "keyboard.h"
#include "vector.h"

/** @defgroup tab tab
 * @{
 * Functions for interaction with text tab
 */

#define tabMaxCharsC 24
#define tabMaxCharsL 59

/// Represents a character in screen (size, color, letter/digit/symbol)
typedef struct {
    int size; ///< Size
    int color; ///< Color
    char character; ///< Letter/digit/symbol
} char_screen;

char_screen char_screen_create(char character, int color, int size); ///< Creates a char_screen
int char_screen_set_color(char_screen* cs, int color); ///< Changes color of char_screen
int char_draw(char_screen* cs, int x, int y); ///< Draws char_screen to screen at x and y

/// Text tab
typedef struct {

    char* file_name; ///< Full file name
    char short_file_name[9]; ///< File name used in tab label
    vector lines; ///< Internal buffer for characters (vector of vector of char_screen*)

    int current_line; ///< Position of cursor - line
    int current_column; ///< Position of cursor - column
    int maxLineSize; ///< Max size of line, in characters

} tab_t;

tab_t* tab_create_from_file(char* file_name, char* file_buffer); ///< Tab filled with contents of file
char* tab_to_file(tab_t* tab); ///< Gets tab content and returns it as a string

int tab_advance_column(tab_t* tab); ///< Advances cursor column (with limits)
int tab_rewind_column(tab_t* tab); ///< Retreats cursor column (with limits)

int tab_advance_line(tab_t* tab); ///< Advances cursor line (with limits)
int tab_rewind_line(tab_t* tab); ///< Retreats cursor line (with limits)

tab_t* tab_create(char* file_name); ///< Creates empty tab
int tab_draw(tab_t* tab, int tab_num, int selected, int drawText, int hasFocus); ///< Draw tab to screen
int tab_destroy(tab_t* tab); ///< Remove tab

int tab_key_press(tab_t* tab, KEY key); ///< A key has been pressed while tab was active
int tab_mouse_press(tab_t* tab, unsigned long x, unsigned long y); ///< Mouse has been pressed while tab was active

int tab_add_char(tab_t* tab, char character); ///< Adds a character to tab
int tab_printf(tab_t* tab, char* format, ...); ///< Prints the message to the end of the tab
int tab_remove_char(tab_t* tab); ///< Removes a character from tab
int tab_remove_all(tab_t* tab); ///< Removes all characters from tab

/**@}*/

#endif
