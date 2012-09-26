#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "vbe.h"

/* Constants for VBE 0x105 mode */

#define VRAM_PHYS_ADDR    0xD0000000
#define H_RES             1024
#define V_RES		  768
#define BITS_PER_PIXEL	  8

#define LINEAR_MODEL_BIT 14
#define BIT(n) (0x1 << (n))

/* Private global variables */

static char *video_mem;		/* Process address to which VRAM is mapped */

static unsigned h_res;		/* Horizontal screen resolution in pixels */
static unsigned v_res;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

void * vg_init(unsigned long mode) {
	int r, s;
	struct mem_range mr;
	vbe_mode_info_t vmi_p;
	struct reg86u reg;

	reg.u.b.intno = 0x10;
	reg.u.b.ah = 0x4F;
	reg.u.b.al = 0x02;
	reg.u.w.bx = BIT(LINEAR_MODEL_BIT) | mode;

	s = sys_int86(&reg);

	if (vbe_get_mode_info(mode, &vmi_p) != 0) {
		return NULL;
	}

	h_res = vmi_p.XResolution;
	v_res = vmi_p.YResolution;
	bits_per_pixel = vmi_p.BitsPerPixel;

	mr.mr_base = vmi_p.PhysBasePtr;
	mr.mr_limit = mr.mr_base + h_res * v_res * (bits_per_pixel / 8);
	r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr);

	video_mem = vm_map_phys(SELF, (void *) mr.mr_base, h_res * v_res * (bits_per_pixel / 8));



	return (video_mem == MAP_FAILED ? NULL : video_mem);

	if (s != OK || r != OK || video_mem == MAP_FAILED) {
		return NULL;
	} else {
		return video_mem;
	}
}

int vg_fill(unsigned long color) {
	int i, j, colorT;
	char* vptr = video_mem;
	for (i = 0; i < h_res * v_res; i++) {
		colorT = color;
		for (j = 0; j < bits_per_pixel / 8; j++) {
			*vptr = (char) colorT;
			vptr++;
			colorT >> 8;
		}
	}
	return 0;
}

int vg_set_pixel(unsigned long x, unsigned long y, unsigned long color) {
	int i;
	char* vptr;
	i = (y * h_res + x) * (bits_per_pixel / 8);
	vptr = &video_mem[i];
	for (i = 0; i < bits_per_pixel / 8; i++) {
		*vptr = (char) color;
		vptr++;
		color >> 8;
	}
	return 0;
}

long vg_get_pixel(unsigned long x, unsigned long y) {
	return 0;
}

int vg_draw_line(unsigned long xi, unsigned long yi, 
		 unsigned long xf, unsigned long yf, unsigned long color) {
	return 0;
}

int vg_exit() {
  struct reg86u reg86;

  reg86.u.b.intno = 0x10; /* BIOS video services */

  reg86.u.b.ah = 0x00;    /* Set Video Mode function */
  reg86.u.b.al = 0x03;    /* 80x25 text mode*/

  if( sys_int86(&reg86) != OK ) {
      printf("\tvg_exit(): sys_int86() failed \n");
      return 1;
  } else
      return 0;
}
