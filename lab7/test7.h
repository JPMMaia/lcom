int ser_test_conf(unsigned short base_addr);

int ser_test_set(unsigned short base_addr, unsigned long bits, unsigned long stop,
	     long parity, /* -1: none, 0: even, 1: odd */
	     unsigned long rate);

int ser_test_poll(unsigned short base_addr, unsigned char tx, unsigned long bits, 	unsigned long stop, long parity, unsigned long rate, int stringc, char *strings[]);

int ser_test_int(unsigned short base_addr, unsigned char tx, unsigned long bits,
		unsigned long stop, long parity, unsigned long rate,
		int stringc, char* strings[]);

int ser_test_fifo(/* details to be provided */);
