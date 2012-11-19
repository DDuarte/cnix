#include <stdio.h>
#include <stdlib.h>
#include <minix/drivers.h>
#include <string.h>
#include "video_gr.h"

int main(int argc, char const *argv[])
{
    char* video_mem;
    int error;

	sef_startup();
	
	
    
    printf("hi\n");
    
    /* initialize video mode */
    video_mem = vg_init(0x113); /* 800x600 */
    if (!video_mem)
    {
        printf("main: vg_init failed.\n");
        return 1;
    }
	
    vg_fill(0xffff);
	vg_swap_buffer();
	
    sleep(1); /* placeholder */
	
	vg_fill(0xffff);
	vg_draw_circle(100, 100, 50, 0x0);
	vg_swap_buffer();
	
	sleep(5);
    
    error = vg_exit();
    if (error)
    {
        printf("vg_exit failed with error code %i.\n", error);
        return 1;
    }

    return 0;
}
