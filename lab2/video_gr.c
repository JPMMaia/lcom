#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "vbe.h"

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab I304 is 0xF0000000
 * Better run my version of lab2 as follows:
 *     service run `pwd`/lab2 -args "mode 0x105"
 */
#define VRAM_PHYS_ADDR	0xF0000000
#define H_RES             1024
#define V_RES		  768
#define BITS_PER_PIXEL	  8

/* Private global variables */

static char *video_mem;		/* Process address to which VRAM is 
mapped */

static unsigned h_res;		/* Horizontal screen resolution in 
pixels */
static unsigned v_res;		/* Vertical screen resolution in pixels 
 */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

void * vg_init(unsigned short mode) {

	struct reg86u reg86;
	vbe_mode_info_t info;
	struct mem_range range;
	int r;

	reg86.u.b.intno = 0x10;
	reg86.u.w.ax = 0x4F02;
	reg86.u.w.bx = 1<<14| mode;
	if(sys_int86(&reg86) != OK)
	{
		printf("set_vbe_mode: sys_int86() failed \n");
		return NULL;
	}

	//vbe_get_mode_info(mode, &info);

	info.PhysBasePtr = VRAM_PHYS_ADDR;
	info.XResolution = H_RES;
	info.YResolution = V_RES;
	info.BitsPerPixel = BITS_PER_PIXEL;


	range.mr_base = info.PhysBasePtr;
	range.mr_limit = range.mr_base + info.XResolution * info.YResolution * info.BitsPerPixel;

	if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &range)))
		panic("video_txt: sys_privctl (ADD_MEM) failed: %d\n", r);

	video_mem = vm_map_phys(SELF, (void *)range.mr_base, info.XResolution * info.YResolution * info.BitsPerPixel);

	h_res = info.XResolution;
	v_res = info.YResolution;
	bits_per_pixel = info.BitsPerPixel;


	return NULL;
}

int vg_fill(unsigned long color)
{
	unsigned long i, j;


	while(i<h_res*v_res)
	{
		video_mem[i]=color;
		i++;
	}

	return 0;
}

int vg_set_pixel(unsigned long x, unsigned long y, unsigned long color) 
{
	unsigned long i;

	if(x<0 || y<0 || x>h_res || y>v_res)
		return -1;

	i=x + y*h_res;
	video_mem[i]=(char) color;

	return 0;
}

long vg_get_pixel(unsigned long x, unsigned long y)
{
	if(x<0 || y<0 || x>h_res || y>v_res)
		return -1;


	return video_mem[x+y*h_res];

}

int vg_draw_line(unsigned long xi, unsigned long yi, 
		unsigned long xf, unsigned long yf, unsigned long
		color)
{
	double m,b;
	unsigned long position, i;

	if(xi<0 || yi<0 || xi>h_res || yi>v_res || xf<0 || yf<0 || xf>h_res || yf>v_res)
		return -1;

	m = (double) (yf-yi) / (double) (xf-xi);
	b =  (double) yi-m * (double) xi;
	i=xi;

	while(i<=xf)
	{
		unsigned long y = (unsigned long) (m*i+b+0.5);
		position=i+y*h_res;
		video_mem[position]=color;
		i++;
	}


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

