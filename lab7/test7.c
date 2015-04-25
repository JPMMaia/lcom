#include "SP.h"
#include "SP_D.h"

int ser_test_conf(unsigned short base_addr)
{
	if(base_addr != SP_COM1 && base_addr != SP_COM2)
	{
		printf("\n\t Warning: accessing wrong COM.\n");
		return -1;
	}


	if(sp_show_config(base_addr) != 0)
		return -1;

	return 0;
}

int ser_test_set(unsigned short base_addr, unsigned long bits, unsigned long stop,
		long parity, unsigned long rate)
{
	if(sp_set_config(base_addr, bits, stop, parity, rate) != 0)
		return -1;

	return 0;
}

int ser_test_poll(unsigned short base_addr, unsigned char tx, unsigned long bits,
		unsigned long stop, long parity, unsigned long rate,
		int stringc, char* strings[])
{
	unsigned long i;
	char* data;

	if(sp_set_config(base_addr, bits, stop, parity, rate) != 0)
		return -1;

	if(tx == 1)
	{
		if(sp_transmit_data(base_addr, stringc, strings) != 0)
			return -1;

		printf("\n\t Data successfully sent \n");
	}
	else if(tx == 0)
	{
		data = sp_receive_data(base_addr);
		if(data == 0)
			return -1;

		printf("\n\t Data received:\n\t\t%s", data);

		free(data);
	}

	return 0;
}

int ser_test_int(unsigned short base_addr, unsigned char tx, unsigned long bits,
		unsigned long stop, long parity, unsigned long rate,
		int stringc, char* strings[])
{
	if(sp_set_config(base_addr, bits, stop, parity, rate) != 0)
		return -1;


	if(tx == 1)
	{
		if(sp_int(base_addr, stringc, strings) != 0)
			return -1;

		printf("\n\t Data successfully sent \n");
	}



	return 0;
}

int ser_test_fifo(unsigned short base_addr, unsigned char tx, unsigned long bits, unsigned long stop,
		long parity, unsigned long rate, int stringc, char *strings[], unsigned long delay)
{
	if(sp_set_config(base_addr, bits, stop, parity, rate) != 0)
		return -1;

	if(tx == 1)
	{
		if(sp_fifo(base_addr, stringc, strings, delay) != 0)
			return -1;

		printf("\n\t Data successfully sent \n");
	}

	return 0;
}
