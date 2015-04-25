#include <stdlib.h>
#include "video_gr.h"

#define WAIT_TIME_S 5

//static int proc_args(int argc, char *argv[]);
//static unsigned long parse_ulong(char *str, int base);
//static long parse_long(char *str, int base);
//static void print_usage(char *argv[]);

int main(int argc, char **argv)
{
	sef_startup();

	vg_init(0x105);

	vg_draw_line(5,100,60,120,9);


	sleep(WAIT_TIME_S);
	vg_exit();


}

