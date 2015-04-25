#include <minix/syslib.h>
#include <minix/drivers.h>
#include "test7.h"

#define NLAB	7
#define FUNC1	"ser_test_conf"
#define FUNC2	"ser_test_set"
#define FUNC3	"ser_test_poll"
#define FUNC4	"ser_test_int"
#define FUNC5	"ser_test_fifo"
#define ARG1	"conf"
#define ARG2	"set"
#define ARG3	"poll"
#define ARG4	"int"
#define ARG5	"fifo"
#define SP_COM1			0x3F8
#define SP_COM2			0x2F8

static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static long parse_long(char *str, int base);
static void print_usage(char *argv[]);
static unsigned long parse_base_addr(char *str, int base);
static long parse_parity(char* str);
static unsigned long parse_tx(char* str);

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
			"\t service run %s -args \"%s 1|2\" - %s \n"
			"\t service run %s -args \"%s 1|2 <bits> <stop> even|odd|none <rate>\" - %s \n"
			"\t service run %s -args \"%s 1|2 tx|rx <bits> <stop> even|odd|none <rate> <*strings[]>\" - %s \n"
			"\t service run %s -args \"%s\" 1|2 tx|rx <bits> <stop> even|odd|none <rate> <*strings[]> - %s \n"
			"\t service run %s -args \"%s\" 1|2 tx|rx <bits> <stop> even|odd|none <rate> <*strings[]> <delay> - %s \n",
			argv[0], ARG1, FUNC1,
			argv[0], ARG2, FUNC2,
			argv[0], ARG3, FUNC3,
			argv[0], ARG4, FUNC4,
			argv[0], ARG5, FUNC5);
}

static int proc_args(int argc, char *argv[])
{
	unsigned long base_addr, bits, stop, rate, delay;
	long parity;
	int stringc, i;
	char** strings;
	unsigned char tx;

	if(strncmp(argv[1], ARG1, strlen(ARG1)) == 0)
	{
		if(argc != 3)
		{
			printf("test%d: wrong number of arguments for test of %s \n", NLAB, FUNC1);
			return 1;
		}
		// Base addr
		base_addr = parse_base_addr(argv[2], 16);
		if(base_addr == -1)
			return -1;

		printf("test%d:: %s(%X) \n", NLAB, FUNC1, base_addr);

		return ser_test_conf(base_addr);
	}
	else if(strncmp(argv[1], ARG2, strlen(ARG2)) == 0)
	{
		if(argc != 7)
		{
			printf("test%d: wrong number of arguments for test of %s \n", NLAB, FUNC2);
			return 1;
		}
		// Base addr
		base_addr = parse_base_addr(argv[2], 16);
		if(base_addr == -1)
			return -1;

		bits = parse_ulong(argv[3], 16);
		stop = parse_ulong(argv[4], 16);

		parity = parse_parity(argv[5]);
		if(parity == -2)
			return -1;

		rate = parse_ulong(argv[6], 16);

		printf("test%d:: %s(%X, %u, %u, %d, %u) \n", NLAB, FUNC2, base_addr, bits, stop, parity, rate);

		return ser_test_set(base_addr, bits, stop, parity, rate);
	}
	else if(strncmp(argv[1], ARG3, strlen(ARG3)) == 0)
	{
		tx = parse_tx(argv[3]);
		if(tx == -1)
			return -1;


		if(argc < 9 && tx == 1)
		{
			printf("test%d: wrong number of arguments for test of %s \n", NLAB, FUNC3);
			return 1;
		}
		else if(argc != 8 && tx == 0)
		{
			printf("test%d: wrong number of arguments for test of %s \n", NLAB, FUNC3);
			return 1;
		}
		// Base addr
		base_addr = parse_base_addr(argv[2], 16);
		if(base_addr == -1)
			return -1;

		bits = parse_ulong(argv[4], 16);
		stop = parse_ulong(argv[5], 16);
		parity = parse_parity(argv[6]);
		if(parity == -2)
			return -1;


		rate = parse_ulong(argv[7], 16);


		if(tx == 1)
		{
			stringc = argc - 8;

			strings = (char**) malloc(sizeof(char*) * stringc);

			for(i = 0; i < stringc; i++)
				strings[i] = argv[8+i];
		}
		else if(tx == 0)
			stringc = 0;

		printf("test%d:: %s(%X, %u, %u, %u, %d, %u, %u, %p) \n", NLAB, FUNC3, base_addr, tx, bits, stop, parity, rate, stringc, strings);

		ser_test_poll(base_addr, tx, bits, stop, parity, rate, stringc, strings);

		free(strings);

		return 0;
	}
	else if(strncmp(argv[1], ARG4, strlen(ARG4)) == 0)
	{
		tx = parse_tx(argv[3]);
		if(tx == -1)
			return -1;


		if(argc < 9 && tx == 1)
		{
			printf("test%d: wrong number of arguments for test of %s \n", NLAB, FUNC4);
			return 1;
		}
		else if(argc != 8 && tx == 0)
		{
			printf("test%d: wrong number of arguments for test of %s \n", NLAB, FUNC4);
			return 1;
		}
		// Base addr
		base_addr = parse_base_addr(argv[2], 16);
		if(base_addr == -1)
			return -1;

		bits = parse_ulong(argv[4], 16);
		stop = parse_ulong(argv[5], 16);
		parity = parse_parity(argv[6]);
		if(parity == -2)
			return -1;


		rate = parse_ulong(argv[7], 16);


		if(tx == 1)
		{
			stringc = argc - 8;

			strings = (char**) malloc(sizeof(char*) * stringc);

			for(i = 0; i < stringc; i++)
				strings[i] = argv[8+i];
		}
		else if(tx == 0)
			stringc = 0;


		printf("test%d:: %s(%X, %u, %u, %u, %d, %u, %u, %p) \n", NLAB, FUNC4, base_addr, tx, bits, stop, parity, rate, stringc, strings);

		ser_test_int(base_addr, tx, bits, stop, parity, rate, stringc, strings);

		free(strings);

		return 0;
	}
	else if(strncmp(argv[1], ARG5, strlen(ARG5)) == 0)
	{
		tx = parse_tx(argv[3]);
		if(tx == -1)
			return -1;


		if(argc < 10 && tx == 1)
		{
			printf("test%d: wrong number of arguments for test of %s \n", NLAB, FUNC5);
			return 1;
		}
		else if(argc != 9 && tx == 0)
		{
			printf("test%d: wrong number of arguments for test of %s \n", NLAB, FUNC5);
			return 1;
		}
		// Base addr
		base_addr = parse_base_addr(argv[2], 16);
		if(base_addr == -1)
			return -1;

		bits = parse_ulong(argv[4], 16);
		stop = parse_ulong(argv[5], 16);
		parity = parse_parity(argv[6]);
		if(parity == -2)
			return -1;


		rate = parse_ulong(argv[7], 16);


		if(tx == 1)
		{
			stringc = argc - 9;

			strings = (char**) malloc(sizeof(char*) * stringc);

			for(i = 0; i < stringc; i++)
				strings[i] = argv[8+i];
		}
		else if(tx == 0)
			stringc = 0;

		delay = parse_ulong(argv[argc-1], 16);

		printf("test%d:: %s(%X, %u, %u, %u, %d, %u, %u, %p, %u) \n", NLAB, FUNC5, base_addr, tx, bits, stop, parity, rate, stringc, strings, delay);

		ser_test_fifo(base_addr, tx, bits, stop, parity, rate, stringc, strings, delay);

		free(strings);

		return 0;
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

static unsigned long parse_base_addr(char* str, int base)
{
	unsigned long base_addr;
	base_addr = parse_ulong(str, base);
	if(base_addr == 1)
		base_addr = SP_COM1;
	else if(base_addr == 2)
		base_addr = SP_COM2;
	else
	{
		printf("\n\t Warning: accessing wrong COM.\n");
		return -1;
	}

	return base_addr;
}

static long parse_parity(char* str)
{
	unsigned long parity;

	if(strncmp(str, "even", strlen("even")) == 0)
		parity = 0;
	else if(strncmp(str, "odd", strlen("odd")) == 0)
		parity = 1;
	else if(strncmp(str, "none", strlen("none")) == 0)
		parity = -1;
	else
	{
		printf("\n\t Warning: Wrong parity.\n");
		return -2;
	}

	return parity;
}

static unsigned long parse_tx(char* str)
{
	unsigned long tx;

	if(strncmp(str, "tx", strlen("tx")) == 0)
		tx = 1;
	else if(strncmp(str, "rx", strlen("rx")) == 0)
		tx = 0;
	else
	{
		printf("\n\t Warning: Wrong parity.\n");
		return -1;
	}

	return tx;
}
