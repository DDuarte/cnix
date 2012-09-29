#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "vbe.h"
#include "lmlib.h"

#define LINEAR_MODEL_BIT 14
#define BIT(n) (0x1 << (n))

#define VBE_MODE_INFO_BLOCK_SIZE sizeof(vbe_mode_info_t)

#define VBE_MODE 0x4F
#define RET_VBE_MODE_INFO 0x01
#define BIOS_VIDEO_SERVICE 0x10

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p)
{
    struct reg86u r;
    mmap_t map;

    if (mode < 0x100)
    {
        printf("lab2/vbe_get_mode_info: mode is non-VBE (%u)", mode);
        return -1;
    }

    if (lm_init())
    {
        printf("lab2/vbe_get_mode_info: failed in lm_init");
        return -1;
    }


    lm_alloc(VBE_MODE_INFO_BLOCK_SIZE, &map);

    r.u.b.ah = VBE_MODE;
    r.u.b.al = RET_VBE_MODE_INFO;
    r.u.w.es = PB2BASE(map.phys);
    r.u.w.di = PB2OFF(map.phys);
    r.u.w.cx = mode | BIT(LINEAR_MODEL_BIT);;
    r.u.b.intno = BIOS_VIDEO_SERVICE;

    if (sys_int86(&r))
    {
        printf("lab2/vbe_get_mode_info: failed in sys_int86");
        return -1;
    }

    *vmi_p = *(vbe_mode_info_t*)map.virtual; /* NEEDS to be called after sys_int86 because reasons */

    lm_free(&map);

    return r.u.w.ax; /* VBE return status */
}
