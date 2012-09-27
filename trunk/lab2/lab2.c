#include <stdlib.h>
#include "video_gr.h"

#define WAIT_TIME_S 5
#define PACKED_PIXEL_1024_768_8_MODE 0x105

int main(int argc, char** argv)
{
    char* video_mem;
    int x, y;
    unsigned short color;
    
    sef_startup();

    video_mem = (char*)vg_init(PACKED_PIXEL_1024_768_8_MODE);

    printf("lab2/main: VRAM mapped on virtual address %p\n", video_mem);

    /* vg_fill(0x3f); */

    color = 0;

    for (x = 0; x < 1024; ++x)
        for (y = 0; y < 768; ++y)
            vg_set_pixel(x, y, color++ % 56);

    sleep(WAIT_TIME_S);
    vg_exit();
}