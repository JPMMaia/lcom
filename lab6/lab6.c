#include <minix/syslib.h>
#include <minix/drivers.h>
#include "test6.h"

#define NLAB	6
#define FUNC1	"rtc_test_conf()"
#define FUNC2	"rtc_test_date()"
#define FUNC3	"rtc_test_int()"
#define ARG1	"conf"
#define ARG2	"date"
#define ARG3	"int"

static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static long parse_long(char *str, int base);
static void print_usage(char *argv[]);

int main(int argc, char **argv)
{
	endpoint_t ep;
	char name[256];
	int priv_f;

	// Initialize Service
	sef_startup();

	// Fetch our endpoint
	sys_whoami(&ep, name, 256, &priv_f);

	// Enable IO-sensitive operations for ourselves
	sys_enable_iop(ep);

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
			"\t service run %s -args \"%s\" - %s \n"
			"\t service run %s -args \"%s\" - %s \n"
			"\t service run %s -args \"%s <delta>\" - %s \n",
			argv[0], ARG1, FUNC1,
			argv[0], ARG2, FUNC2,
			argv[0], ARG3, FUNC3);
}

static int proc_args(int argc, char *argv[])
{
	unsigned long delta;

	if(strncmp(argv[1], ARG1, strlen(ARG1)) == 0)
	{
		if(argc != 2)
		{
			printf("test%d: wrong number of arguments for test of %s \n", NLAB, FUNC1);
			return 1;
		}

		printf("test%d:: %s \n", NLAB, FUNC1);

		return rtc_test_conf();
	}
	else if(strncmp(argv[1], ARG2, strlen(ARG2)) == 0)
	{
		if(argc != 2)
		{
			printf("test%d: wrong number of arguments for test of %s \n", NLAB, FUNC2);
			return 1;
		}

		printf("test%d:: %s \n", NLAB, FUNC2);

		return rtc_test_date();
	}
	else if(strncmp(argv[1], ARG3, strlen(ARG3)) == 0)
	{
		if(argc != 3)
		{
			printf("test%d: wrong number of arguments for test of %s \n", NLAB, FUNC3);
			return 1;
		}
		delta = parse_ulong(argv[2], 16);

		printf("test%d:: %s \n", NLAB, FUNC3);

		return rtc_test_int(delta);
	}
	else
	{
		printf("test%d: non valid function \"%s\" to test\n", NLAB, argv[1]);
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
