#include <minix/drivers.h>
#include <stdlib.h>

#include "video_gr.h"

#define WAIT_TIME_S 5
#define PACKED_PIXEL_1024_768_8_MODE 0x105
#define PACKED_PIXEL_1280_1024_8_MODE 0x107

int main(int argc, char* argv[])
{
    char* video_mem;
    int x, y, i;
    unsigned short color;
    
    sef_startup();

    video_mem = (char*)vg_init(PACKED_PIXEL_1280_1024_8_MODE);
    printf("lab2/main: VRAM mapped on virtual address %p\n", video_mem);

    vg_draw_line(0    ,0    ,0    ,1023 ,0x3F);
    vg_draw_line(0    ,0    ,1279 ,0    ,0x3F);
    vg_draw_line(0    ,1023 ,1279 ,1023 ,0x3F);
    vg_draw_line(1279 ,1023 ,1279 ,0    ,0x3F);

    for (i = 0; i < 256; i++)
        vg_draw_rectangle((i%10)*128,(i/10)*128,(i%10)*128+128,(i/10)*128+128,i);

    sleep(WAIT_TIME_S);
    vg_exit();

    return 0;
}
