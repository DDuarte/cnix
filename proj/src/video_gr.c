#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/bitmap.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

//#define NGRAPHICS

#include "vbe.h"
#include "video_gr.h"
#include "utilities.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#define LINEAR_MODEL_BIT 14

#define VBE_MODE 0x4F
#define VIDEO_MODE 0x00

#define SET_VBE_MODE 0x02
#define SET_TEXT_MODE 0x03

#define BIOS_VIDEO_SERVICE 0x10

static unsigned BASE_H_RES = 1024;
static unsigned BASE_V_RES = 768;

/* Private global variables */

static char *video_mem; /* Process address to which VRAM is mapped */
static char *temp_video_mem; /* address of the temporary buffer */
static unsigned int vram_size;

static int _vg_set_absolute_pixel(long x, long y, unsigned long color);

static long h_res; /* Horizontal screen resolution in pixels */
static long v_res; /* Vertical screen resolution in pixels */

static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */
static unsigned bytes_per_pixel; /* Number of VRAM bytes per pixel (at least one) */
static vbe_mode_info_t vmi_p;

/* Font related variables */
static FT_Library _library;
static FT_Face _face;
static FT_Bool _use_kerning;

static struct {
    unsigned long redMask;
    unsigned long greenMask;
    unsigned long blueMask;
    unsigned long redPosition;
    unsigned long greenPosition;
    unsigned long bluePosition;
} gr_color;

int vg_get_h_res(void) { LOG
    return h_res;
}
int vg_get_v_res(void) { LOG
    return v_res;
}

unsigned long vg_color_rgb(unsigned long r, unsigned long g, unsigned long b) { LOG
    return ((((b * gr_color.blueMask)  / 255) << gr_color.bluePosition)  |
            (((g * gr_color.greenMask) / 255) << gr_color.greenPosition) |
            (((r * gr_color.redMask)   / 255) << gr_color.redPosition));
}

int vg_init_FreeType(void) { LOG
    int error;

    error = FT_Init_FreeType(&_library);
    if (error) {
        printf("_init_FreeType: FT_Init_FreeType failed with error code %d.\n", error);
        return error;
    }

    error = FT_New_Face(_library, "/root/Inconsolata.ttf", 0, &_face);
    if (error == FT_Err_Unknown_File_Format) {
        printf("_init_FreeType: FT_New_Face - font format unsupported.\n");
        return error;
    } else if (error) {
        printf("_init_FreeType: FT_New_Face failed with error code %d.\n", error);
        return error;
    }

    _use_kerning = FT_HAS_KERNING(_face);

    return 0;
}

void* vg_init(unsigned short mode) { LOG
    struct mem_range mr;
    struct reg86u r;

    r.u.b.intno = BIOS_VIDEO_SERVICE;
    r.u.b.ah = VBE_MODE;
    r.u.b.al = SET_VBE_MODE;
    r.u.w.bx = BIT(LINEAR_MODEL_BIT) | mode;

    if (sys_int86(&r))
    {
        printf("vg_init: failed in sys_int86");
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

    vram_size = h_res * (v_res+1) * bytes_per_pixel;

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

int vg_fill(unsigned long color) { LOG
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

long vg_scale_x(long x) { LOG
    if (h_res == BASE_H_RES)
        return x;
    if (x == BASE_H_RES)
        return h_res;

    return (((double)h_res * (double)x) / (double)BASE_H_RES);
}

long vg_scale_y(long y) { LOG
    if (v_res == BASE_V_RES)
        return y;
    if (y == BASE_V_RES)
        return v_res;

    return (((double)v_res * (double)y) / (double)BASE_V_RES);
}

int vg_set_pixel(long x, long y, unsigned long color) { LOG

    if (x > BASE_H_RES || y > BASE_V_RES || x < 0 || y < 0) {
        return -1;
    }

    x = vg_scale_x(x);
    y = vg_scale_y(y);

    return _vg_set_absolute_pixel(x, y, color);
}

static int _vg_set_absolute_pixel(long x, long y, unsigned long color) { LOG
    int i;
    char* vptr;

    if (x > h_res || y > v_res || x < 0 || y < 0) {
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

long vg_get_pixel(long x, long y) { LOG
    int i;
    long res;
    char* vptr;

    if (x > BASE_H_RES || y > BASE_V_RES || x < 0 || y < 0) {
        return -1;
    }

    x = vg_scale_x(x);
    y = vg_scale_y(y);

    res = 0;

    i = (y * h_res + x) * bytes_per_pixel;
    vptr = &temp_video_mem[i];
    for (i = 0; i < bytes_per_pixel; i++) {
        res <<= 8;
        res |= *vptr;
        vptr++;
    }
    return res;
}

int _vg_draw_absolute_line(long xi, long yi, long xf, long yf, unsigned long color) { LOG
    int i;

    if ((xi > xf) || (xi == xf && yi > yf)) {
        swapl(&xi, &xf);
        swapl(&yi, &yf);
    }


    if ((xi < 0 && xf < 0) || (yi < 0 && yf < 0) || (xi > h_res && xf > h_res) || (yi > v_res && yf > v_res)) {
        return -1;
    }

    if (xi == xf) {
        if (yi < 0)
            yi = 0;

        if (yf > v_res)
            yf = v_res;

        for (i = yi; i < yf; i++)
                _vg_set_absolute_pixel(xi, i, color);
    } else if (yi == yf) {
        xf = (xf > h_res ? h_res : xf);
        for (i = ((xi < 0) ? 0 : xi); i <= xf; i++)
            _vg_set_absolute_pixel(i, (unsigned long)yi, color);
    } else {
        double m = (double)(yf - yi) / (double)(xf - xi);
        double b = (double)yi - m * (double)xi;

        if (m != 0) {
            if (xi < 0 || yi < 0) {
                if (m > 0) {
                    if (b >= 0) {
                        xi = 0;
                        yi = b;
                    } else {
                        xi = - b / m;
                        yi = 0;
                    }
                } else {
                    if (xi < 0) {
                        xi = 0;
                        yi = b;
                    }
                    if (yf < 0) {
                        xf = - b / m;
                        yf = 0;
                    }
                }
            }
            if (xf > h_res) {
                xf = h_res;
                yf = m * h_res + b;
            }

            if (yf > v_res) {
                xf = (v_res - b) / m;
                yf = v_res;
            } else {
                if (xi < 0)
                    xi = 0;

                if (xf > h_res)
                    xf = h_res;
            }
        }

        if (abs(m) <= 1) {
            double yt = yi;
            for (i = xi; i <= xf; i++) {
                _vg_set_absolute_pixel(i, (unsigned long)yt, color);
                yt += m;
            }
        } else {
            double xt = xi;
            double invm = 1.0 / m;
            for (i = yi; i <= yf; i++) {
                _vg_set_absolute_pixel(xt, (unsigned long)i, color);
                xt += invm;
            }
        }
    }

    return 0;
}

int vg_draw_line(long xi, long yi, long xf, long yf, unsigned long color) { LOG
    if (xi > BASE_H_RES && yi > BASE_V_RES && xf > BASE_H_RES && yf > BASE_V_RES) {
        return -1;
    }

    xi = vg_scale_x(xi);
    yi = vg_scale_y(yi);
    xf = vg_scale_x(xf);
    yf = vg_scale_y(yf);

    return _vg_draw_absolute_line(xi, yi, xf, yf, color);
}

int vg_draw_rectangle(long x1, long y1, long x2,
        long y2, unsigned long color) { LOG
    int x, y;

    if (x1 > BASE_H_RES && y1 > BASE_V_RES && x2 > BASE_H_RES && y2 > BASE_V_RES) {
        return -1;
    }

    x1 = vg_scale_x(x1);
    y1 = vg_scale_y(y1);
    x2 = vg_scale_x(x2);
    y2 = vg_scale_y(y2);

    if (x1 < x2 && y1 < y2)
        for (x = x1; x <= x2; x++)
            for (y = y1; y <= y2; y++)
                _vg_set_absolute_pixel(x, y, color);
    else if (x2 < x1 && y1 < y2)
        for (x = x2; x <= x1; x++)
            for (y = y1; y <= y2; y++)
                _vg_set_absolute_pixel(x, y, color);
    else if (x2 < x1 && y2 < y1)
        for (x = x2; x <= x1; x++)
            for (y = y2; y <= y1; y++)
                _vg_set_absolute_pixel(x, y, color);
    else if (x1 < x2 && y2 < y1)
        for (x = x1; x <= x2; x++)
            for (y = y2; y <= y1; y++)
                _vg_set_absolute_pixel(x, y, color);

    return 0;
}

int vg_draw_circle(int x0, int y0, int radius, unsigned long color) { LOG
    /* Based on a sample code from Wikipedia */

    int f, ddF_x, ddF_y, x, y;

    x0 = vg_scale_x(x0);
    y0 = vg_scale_y(y0);
    radius = vg_scale_x(radius);

    f = 1 - radius;
    ddF_x = 1;
    ddF_y = -2 * radius;
    x = 0;
    y = radius;

    _vg_set_absolute_pixel(x0, y0 + radius, color);
    _vg_set_absolute_pixel(x0, y0 - radius, color);
    _vg_draw_absolute_line(x0, y0 + radius, x0, y0 - radius, color);

    _vg_set_absolute_pixel(x0 + radius, y0, color);
    _vg_set_absolute_pixel(x0 - radius, y0, color);
    _vg_draw_absolute_line(x0 + radius, y0, x0 - radius, y0, color);

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
        _vg_draw_absolute_line(x0 + x, y0 + y, x0 - x, y0 + y, color);

        /* vg_set_pixel(x0 + x, y0 - y, color); */
        /* vg_set_pixel(x0 - x, y0 - y, color); */
        _vg_draw_absolute_line(x0 + x, y0 - y, x0 - x, y0 - y, color);

        /* vg_set_pixel(x0 + y, y0 + x, color); */
        /* vg_set_pixel(x0 - y, y0 + x, color); */
        _vg_draw_absolute_line(x0 + y, y0 + x, x0 - y, y0 + x, color);

        /* vg_set_pixel(x0 + y, y0 - x, color); */
        /* vg_set_pixel(x0 - y, y0 - x, color); */
        _vg_draw_absolute_line(x0 + y, y0 - x, x0 - y, y0 - x, color);
    }

    return 0;
}

void vg_swap_buffer(void) { LOG
    _vg_swap_buffer(temp_video_mem, video_mem, vram_size);
}

int vg_exit(void) { LOG
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

int vg_ft_draw_bitmap(FT_Bitmap* bitmap, int x, int y, unsigned long color) { LOG
    int i, j, p, q;

    int x_max = x + bitmap->width;
    int y_max = y + bitmap->rows;

    for (i = x, p = 0; i < x_max; i++, p++) {
        for (j = y, q = 0; j < y_max; j++, q++) {
            if (i < 0 || j < 0 || i >= h_res || j >= v_res) {
                continue;
            }

            unsigned char c = bitmap->buffer[q * bitmap->width + p];
            if (c != 0) /* 0 is background */
                vg_set_pixel(i, j, color);
        }
    }

    return 0;
}

int vg_draw_char(char character, int size, unsigned long x, unsigned long y, unsigned long color) { LOG
        int error;

    error = FT_Set_Pixel_Sizes(_face, 0, size);
    if (error) {
        printf("vg_draw_char: FT_Set_Pixel_Sizes failed with error code %d.\n", error);
        return error;
    }

    FT_GlyphSlot slot = _face->glyph;
    FT_UInt glyph_index;

    glyph_index = FT_Get_Char_Index(_face, character);

    error = FT_Load_Glyph(_face, glyph_index, FT_LOAD_RENDER);
    if (error) {
        printf("vg_draw_char: FT_Load_Glyph failed with error code %d for character %c.\n", error, character);
        return error;
    }

    error = vg_ft_draw_bitmap(&slot->bitmap, x + slot->bitmap_left, y - slot->bitmap_top, color);
    if (error) {
        printf("vg_draw_char: vg_ft_draw_bitmap failed with error code %d for character %c.\n", error, character);
        return error;
    }

    return 0;
}

int vg_draw_string(char* str, int size, unsigned long x,
        unsigned long y, unsigned long color) { LOG
    int error;

    error = FT_Set_Pixel_Sizes(_face, 0, size);
    if (error) {
        printf("vg_draw_string: FT_Set_Pixel_Sizes failed with error code %d.\n", error);
        return error;
    }

    FT_GlyphSlot slot = _face->glyph;
    FT_UInt previous;
    int pen_x, pen_y, n;
    FT_UInt glyph_index;
    int num_chars;

    pen_x = x;
    pen_y = y;
    previous = 0;
    num_chars = strlen(str);

    for (n = 0; n < num_chars; n++) {

        glyph_index = FT_Get_Char_Index(_face, str[n]);

        if (_use_kerning && previous && glyph_index) {
            FT_Vector delta;

            FT_Get_Kerning(_face, previous, glyph_index, FT_KERNING_DEFAULT, &delta);

            pen_x += delta.x >> 6;
        }

        error = FT_Load_Glyph(_face, glyph_index, FT_LOAD_RENDER);
        if (error)
            continue;

        vg_ft_draw_bitmap(&slot->bitmap, pen_x + slot->bitmap_left, pen_y - slot->bitmap_top, color);
        pen_x += slot->advance.x >> 6;
        previous = glyph_index;
    }

    return 0;
}
