#include <stdio.h>
#include <stdlib.h>
#include <minix/drivers.h>
#include <string.h>
#include "video_gr.h"
#include "vbe.h"

int main(int argc, char const *argv[])
{
    char* video_mem;
    int error;
	vbe_mode_info_t vmi;
	
	sef_startup();
    
    
    printf("hi\n");
    
    /* initialize video mode */
    video_mem = vg_init(0x117); /* 800x600 */
    if (!video_mem)
    {
        printf("main: vg_init failed.\n");
        return 1;
    }
	
    vg_fill(vg_color_rgb(255,255,255));
    vg_draw_rectangle(0, 0, 1024, 30, vg_color_rgb(90, 90, 90));
    vg_draw_rectangle(994, 5, 1014, 25, vg_color_rgb(230, 0, 0));
    vg_draw_line(997, 8, 1011, 22, vg_color_rgb(255,255,255));
    vg_draw_line(1011, 8, 997, 22, vg_color_rgb(255,255,255));
	vg_swap_buffer();
	
    sleep(5); /* placeholder */
    
    error = vg_exit();
    if (error)
    {
        printf("vg_exit failed with error code %i.\n", error);
        return 1;
    }

    return 0;
}
