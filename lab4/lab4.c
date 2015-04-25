#include <minix/syslib.h>
#include <minix/drivers.h>
#include "test4.h"

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
			"\t service run %s -args \"scan\" - test_scan() \n"
			"\t service run %s -args \"leds <n> <*leds>\" - test_leds() \n",
			argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[])
{
	unsigned long n, i;
	unsigned short *leds;
	char *str;
	long num;

	if(strncmp(argv[1], "scan", strlen("scan")) == 0)
	{
		if(argc != 2)
		{
			printf("test4: wrong number of arguments for test of test_scan() \n");
			return 1;
		}

		printf("test4:: test_scan() \n");

		return test_scan();
	}
	else if(strncmp(argv[1], "leds", strlen("leds")) == 0)
	{
		n = parse_ulong(argv[2], 16);
		if(argc != 3 + n)
		{
			printf("test4: wrong number of arguments for test of test_leds() \n");
			return 1;
		}

		leds = (unsigned short*) malloc(sizeof(unsigned long) * n);
		for(i = 0; i < n; i++)
			leds[i] = (unsigned short) parse_ulong(argv[i+3], 16);

		printf("test4:: test_leds(%X, %p) \n", n, leds);

		test_leds(n, leds);

		free(leds);
	}
	else
	{
		printf("test4: non valid function \"%s\" to test\n", argv[1]);
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

