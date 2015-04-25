#include <minix/syslib.h>
#include <minix/drivers.h>
#include "test5.h"

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
			"\t service run %s -args \"packet\" - test_packet() \n"
			"\t service run %s -args \"asynch <duration>\" - test_async() \n"
			"\t service run %s -args \"config\" - test_config() \n",
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[])
{
	unsigned long duration;

	if(strncmp(argv[1], "packet", strlen("packet")) == 0)
	{
		if(argc != 2)
		{
			printf("test5: wrong number of arguments for test of test_packet() \n");
			return 1;
		}

		printf("test5:: test_packet() \n");

		return test_packet();
	}
	else if(strncmp(argv[1], "asynch", strlen("asynch")) == 0)
	{
		if(argc != 3)
		{
			printf("test5: wrong number of arguments for test of test_asynch() \n");
			return 1;
		}

		duration = parse_ulong(argv[2], 16);

		printf("test5:: test_asynch(%u) \n", duration);

		return test_asynch(duration);
	}
	else if(strncmp(argv[1], "config", strlen("config")) == 0)
	{
		if(argc != 2)
		{
			printf("test5: wrong number of arguments for test of test_config() \n");
			return 1;
		}

		printf("test5:: test_config() \n");

		return test_config();
	}
	else
	{
		printf("test5: non valid function \"%s\" to test\n", argv[1]);
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
