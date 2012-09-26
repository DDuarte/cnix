#include <minix/drivers.h>
#include <stdlib.h>

#include "video_gr.h"

#define WAIT_TIME_S 5

int main (int argc, char* argv[]) {

	char *video_mem;

	sef_startup();

	video_mem = vg_init(0x105);
	/*vg_fill(0x3F);*/
	vg_set_pixel(500, 500, 0x3F);
	sleep(WAIT_TIME_S);
	vg_exit();

	return 0;
}
