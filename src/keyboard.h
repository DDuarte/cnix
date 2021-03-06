#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "utilities.h"

/** @defgroup keyboard keyboard
 * @{
 * Constants and functions for managing keyboard (higher level layer over kbc)
 */

#define KEY_PRESS(x) (x) ///< Convers scancode to press
#define KEY_RELEASE(x) (0x80 | (x)) ///< Converts scancode to release
#define VALID_KEY(KEY) (((KEY) > 0 && (KEY) <= KEY_F12) || ((KEY) >= KEY_NUM_ENTER && (KEY) <= KEY_END)) ///< Non zero if key is valid

/// Keys
typedef enum {
    KEY_NONE         = 0x0000,
    KEY_ESC          = 0x0001,
    KEY_1            = 0x0002,
    KEY_2            = 0x0003,
    KEY_3            = 0x0004,
    KEY_4            = 0x0005,
    KEY_5            = 0x0006,
    KEY_6            = 0x0007,
    KEY_7            = 0x0008,
    KEY_8            = 0x0009,
    KEY_9            = 0x000A,
    KEY_0            = 0x000B,
    KEY_APOSTROPHE   = 0x000C,
    KEY_ANGLE_QUOTES = 0x000D,
    KEY_BKSP         = 0x000E,
    KEY_TAB          = 0x000F,
    KEY_Q            = 0x0010,
    KEY_W            = 0x0011,
    KEY_E            = 0x0012,
    KEY_R            = 0x0013,
    KEY_T            = 0x0014,
    KEY_Y            = 0x0015,
    KEY_U            = 0x0016,
    KEY_I            = 0x0017,
    KEY_O            = 0x0018,
    KEY_P            = 0x0019,
    KEY_PLUS         = 0x001A,
    KEY_ACCENT       = 0x001B,
    KEY_ENTER        = 0x001C,
    KEY_L_CTRL       = 0x001D,
    KEY_A            = 0x001E,
    KEY_S            = 0x001F,
    KEY_D            = 0x0020,
    KEY_F            = 0x0021,
    KEY_G            = 0x0022,
    KEY_H            = 0x0023,
    KEY_J            = 0x0024,
    KEY_K            = 0x0025,
    KEY_L            = 0x0026,
    KEY_C_CEDILLA    = 0x0027,
    KEY_ORDINAL      = 0x0028,
    KEY_BACKSLASH    = 0x0029,
    KEY_L_SHIFT      = 0x002A,
    KEY_TILDE        = 0x002B,
    KEY_Z            = 0x002C,
    KEY_X            = 0x002D,
    KEY_C            = 0x002E,
    KEY_V            = 0x002F,
    KEY_B            = 0x0030,
    KEY_N            = 0x0031,
    KEY_M            = 0x0032,
    KEY_COMMA        = 0x0033,
    KEY_DOT          = 0x0034,
    KEY_MINUS        = 0x0035,
    KEY_R_SHIFT      = 0x0036,
    KEY_ALT          = 0x0038,
    KEY_SPACE        = 0x0039,
    KEY_CAPS         = 0x003A,
    KEY_F1           = 0x003B,
    KEY_F2           = 0x003C,
    KEY_F3           = 0x003D,
    KEY_F4           = 0x003E,
    KEY_F5           = 0x003F,
    KEY_F6           = 0x0040,
    KEY_F7           = 0x0041,
    KEY_F8           = 0x0042,
    KEY_F9           = 0x0043,
    KEY_F10          = 0x0044,
    KEY_NUM          = 0x0045,
    KEY_SCRLL        = 0x0046,
    KEY_NUM_7        = 0x0047,
    KEY_NUM_8        = 0x0048,
    KEY_NUM_9        = 0x0049,
    KEY_NUM_MINUS    = 0x004A,
    KEY_NUM_4        = 0x004B,
    KEY_NUM_5        = 0x004C,
    KEY_NUM_6        = 0x004D,
    KEY_NUM_PLUS     = 0x004E,
    KEY_NUM_1        = 0x004F,
    KEY_NUM_2        = 0x0050,
    KEY_NUM_3        = 0x0051,
    KEY_NUM_0        = 0x0052,
    KEY_NUM_DEL      = 0x0053,
    KEY_MINOR        = 0x0056,
    KEY_F11          = 0x0057,
    KEY_F12          = 0x0058,
    KEY_NUM_ENTER    = 0xE01C,
    KEY_R_CTRL       = 0xE01D,
    KEY_NUM_SLASH    = 0xE035,
    KEY_ALT_GR       = 0xE038,
    KEY_HOME         = 0xE047,
    KEY_ARR_UP       = 0xE048,
    KEY_PGUP         = 0xE049,
    KEY_ARR_LEFT     = 0xE04B,
    KEY_ARR_RIGHT    = 0xE04D,
    KEY_ARR_DOWN     = 0xE050,
    KEY_PGDN         = 0xE051,
    KEY_INS          = 0xE052,
    KEY_DEL          = 0xE053,
    KEY_WIN          = 0xE05B,  // right side of alt
    KEY_CNTX         = 0xE05D,  // left side of right ctrl
    KEY_END          = 0xE04F
} KEY;

/// Key modifiers
typedef enum {
    KEY_MOD_NONE    = 0,
    KEY_MOD_L_SHIFT = BIT(0),
    KEY_MOD_R_SHIFT = BIT(1),
    KEY_MOD_L_CTRL  = BIT(2),
    KEY_MOD_R_CTRL  = BIT(3),
    KEY_MOD_ALT     = BIT(4),
    KEY_MOD_ALT_GR  = BIT(5),
    KEY_MOD_CAPS    = BIT(6)
} KEY_MOD;

#define LAST_KEY KEY_F12 ///< Last key

/// Key index to be used with key_c[][]
typedef enum {
    KEY_IDX_NONE,
    KEY_IDX_UP,
    KEY_IDX_SHT,
    KEY_IDX_ALTGR
} KEY_IDX;

/// Array used to convert keys into chars, including modifiers (caps lock, alt, etc.)
// normal, upper case, shift, alt gr
static char key_c[][4] = {
    {  0,   0,   0,   0  }, // KEY_NONE
    {  0,   0,   0,   0  }, // KEY_ESC
    { '1', '1', '!',  0  }, // KEY_1
    { '2', '2', '"', '@' }, // KEY_2
    { '3', '3', '#',  0  }, // KEY_3 (4: £)
    { '4', '4', '$',  0  }, // KEY_4 (4: §)
    { '5', '5', '%',  0  }, // KEY_5
    { '6', '6', '&',  0  }, // KEY_6
    { '7', '7', '/', '{' }, // KEY_7
    { '8', '8', '(', '[' }, // KEY_8
    { '9', '9', ')', ']' }, // KEY_9
    { '0', '0', '=', '}' }, // KEY_0
    { '\'', '\'', '?', 0 }, // KEY_APOSTROPHE
    {  0,   0,   0,   0  }, // KEY_ANGLE_QUOTES (1: «, 2: «, 3: »)
    {  0,   0,   0,   0  }, // KEY_BKSP
    {  0,   0,   0,   0  }, // KEY_TAB
    { 'q', 'Q', 'q',  0  }, // KEY_Q
    { 'w', 'W', 'w',  0  }, // KEY_W
    { 'e', 'E', 'e',  0  }, // KEY_E (4: €)
    { 'r', 'R', 'r',  0  }, // KEY_R
    { 't', 'T', 't',  0  }, // KEY_T
    { 'y', 'Y', 'y',  0  }, // KEY_Y
    { 'u', 'U', 'u',  0  }, // KEY_U
    { 'i', 'I', 'i',  0  }, // KEY_I
    { 'o', 'O', 'o',  0  }, // KEY_O
    { 'p', 'P', 'p',  0  }, // KEY_P
    { '+', '+', '*',  0  }, // KEY_PLUS (4: ¨)
    {  0,   0,   0,  ']' }, // KEY_ACCENT (1: ´, 2: ´, 3: `)
    {  0,   0,   0,   0  }, // KEY_ENTER
    {  0,   0,   0,   0  }, // KEY_L_CTRL
    { 'a', 'A', 'a',  0  }, // KEY_A
    { 's', 'S', 's',  0  }, // KEY_S
    { 'd', 'D', 'd',  0  }, // KEY_D
    { 'f', 'F', 'f',  0  }, // KEY_F
    { 'g', 'G', 'g',  0  }, // KEY_G
    { 'h', 'H', 'h',  0  }, // KEY_H
    { 'j', 'J', 'j',  0  }, // KEY_J
    { 'k', 'K', 'k',  0  }, // KEY_K
    { 'l', 'L', 'l',  0  }, // KEY_L
    {  0,   0,   0,   0  }, // KEY_C_CEDILLA (1: ç, 2: Ç, 3: ç)
    {  0,   0,   0,   0  }, // KEY_ORDINAL (1: º, 2: º, 3: ª)
    { '\\', '\\', '|', 0 }, // KEY_BACKSLASH
    {  0,   0,   0,   0  }, // KEY_L_SHIFT
    { '~', '~', '^',  0  }, // KEY_TILDE
    { 'z', 'Z', 'z',  0  }, // KEY_Z
    { 'x', 'X', 'x',  0  }, // KEY_X
    { 'c', 'C', 'c',  0  }, // KEY_C
    { 'v', 'V', 'v',  0  }, // KEY_V
    { 'b', 'B', 'b',  0  }, // KEY_B
    { 'n', 'N', 'n',  0  }, // KEY_N
    { 'm', 'M', 'm',  0  }, // KEY_M
    { ',', ',', ';',  0  }, // KEY_COMMA
    { '.', '.', ':',  0  }, // KEY_DOT
    { '-', '-', '_',  0  }, // KEY_MINUS
    {  0,   0,   0,   0  }, // KEY_R_SHIFT
    {  0,   0,   0,   0  }, // KEY_UNKNOWN2
    {  0,   0,   0,   0  }, // KEY_ALT
    { ' ', ' ', ' ',  0  }, // KEY_SPACE
    {  0,   0,   0,   0  }, // KEY_CAPS
    {  0,   0,   0,   0  }, // KEY_F1
    {  0,   0,   0,   0  }, // KEY_F2
    {  0,   0,   0,   0  }, // KEY_F3
    {  0,   0,   0,   0  }, // KEY_F4
    {  0,   0,   0,   0  }, // KEY_F5
    {  0,   0,   0,   0  }, // KEY_F6
    {  0,   0,   0,   0  }, // KEY_F7
    {  0,   0,   0,   0  }, // KEY_F8
    {  0,   0,   0,   0  }, // KEY_F9
    {  0,   0,   0,   0  }, // KEY_F10
    {  0,   0,   0,   0  }, // KEY_NUM
    {  0,   0,   0,   0  }, // KEY_SCRL
    { '7', '7',  0,   0  }, // KEY_NUM_7
    { '8', '8',  0,   0  }, // KEY_NUM_8
    { '9', '9',  0,   0  }, // KEY_NUM_9
    { '-', '-', '-',  0  }, // KEY_NUM_MINUS
    { '4', '4',  0,   0  }, // KEY_NUM_4
    { '5', '5',  0,   0  }, // KEY_NUM_5
    { '6', '6',  0,   0  }, // KEY_NUM_6
    { '+', '+', '+',  0  }, // KEY_NUM_PLUS
    { '1', '1',  0,   0  }, // KEY_NUM_1
    { '2', '2',  0,   0  }, // KEY_NUM_2
    { '3', '3',  0,   0  }, // KEY_NUM_3
    { '0', '0',  0,   0  }, // KEY_NUM_0
    { '.', '.',  0,   0  }, // KEY_NUM_DEL
    {  0,   0,   0,   0  }, // KEY_UNKNOWN3
    {  0,   0,   0,   0  }, // KEY_UNKNOWN4
    { '<', '<', '>',  0  }, // KEY_MINOR
    {  0,   0,   0,   0  }, // KEY_F11
    {  0,   0,   0,   0  }, // KEY_F12
};

unsigned int last_key_pressed; ///< Key that was pressed (-1 if no key)


/**
 * @brief Converts a key into a char
 *
 * @param scancode Scancode of key
 * @return 0 if bad key, character otherwise
 */
char key_to_char(unsigned int scancode);

int keyboard_install(void); ///< Subscribe keyboard
int keyboard_destroy(void); ///< Unsubscribe keyboard

/**@}*/

#endif
