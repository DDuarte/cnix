#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#define KEY_PRESS(x) (x)
#define KEY_RELEASE(x) (0x80 | (x))

typedef enum {
KEY_NONE      ,
KEY_ESC       ,
KEY_1         ,
KEY_2         ,
KEY_3         ,
KEY_4         ,
KEY_5         ,
KEY_6         ,
KEY_7         ,
KEY_8         ,
KEY_9         ,
KEY_0         ,
KEY_APOSTROPHE,
KEY_ANGLE_QUOTES,
KEY_BKSP      ,
KEY_TAB       ,
KEY_Q         ,
KEY_W         ,
KEY_E         ,
KEY_R         ,
KEY_T         ,
KEY_Y         ,
KEY_U         ,
KEY_I         ,
KEY_O         ,
KEY_P         ,
KEY_PLUS      ,
KEY_ACCENT    ,
KEY_ENTER     ,
KEY_L_CTRL    ,
KEY_A         ,
KEY_S         ,
KEY_D         ,
KEY_F         ,
KEY_G         ,
KEY_H         ,
KEY_J         ,
KEY_K         ,
KEY_L         ,
KEY_C_CEDILLA ,
KEY_ORDINAL   ,
KEY_UNKNOWN1  ,
KEY_L_SHIFT   ,
KEY_TILDE     ,
KEY_Z         ,
KEY_X         ,
KEY_C         ,
KEY_V         ,
KEY_B         ,
KEY_N         ,
KEY_M         ,
KEY_COMMA         ,
KEY_DOT         ,
KEY_MINUS         ,
KEY_R_SHIFT   ,
KEY_UNKNOWN2  ,
KEY_ALT       ,
KEY_SPACE     ,
KEY_CAPS      ,
KEY_F1	      ,
KEY_F2	      ,
KEY_F3	      ,
KEY_F4	      ,
KEY_F5	      ,
KEY_F6	      ,
KEY_F7	      ,
KEY_F8	      ,
KEY_F9	      ,
KEY_F10	      ,
KEY_NUM	      ,
KEY_SCRL      ,
KEY_NUM_7     ,
KEY_NUM_8     ,
KEY_NUM_9     ,
KEY_NUM_MINUS     ,
KEY_NUM_4     ,
KEY_NUM_5     ,
KEY_NUM_6     ,
KEY_NUM_PLUS	  ,
KEY_NUM_1	  ,
KEY_NUM_2     ,
KEY_NUM_3     ,
KEY_NUM_0     ,
KEY_NUM_DEL	  ,
KEY_UNKNOWN3  ,
KEY_UNKNOWN4  ,
KEY_MINOR     ,
KEY_F11	      ,
KEY_F12       	      
} KEY;

#define LAST_KEY KEY_F12

unsigned char keys[LAST_KEY];

unsigned int last_key_pressed;

int keyboard_install(void);
int keyboard_destroy(void);



#endif