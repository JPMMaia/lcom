#include <minix/drivers.h>
#include <sys/video.h>
#include <sys/mman.h>

#include <assert.h>

#include "vt_info.h"

#include "video_txt.h"

/* Private global variables */

static char *video_mem;		/* Address to which VRAM is mapped */

static unsigned scr_width;	/* Width of screen in columns */
static unsigned scr_lines;	/* Height of screen in lines */

void vt_fill(char ch, char attr)
{
	unsigned limit = scr_width*scr_lines*2;

	unsigned i = 0;
	while(i < limit)
	{
		video_mem[i] = ch;
		video_mem[i+1] = attr;

		i += 2;
	}

}

void vt_blank()
{
	unsigned limit = scr_width*scr_lines*2;

	unsigned i = 0;
	while(i < limit)
	{
		video_mem[i] = ' ';
		video_mem[i+1] = 0x00;

		i += 2;
	}

}

int vt_print_char(char ch, char attr, int r, int c)
{
	if(r >= scr_lines || c >= scr_width || c < 0 || r < 0)
		return -1;

	int index = (scr_width*r + c)*2;

	video_mem[index]=ch;
	video_mem[index+1]=attr;
	return 0;


}

int vt_print_string(char *str, char attr, int r, int c)
{
	unsigned i = 0;
	if(r >= scr_lines || c >= scr_width || c < 0 || r < 0 || str[i]==0x00)
		return -1;

	int index = (scr_width*r + c)*2;


	while(str[i] != 0x00)
	{
		video_mem[index]=str[i];
		video_mem[index+1]=attr;
		i++;
		index+=2;
	}
	return 0;
}

int vt_print_int(int num, char attr, int r, int c)
{
	if(r >= scr_lines || c >= scr_width || c < 0 || r < 0 )
		return -1;

	int index = (scr_width*r + c)*2;

	int unity, remainder, i = 0;
	char* str[20];
	str[19] = 0;

	while(1)
	{
		unity = num % 10, remainder = num / 10;
		if(unity != 0 || remainder != 0)
		{
			str[i] = unity + 0x30;

			num = remainder;
			i++;
		}
		if(remainder == 0 && unity == 0)
			break;

	}
	str[i] = 0x0;
	i--;
	while(i >= 0)
	{
		video_mem[index]=str[i];
		video_mem[index+1]=attr;
		i--;
		index+=2;
	}
	return 0;
}


int vt_draw_frame(int width, int height, char attr, int r, int c)
{
	if(r >= scr_lines || c >= scr_width || c < 0 || r < 0 || width<=1 || width > scr_width || height <=1 || height > scr_lines)
		return -1;

	int index = (scr_width*r + c)*2;
	const int ULC  =index;
	const int URC = index+width*2-2;
	const int LLC = index+((height-1)*scr_width*2);
	const int LRC = index+((height-1)*scr_width*2)+width*2-2;

	video_mem[ULC]=0xDA;
	video_mem[URC]=0xBF;
	video_mem[LRC]=0xD9;
	video_mem[LLC]= 0xC0;

	video_mem[ULC+1]=attr;
	video_mem[URC+1]=attr;
	video_mem[LRC+1]=attr;
	video_mem[LLC+1]= attr;

	int i = 0;
	while(i< width-2)
	{
		video_mem[ULC+i*2+2]=0xC4;
		video_mem[ULC+i*2+3]=attr;
		i += 1;
	}
	i=0;

	while(i < width-2)
	{
		video_mem[LLC+i*2+2]=0xC4;
		video_mem[LLC+i*2+3]=attr;
		i += 1;
	}
	i=0;

	while(i< height-2)
	{
		video_mem[ULC+(i*2+2)*scr_width]=0xB3;
		video_mem[ULC+(i*2+2)*scr_width+1]=attr;
		i += 1;
	}

	i=0;

	while(i< height-2)
	{
		video_mem[URC+(i*2+2)*scr_width]=0xB3;
		video_mem[URC+(i*2+2)*scr_width+1]=attr;
		i += 1;
	}

	return 0;
}

/*
 * THIS FUNCTION IS FINALIZED, do NOT touch it
 */

char *vt_init(vt_info_t *vi_p) {

	int r;
	struct mem_range mr;

	/* Allow memory mapping */

	mr.mr_base = (phys_bytes)(vi_p->vram_base);
	mr.mr_limit = mr.mr_base + vi_p->vram_size;

	if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("video_txt: sys_privctl (ADD_MEM) failed: %d\n", r);

	/* Map memory */

	video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vi_p->vram_size);

	if(video_mem == MAP_FAILED)
		panic("video_txt couldn't map video memory");

	/* Save text mode resolution */

	scr_lines = vi_p->scr_lines;
	scr_width = vi_p->scr_width;

	return video_mem;
}
