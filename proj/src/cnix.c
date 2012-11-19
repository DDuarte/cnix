#include <stdio.h>
#include <stdlib.h>
#include <minix/drivers.h>

#include "video_gr.h"

int main(int argc, char const *argv[])
{
    char* video_mem;
    int error;

    sef_startup();
    printf("hi\n");

    /* initialize video mode */
    video_mem = vg_init(0x103); /* 800x600 */
    if (!video_mem)
    {
        printf("main: vg_init failed.\n");
        return 1;
    }

    sleep(5); /* placeholder */

    error = vg_exit();
    if (error)
    {
        printf("vg_exit failed with error code %i.\n", error);
        return 1;
    }

    return 0;
}
