#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "lmlib.h"
#include "vbe.h"

/* Constants for VBE 0x105 mode */

#define VRAM_PHYS_ADDR    0xD0000000
#define H_RES             1024
#define V_RES		  768
#define BITS_PER_PIXEL	  8

#define LINEAR_MODEL_BIT 14
#define BIT(n) (0x1 << (n))

#define VBE_MODE 0x4F
#define SET_VBE_MODE 0x02
#define BIOS_VIDEO_SERVICE 0x10

#define ERR -1

/* Private global variables */

static char *video_mem;		/* Process address to which VRAM is mapped */

static unsigned h_res;		/* Horizontal screen resolution in pixels */
static unsigned v_res;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

void* vg_init(unsigned long mode)
{
    vbe_mode_info_t vmi_p;
    struct reg86u r;
    struct mem_range mr;
    unsigned int vram_size;

    r.u.b.intno = BIOS_VIDEO_SERVICE;
    r.u.b.ah = VBE_MODE;
    r.u.b.al = SET_VBE_MODE;
    r.u.w.bx = BIT(LINEAR_MODEL_BIT) | mode;
    if (sys_int86(&r))
    {
        printf("lab2/vg_init: failed in sys_int86");
        return NULL;
    }

    if (vbe_get_mode_info(mode, &vmi_p) == ERR)
    {
        printf("lab2/vg_init: failed in vbe_get_mode_info");
        return NULL;
    }

    h_res = vmi_p.XResolution;
    v_res = vmi_p.YResolution;
    bits_per_pixel = vmi_p.BitsPerPixel;

    vram_size =  h_res * v_res * (bits_per_pixel / 8);

    mr.mr_base = vmi_p.PhysBasePtr;
    mr.mr_limit = mr.mr_base + vram_size;
    if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))
    {
        printf("lab2/vg_init: failed in sys_privctl");
        return NULL;
    }

    video_mem = vm_map_phys(SELF, (void*)mr.mr_base, vram_size);

    if (video_mem == MAP_FAILED)
    {
        printf("lab2/vg_init: failed in vm_map_phys");
        return NULL;
    }

    return video_mem;
}

int vg_fill(unsigned long color)
{
    int i, j;
    unsigned long colorTemp;
    char* ptr;

    ptr = video_mem;

    for (i = 0; i < h_res * v_res; ++i)
    {
        colorTemp = color;
        for (j = 0; j < ((bits_per_pixel / 8)| 1); ++j)
        {
            *ptr = (char)colorTemp;
            ++ptr;
            colorTemp >>= 8;
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
