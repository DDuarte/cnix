#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/bitmap.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "vbe.h"
#include "video_gr.h"
#include "utilities.h"

#define LINEAR_MODEL_BIT 14

#define VBE_MODE 0x4F
#define VIDEO_MODE 0x00

#define SET_VBE_MODE 0x02
#define SET_TEXT_MODE 0x03

#define BIOS_VIDEO_SERVICE 0x10

/* Private global variables */

static char *video_mem; /* Process address to which VRAM is mapped */
static char *temp_video_mem; /* address of the temporary buffer */
static unsigned int vram_size;

static unsigned h_res; /* Horizontal screen resolution in pixels */
static unsigned v_res; /* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */
static unsigned bytes_per_pixel; /* Number of VRAM bytes per pixel (at least one) */
static vbe_mode_info_t vmi_p;

static struct {
    unsigned long redMask;
    unsigned long greenMask;
    unsigned long blueMask;
    unsigned long redPosition;
    unsigned long greenPosition;
    unsigned long bluePosition;
} gr_color;


unsigned long vg_color_rgb(unsigned long r, unsigned long g, unsigned long b) {
    return ((((b * gr_color.blueMask)  / 255) << gr_color.bluePosition)  |
            (((g * gr_color.greenMask) / 255) << gr_color.greenPosition) |
            (((r * gr_color.redMask)   / 255) << gr_color.redPosition));
}

void* vg_init(unsigned short mode)
{
    struct mem_range mr;
    struct reg86u r;

    r.u.b.intno = BIOS_VIDEO_SERVICE;
    r.u.b.ah = VBE_MODE;
    r.u.b.al = SET_VBE_MODE;
    r.u.w.bx = BIT(LINEAR_MODEL_BIT) | mode;

    if (sys_int86(&r))
    {
        printf("lab2/vg_init: failed in sys_int86");
        return NULL;
    }

    if (vbe_get_mode_info(mode, &vmi_p) < 0)
    {
        printf("vg_init: failed in vbe_get_mode_info");
        return NULL;
    }

    h_res = vmi_p.XResolution;
    v_res = vmi_p.YResolution;
    bits_per_pixel = vmi_p.BitsPerPixel;
    bytes_per_pixel = ((bits_per_pixel / 8) > 0) ? (bits_per_pixel / 8) : 1;

    vram_size = h_res * v_res * bytes_per_pixel;

    mr.mr_base = vmi_p.PhysBasePtr;
    mr.mr_limit = mr.mr_base + vram_size;
    if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))
    {
        printf("vg_init: failed in sys_privctl");
        return NULL;
    }

    video_mem = vm_map_phys(SELF, (void*)mr.mr_base, vram_size);

    if (video_mem == MAP_FAILED)
    {
        printf("vg_init: failed in vm_map_phys");
        return NULL;
    }

    temp_video_mem = (char*)malloc(vram_size);

    gr_color.redMask = bit_set_all(vmi_p.RedMaskSize);
    gr_color.greenMask = bit_set_all(vmi_p.GreenMaskSize);
    gr_color.blueMask = bit_set_all(vmi_p.BlueMaskSize);
    gr_color.redPosition = vmi_p.RedFieldPosition;
    gr_color.greenPosition = vmi_p.GreenFieldPosition;
    gr_color.bluePosition = vmi_p.BlueFieldPosition;

    return video_mem;
}

int vg_fill(unsigned long color) {
    int i, j;
    unsigned long colorTemp;
    char* ptr;

    ptr = temp_video_mem;

    for (i = 0; i < h_res * v_res; ++i)
    {
        colorTemp = color;
        for (j = 0; j < bytes_per_pixel; ++j)
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

    if (x > h_res || y > v_res) {
        return -1;
    }

    i = (y * h_res + x) * bytes_per_pixel;

    vptr = &temp_video_mem[i];

    for (i = 0; i < bytes_per_pixel; i++) {
        *vptr = (char) color;
        vptr++;
        color >>= 8;
    }

    return 0;
}

long vg_get_pixel(unsigned long x, unsigned long y) {
    int i;
    long res;
    char* vptr;

    res = 0;

    if (x > h_res || y > v_res) {
        return -1;
    }

    i = (y * h_res + x) * bytes_per_pixel;
    vptr = &temp_video_mem[i];
    for (i = 0; i < bytes_per_pixel; i++) {
        res <<= 8;
        res |= *vptr;
        vptr++;
    }
    return res;
}

int vg_draw_line(unsigned long xi, unsigned long yi, unsigned long xf,
    unsigned long yf, unsigned long color)
{
    long i;
    double m, yt;
    yt = yi;

    if (xi > h_res || yi > v_res || xf > h_res || yf > v_res) {
        return -1;
    }

    if (xi == xf) {
        if (yi > yf) {
            for (i = yi; i >= yf; i--)
                vg_set_pixel(xi, i, color);
        }
        else {
            for (i = yi; i < yf; i++)
                vg_set_pixel(xi, i, color);
        }

    } else if (xi < xf) {
        if (yi > yf)
            m = (double)(yf - yi) / (double)(xi - xf);
        else
            m = (double)(yf - yi) / (double)(xf - xi);

        for (i = xi; i <= xf; i++)
        {
            vg_set_pixel(i, (unsigned long)yt, color);
            yt += m;
        }
    } else if (xi > xf) {
        if (yi < yf)
            m = (double)(yi - yf) / (double)(xf - xi);
        else
            m = (double)(yi - yf) / (double)(xi - xf);

        for (i = xi; i >= xf; i--) {
            vg_set_pixel(i, (unsigned long)yt, color);
            yt += m;
        }
    } else {
        return -1;
    }

    return OK;
}

int vg_draw_rectangle(unsigned long x1, unsigned long y1, unsigned long x2,
        unsigned long y2, unsigned long color) {
    int x, y;

    if (x1 > h_res || y1 > v_res || x2 > h_res || y2 > v_res) {
        return -1;
    }

    if (x1 < x2 && y1 < y2)
        for (x = x1; x <= x2; x++)
            for (y = y1; y <= y2; y++)
                vg_set_pixel(x, y, color);
    else if (x2 < x1 && y1 < y2)
        for (x = x2; x <= x1; x++)
            for (y = y1; y <= y2; y++)
                vg_set_pixel(x, y, color);
    else if (x2 < x1 && y2 < y1)
        for (x = x2; x <= x1; x++)
            for (y = y2; y <= y1; y++)
                vg_set_pixel(x, y, color);
    else if (x1 < x2 && y2 < y1)
        for (x = x1; x <= x2; x++)
            for (y = y2; y <= y1; y++)
                vg_set_pixel(x, y, color);

    return OK;
}

int vg_draw_circle(int x0, int y0, int radius, unsigned long color)
{
  /* Based on a sample code from Wikipedia */

  int f, ddF_x, ddF_y, x, y;

  f = 1 - radius;
  ddF_x = 1;
  ddF_y = -2 * radius;
  x = 0;
  y = radius;

  vg_set_pixel(x0, y0 + radius, color);
  vg_set_pixel(x0, y0 - radius, color);
  vg_draw_line(x0, y0 + radius, x0, y0 - radius, color);

  vg_set_pixel(x0 + radius, y0, color);
  vg_set_pixel(x0 - radius, y0, color);
  vg_draw_line(x0 + radius, y0, x0 - radius, y0, color);

  while(x < y)
  {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }

    x++;
    ddF_x += 2;
    f += ddF_x;
    /* vg_set_pixel(x0 + x, y0 + y, color); */
    /* vg_set_pixel(x0 - x, y0 + y, color); */
    vg_draw_line(x0 + x, y0 + y, x0 - x, y0 + y, color);

    /* vg_set_pixel(x0 + x, y0 - y, color); */
    /* vg_set_pixel(x0 - x, y0 - y, color); */
    vg_draw_line(x0 + x, y0 - y, x0 - x, y0 - y, color);

    /* vg_set_pixel(x0 + y, y0 + x, color); */
    /* vg_set_pixel(x0 - y, y0 + x, color); */
    vg_draw_line(x0 + y, y0 + x, x0 - y, y0 + x, color);

    /* vg_set_pixel(x0 + y, y0 - x, color); */
    /* vg_set_pixel(x0 - y, y0 - x, color); */
    vg_draw_line(x0 + y, y0 - x, x0 - y, y0 - x, color);
  }

  return 0;
}

void vg_swap_buffer(void) {
    _vg_swap_buffer(temp_video_mem, video_mem, vram_size);
}

int vg_exit(void) {
    struct reg86u reg86;

    reg86.u.b.intno = BIOS_VIDEO_SERVICE; /* BIOS video services */

    reg86.u.b.ah = VIDEO_MODE; /* Set Video Mode function */
    reg86.u.b.al = SET_TEXT_MODE; /* 80x25 text mode*/


    free(temp_video_mem);

    if (sys_int86(&reg86) != OK) {
        printf("vg_exit: sys_int86() failed \n");
        return -1;
    } else
        return OK;
}
