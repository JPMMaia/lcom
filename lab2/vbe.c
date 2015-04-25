#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "vbe.h"
#include "lmlib.h"

#define LINEAR_MODEL_BIT 14

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p) {

	mmap_t address;
	struct reg86u r;

	lm_init();
	lm_alloc(256, &address);

	r.u.w.ax = 0x4F01;
	r.u.w.es = PB2BASE(address.phys);
	r.u.w.di = PB2OFF(address.phys);
	r.u.w.cx = 1 << 14 | mode;
	r.u.b.intno = 0x10;

	sys_int86(&r);

	lm_free(&address);


	return 1;
}

