#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "vbe.h"
#include "lmlib.h"

#define LINEAR_MODEL_BIT 14
#define VBE_MODE_INFO_BLOCK_SIZE 256
#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p) {

	struct reg86u reg;
	vbe_mode_info_t* vmi_temp;
	mmap_t mem;
	int s;

	if (lm_init() == 0) {
		lm_alloc(VBE_MODE_INFO_BLOCK_SIZE, &mem);
		vmi_temp = mem.virtual;
		reg.u.b.intno = 0x10;
		reg.u.b.ah = 0x4F;
		reg.u.b.al = 0x01;
		reg.u.w.cx = 0x1 << 14 | mode;
		reg.u.w.es = PB2BASE(mem.phys);
		reg.u.w.di = PB2OFF(mem.phys);
		s = sys_int86(&reg);
		*vmi_p = *vmi_temp;
		lm_free(&mem);
		return s == OK ? 0 : 1;
	} else {
		return 1;
	}
}

