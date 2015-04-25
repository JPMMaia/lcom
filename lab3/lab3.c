#include "timer.h"

static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static long parse_long(char *str, int base);
static void print_usage(char *argv[]);

int main(int argc, char **argv)
{
	// Initialize Service
	sef_startup();


	if(argc == 1)
	{
		print_usage(argv);
		return 0;
	}
	else
	{
		proc_args(argc, argv);
	}

	return 0;
}

static void print_usage(char *argv[])
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"square <freq>\" - timer_test_square() \n"
			"\t service run %s -args \"int <time>\" - timer_test_int() \n"
			"\t service run %s -args \"config <timer>\" - timer_test_config() \n",
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[])
{
	unsigned long freq, time, timer;
	char *str;
	long num;

	if(strncmp(argv[1], "square", strlen("square")) == 0)
	{
		if(argc != 3)
		{
			printf("timer: wrong number of arguments for test of timer_test_square() \n");
			return 1;
		}
		freq = parse_ulong(argv[2], 16);

		printf("timer:: timer_test_square(%X)\n", (unsigned) freq);

		return timer_test_square(freq);
	}
	else if(strncmp(argv[1], "int", strlen("int")) == 0)
	{
		if(argc != 3)
		{
			printf("timer: wrong number of arguments for test of timer_test_int() \n");
			return 1;
		}
		time = parse_ulong(argv[2], 16);

		printf("timer:: timer_test_int(%X)\n", (unsigned) time);

		return timer_test_int(time);
	}
	else if(strncmp(argv[1], "config", strlen("config")) == 0)
	{
		if(argc != 3)
		{
			printf("timer: wrong no of arguments for test of timer_test_config() \n");
			return 1;
		}
		timer = parse_ulong(argv[2], 16);

		printf("timer:: timer_test_config(%X)\n", timer);

		return timer_test_config(timer);
	}
	else
	{
		printf("timer: non valid function \"%s\" to test\n", argv[1]);
		return 1;
	}
}

static unsigned long parse_ulong(char *str, int base)
{
	unsigned long val;

	val = strtoul(str, 0, 0);

	return val;
}

static long parse_long(char *str, int base)
{
	long val;

	val = strtol(str, 0, 0);

	return val;
}

