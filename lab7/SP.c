#include "SP.h"
#include "SP_D.h"

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>

static int sp_hook_id;

int sp_error_detection(unsigned short reg_lsr)
{
	unsigned long error;

	if(reg_lsr & SP_LSR_OE)
		error = 1;
	else if(reg_lsr & SP_LSR_PE)
		error = 2;
	else if(reg_lsr & SP_LSR_FE)
		error = 3;
	else
		error = 0;

	if(error != 0)
		printf("\n\t Error Detected: %d\n", error);

	return error;
}
int sp_lcr_set_dlab(unsigned short base_addr, unsigned long enable)
{
	unsigned long reg_lcr;

	if(sys_inb(base_addr + SP_REG_LCR, &reg_lcr) != OK)
	{
		printf("\n\t SP.c :: sp_lcr_set_dlab : sys_inb() failed \n");
		return -1;
	}

	if(enable == 0)
		reg_lcr &= ~SP_LCR_DLAB;
	else
		reg_lcr |= SP_LCR_DLAB;

	if(sys_outb(base_addr + SP_REG_LCR, reg_lcr) != OK)
	{
		printf("\n\t SP.c :: sp_lcr_set_dlab : sys_outb() failed \n");
		return -1;
	}

	return 0;
}

int sp_show_config(unsigned short base_addr)
{
	if(sp_lcr_show(base_addr) != 0)
		return -1;

	if(sp_ier_show(base_addr) != 0)
		return -1;

	return 0;
}
int sp_lcr_show(unsigned short base_addr)
{
	unsigned long reg_lcr, word_length, stop_bits, set_break_enabled, dlab, bit_rate;
	long parity;

	// Reading Line Control Register
	if(sys_inb(base_addr + SP_REG_LCR, &reg_lcr) != OK)
	{
		printf("\n\t SP.c :: sp_show_config : sys_inb() failed \n");
		return -1;
	}

	// Calculating word-length
	sp_lcr_word_length(reg_lcr, &word_length);

	// Retrieving stop-bits
	sp_lcr_stop_bits(reg_lcr, &stop_bits);

	// Calculating bit-rate
	if(sp_lcr_bit_rate(base_addr, reg_lcr, &bit_rate) != 0)
		return -1;

	// Calculating parity
	sp_lcr_parity(reg_lcr, &parity);

	// Retrieving set-break-enabled
	sp_lcr_break(reg_lcr, &set_break_enabled);

	// Retrieving DLAB
	sp_lcr_dlab(reg_lcr, &dlab);


	// Printing info
	printf("\n\t LCR Reg:\t\t\t0x%X", reg_lcr);

	// Printing word-length
	printf("\n\t Word Length:\t\t\t");
	printf("%d", word_length);

	// Printing number of Stop Bits
	printf("\n\t No. Stop Bits:\t\t\t");
	printf("%d", stop_bits);

	// Printing bit-rate
	printf("\n\t Bit Rate:\t\t\t");
	printf("%d Hz", bit_rate);

	// Printing parity
	printf("\n\t Parity:\t\t\t");
	if(parity == -1)
		printf("None");
	else if(parity == 0)
		printf("Even");
	else if(parity == 1)
		printf("Odd");
	else
		printf("%d", parity);

	// Priting set-break-enabled
	printf("\n\t Set Break:\t\t\t");
	if(set_break_enabled == 1)
		printf("Enabled");
	else
		printf("Disabled");

	// Printing DLAB
	printf("\n\t DLAB:\t\t\t\t");
	if(dlab == 1)
		printf("Selects DL");
	else
		printf("Selects Data");

	printf("\n\n");

	return 0;
}
void sp_lcr_word_length(unsigned long reg_lcr, unsigned long* word_length)
{
	*word_length = (reg_lcr & 0x3) + 5;
}
int sp_lcr_bit_rate(unsigned short base_addr, unsigned long reg_lcr, unsigned long* bit_rate)
{
	unsigned long lsb, msb, divisor;

	// Checking if DLAB is set and setting it if it is not
	if((reg_lcr & SP_LCR_DLAB) == 0)
	{
		reg_lcr |= SP_LCR_DLAB;
		if(sys_outb(base_addr + SP_REG_LCR, reg_lcr) != OK)
		{
			printf("\n\t SP.c :: sp_lcr_bit_rate : sys_outb() #1 failed \n");
			return -1;
		}
	}

	// Read lsb and msb and calculating divisor
	if(sys_inb(base_addr + SP_REG_DLL, &lsb) != OK)
	{
		printf("\n\t SP.c :: sp_lcr_bit_rate : sys_inb() #1 failed \n");
		return -1;
	}
	if(sys_inb(base_addr + SP_REG_DLM, &msb) != OK)
	{
		printf("\n\t SP.c :: sp_lcr_bit_rate : sys_inb() #2 failed \n");
		return -1;
	}
	divisor = (msb << 8) | lsb;

	// Calculating bit-rate
	*bit_rate = SP_BIT_RATE_NUM / divisor;

	// Resetting DLAB
	reg_lcr &= ~SP_LCR_DLAB;
	if(sys_outb(base_addr + SP_REG_LCR, reg_lcr) != OK)
	{
		printf("\n\t SP.c :: sp_lcr_bit_rate : sys_outb() #2 failed \n");
		return -1;
	}

	return 0;
}
void sp_lcr_stop_bits(unsigned long reg_lcr, unsigned long* stop_bits)
{
	if(reg_lcr & SP_LCR_2STOP)
		*stop_bits = 2;
	else
		*stop_bits = 1;
}
void sp_lcr_parity(unsigned long reg_lcr, long* parity)
{
	if((reg_lcr & SP_LCR_NONE_) == 0)
	{
		*parity = -1;
		return;
	}
	if(reg_lcr & SP_LCR_EVEN)
		*parity = 0;
	else
		*parity = 1;
}
void sp_lcr_break(unsigned long reg_lcr, unsigned long* set_break_enabled)
{
	if(reg_lcr & SP_LCR_BREAK)
		*set_break_enabled = 1;
	else
		*set_break_enabled = 0;
}
void sp_lcr_dlab(unsigned long reg_lcr, unsigned long* dlab)
{
	if(reg_lcr & SP_LCR_DLAB)
		*dlab = 1;
	else
		*dlab = 0;
}

int sp_ier_show(unsigned short base_addr)
{
	unsigned long reg_ier, rdi_enabled, tei_enabled, lsi_enabled, msi_enabled;

	// Reading IER Register
	if(sys_inb(base_addr + SP_REG_IER, &reg_ier) != OK)
	{
		printf("SP.c :: sp_ier_show : sys_inb() failed");
		return -1;
	}

	// Retrieving data
	sp_ier_rdi(reg_ier, &rdi_enabled);
	sp_ier_tei(reg_ier, &tei_enabled);
	sp_ier_lsi(reg_ier, &lsi_enabled);
	sp_ier_msi(reg_ier, &msi_enabled);

	// Printing data
	printf("\n\t IER Reg:\t\t\t0x%X", reg_ier);

	printf("\n\t Received Data Int:\t\t");
	if(rdi_enabled == 1)
		printf("Enabled");
	else
		printf("Disabled");

	printf("\n\t Transmitter Empty Int:\t\t");
	if(tei_enabled == 1)
		printf("Enabled");
	else
		printf("Disabled");

	printf("\n\t Receiver Line Status Int:\t");
	if(lsi_enabled == 1)
		printf("Enabled");
	else
		printf("Disabled");

	printf("\n\t Modem Status Int:\t\t");
	if(msi_enabled == 1)
		printf("Enabled");
	else
		printf("Disabled");

	printf("\n\n");

	return 0;
}
void sp_ier_rdi(unsigned long reg_ier, unsigned long* rdi_enabled)
{
	if(reg_ier & SP_IER_RDI)
		*rdi_enabled = 1;
	else
		*rdi_enabled = 0;
}
void sp_ier_tei(unsigned long reg_ier, unsigned long* tei_enabled)
{
	if(reg_ier & SP_IER_TEI)
		*tei_enabled = 1;
	else
		*tei_enabled = 0;
}
void sp_ier_lsi(unsigned long reg_ier, unsigned long* lsi_enabled)
{
	if(reg_ier & SP_IER_LSI)
		*lsi_enabled = 1;
	else
		*lsi_enabled = 0;
}
void sp_ier_msi(unsigned long reg_ier, unsigned long* msi_enabled)
{
	if(reg_ier & SP_IER_MSI)
		*msi_enabled = 1;
	else
		*msi_enabled = 0;
}

int sp_set_config(unsigned short base_addr, unsigned long bits, unsigned long stop, long parity, unsigned long rate)
{
	unsigned long byte;

	byte = 0;
	if(sp_set_bits(bits, &byte) != 0)
		return -1;

	if(sp_set_stop(stop, &byte) != 0)
		return -1;

	if(sp_set_parity(parity, &byte) != 0)
		return -1;

	if(sys_outb(base_addr + SP_REG_LCR, byte) != OK)
	{
		printf("\n\t SP.c :: sp_set_config : sys_outb() failed \n");
		return -1;
	}

	if(sp_set_rate(base_addr, rate) != 0)
		return -1;

	return 0;

}
int sp_set_bits(unsigned long bits, unsigned long* byte)
{
	if(bits < 5 || bits > 8)
	{
		printf("\n\t SP.c :: sp_set_bits : bits out of range \n");
		return -1;
	}

	bits -= 5;
	*byte &= 0xFC; // bit 0 and 1 set to 0
	*byte |= bits;

	return 0;
}
int sp_set_stop(unsigned long stop, unsigned long* byte)
{
	if(stop != 1 && stop != 2)
	{
		printf("\n\t SP.c :: sp_set_stop : stop out of range \n");
		return -1;
	}

	if(stop == 2)
		*byte |= SP_LCR_2STOP;

	return 0;
}
int sp_set_parity(long parity, unsigned long* byte)
{
	if(parity < -1 || parity > 1)
	{
		printf("\n\t SP.c :: sp_set_stop : parity out of range \n");
		return -1;
	}

	if(parity == -1)
		return 0;

	*byte |= SP_LCR_NONE_;

	if(parity == 0)
		*byte |= SP_LCR_EVEN;

	return 0;
}
int sp_set_rate(unsigned short base_addr, unsigned long rate)
{
	unsigned long divisor, lsb, msb;

	// Calculating divisor
	divisor = SP_BIT_RATE_NUM / rate;

	// Calculating LSB and MSB
	lsb = divisor & 0xFF;
	msb = (divisor >> 8) & 0xFF;

	// Setting DLAB to 1
	if(sp_lcr_set_dlab(base_addr, 1) != 0)
		return -1;

	// Setting LSB
	if(sys_outb(base_addr + SP_REG_DLL, lsb) != 0)
	{
		printf("\n\t SP.c :: sp_set_rate : sys_outb() #1 failed \n");
		return -1;
	}
	// Setting MSB
	if(sys_outb(base_addr + SP_REG_DLM, msb) != 0)
	{
		printf("\n\t SP.c :: sp_set_rate : sys_outb() #2 failed \n");
		return -1;
	}

	// Setting DLAB to 0
	if(sp_lcr_set_dlab(base_addr, 0) != 0)
		return -1;

	return 0;
}

int sp_transmit(unsigned short base_addr, unsigned long data)
{
	unsigned long reg_lsr, reg_lcr;

	// Read Line Status Register
	if(sys_inb(base_addr + SP_REG_LSR, &reg_lsr) != OK)
	{
		printf("\n\t SP.c :: sp_transmit : sys_inb() #1 failed \n");
		return -1;
	}

	// Waiting for transmitter ready
	while(!(reg_lsr & SP_LSR_THRE))
	{
		tickdelay(micros_to_ticks(DELAY_US));

		if(sys_inb(base_addr + SP_REG_LSR, &reg_lsr) != OK)
		{
			printf("\n\t SP.c :: sp_transmit : sys_inb() #2 failed \n");
			return -1;
		}
	}

	// Send character
	if(sys_outb(base_addr + SP_REG_THR, data) != OK)
	{
		printf("\n\t SP.c :: sp_transmit : sys_outb() failed \n");
		return -1;
	}

	return 0;
}
int sp_transmit_data(unsigned short base_addr, int stringc, char* strings[])
{
	unsigned long i;

	// If Divisor Latch Access bit is set to 1, then we need to set it to 0
	if(sp_lcr_set_dlab(base_addr, 0) != 0)
		return -1;

	for(i = 0; i < stringc - 1; i++)
		if(sp_transmit_word(base_addr, strings[i], 0) != 0)
			return -1;

	if(sp_transmit_word(base_addr, strings[i], 1) != 0)
		return -1;

	return 0;
}
int sp_transmit_word(unsigned short base_addr, char* word, unsigned long last)
{
	unsigned long c, i;

	i = 0;
	do
	{
		c = word[i];
		if(c == '\0')
		{
			if(last == 0)
				c = ' ';
			else
				c = '.';
		}

		if(sp_transmit(base_addr, c) != 0)
			return -1;

		i++;
	} while(c != ' ' && c != '.');

	return 0;
}

int sp_receive(unsigned short base_addr, unsigned long* data)
{
	unsigned long reg_lsr, reg_ier;

	if(sys_inb(base_addr + SP_REG_IER, &reg_ier) != OK)
	{
		printf("\n\t SP.c :: sp_receive : sys_inb() #1 failed \n");
		return -1;
	}
	if(sys_outb(base_addr + SP_REG_IER, reg_ier & 0xF0) != OK)
	{
		printf("\n\t SP.c :: sp_receive : sys_outb() #1 failed \n");
		return -1;
	}

	// Read Line Status Register
	if(sys_inb(base_addr + SP_REG_LSR, &reg_lsr) != OK)
	{
		printf("\n\t SP.c :: sp_receive : sys_inb() #2 failed \n");
		return -1;
	}

	// Error detection
	if(sp_error_detection(reg_lsr) != 0)
		return -1;

	// If Divisor Latch Access bit is set to 1, then we need to set it to 0
	if(sp_lcr_set_dlab(base_addr, 0) != 0)
		return -1;


	// Waiting for Receiver Ready bit
	while(!(reg_lsr & SP_LSR_RD))
	{
		tickdelay(micros_to_ticks(DELAY_US));

		if(sys_inb(base_addr + SP_REG_LSR, &reg_lsr) != OK)
		{
			printf("\n\t SP.c :: sp_receive : sys_inb() #3 failed \n");
			return -1;
		}
	}

	// Read character
	if(sys_inb(base_addr + SP_REG_RBR, data) != OK)
	{
		printf("\n\t SP.c :: sp_receive : sys_inb() #4 failed \n");
		return -1;
	}

	return 0;
}
char* sp_receive_data(unsigned short base_addr)
{
	unsigned long c, i, end;
	char strings[256];
	char* data;

	// If Divisor Latch Access bit is set to 1, then we need to set it to 0
	if(sp_lcr_set_dlab(base_addr, 0) != 0)
		return 0;

	printf("\n\t Receiving data \n");

	end = 0;
	for(i = 0; end != 1; i++)
	{
		if(sp_receive(base_addr, &c) != 0)
			return 0;

		if(c == '.')
			end = 1;

		strings[i] = (char) c;
	}
	strings[i] = '\0';

	// Allocating memory for data
	data = (char*) malloc(sizeof(char) * i);
	strncpy(data, strings, i);

	return data;
}

int sp_enable_int(unsigned short base_addr, unsigned long bitmask)
{
	unsigned long reg_ier;

	if(sys_inb(base_addr + SP_REG_IER, &reg_ier) != 0)
		return -1;

	reg_ier &= 0xF0;

	if(sys_outb(base_addr + SP_REG_IER, reg_ier | bitmask) != 0)
		return -1;

	return 0;
}
int sp_int_handler(unsigned short base_addr, unsigned long data)
{
	unsigned long reg_iir, reg_ier;

	sys_inb(base_addr + SP_REG_IIR, &reg_iir);

	if(!(reg_iir & SP_IIR_INTP_))
	{
		if(SP_IIR_THRE & reg_iir)
		{
			if(sp_transmit_int(base_addr, data) != 0)
				return -1;
		}
	}

	return 0;
}
int sp_int(unsigned short base_addr, int stringc, char* strings[])
{
	unsigned long done, irq_set, r, reg_lsr, i, j, size, ipc_status, end;
	unsigned long data[255];
	message msg;

	size = 0;
	end = 0;
	for(i = 0; i < stringc && end == 0; i++)
	{
		for(j = 0; end == 0; j++)
		{
			if(strings[i][j] == (unsigned long) '\0' && i == stringc - 1)
			{
				data[size] = '.';
				end = 1;
			}
			else if(strings[i][j] == (unsigned long) '\0')
			{
				data[size] = (unsigned long) ' ';
				size ++;
				break;
			}
			else
				data[size] = strings[i][j];

			size++;
		}
	}

	// Disable all interrupts
	sp_enable_int(base_addr, SP_IER_NONE);

	// Check for errors
	if(sys_inb(base_addr + SP_REG_LSR, &reg_lsr) != 0)
		return -1;

	if(sp_error_detection(reg_lsr) != 0)
		return -1;

	// Enable tx interrupt
	sp_enable_int(base_addr, SP_IER_TEI);

	// Subscribe interrupts
	irq_set = sp_subscribe(base_addr);

	i = 0;
	sp_transmit_int(base_addr, data[i]);
	i++;

	done = 0;
	while(!done)
	{
		if((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
		{
			printf("\t test_can: driver_receive() failed: %d \n", r);
			continue;
		}

		if(is_ipc_notify(ipc_status))
		{
			switch(_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:
				if(msg.NOTIFY_ARG & irq_set)
				{
					if(sp_int_handler(base_addr, data[i]) != 0)
						return -1;
					i++;

					if(size == i)
						done = 1;
				}
				break;
			default:
				break;
			}
		}
	}

	// Disable all interrupts
	sp_enable_int(base_addr, SP_IER_NONE);

	// Unsubscribe int
	sp_unsubscribe(base_addr);
}
int sp_transmit_int(unsigned short base_addr, unsigned long data)
{
	// Send character
	if(sys_outb(base_addr + SP_REG_THR, data) != OK)
	{
		printf("\n\t SP.c :: sp_transmit_int : sys_outb() failed \n");
		return -1;
	}

	return 0;
}

int sp_subscribe(unsigned short base_addr)
{
	int bitmask;

	if(base_addr == SP_COM1)
		sp_hook_id = SP_IRQ4;
	else
		sp_hook_id = SP_IRQ3;

	bitmask = BIT(sp_hook_id);

	if(sys_irqsetpolicy(sp_hook_id, IRQ_REENABLE | IRQ_EXCLUSIVE, &sp_hook_id) != OK)
	{
		printf("\t sp_subscribe: sys_irqsetpolicy() failed. \n");
		return -1;
	}

	if(sys_irqenable(&sp_hook_id) != OK)
	{
		printf("\t sp_subscribe: sys_irqenable() failed. \n");
		return -1;
	}

	return bitmask;
}
int sp_unsubscribe(unsigned short base_addr)
{
	if(sys_irqdisable(&sp_hook_id) != OK)
	{
		printf("\t sp_unsubscribe: sys_disable() failed. \n");
		return -1;
	}

	if(sys_irqrmpolicy(&sp_hook_id) != OK)
	{
		printf("\t sp_unsubscribe: sys_irqrmpolicy() failed. \n");
		return -1;
	}

	return 0;
}

int sp_fifo(unsigned short base_addr, int stringc, char* strings[], unsigned long delay)
{
	unsigned long data[255], i, j, end, size, reg_lsr;

	size = 0;
	end = 0;
	for(i = 0; i < stringc && end == 0; i++)
	{
		for(j = 0; end == 0; j++)
		{
			if(strings[i][j] == (unsigned long) '\0' && i == stringc - 1)
			{
				data[size] = '.';
				end = 1;
			}
			else if(strings[i][j] == (unsigned long) '\0')
			{
				data[size] = (unsigned long) ' ';
				size ++;
				break;
			}
			else
				data[size] = strings[i][j];

			size++;
		}
	}

	// Disable all interrupts
	sp_enable_int(base_addr, SP_IER_NONE);

	// Check for errors
	if(sys_inb(base_addr + SP_REG_LSR, &reg_lsr) != 0)
		return -1;

	if(sp_error_detection(reg_lsr) != 0)
		return -1;

	// Enable fifo
	sp_enable_fifo(base_addr, SP_FCR_EB | SP_FCR_CLEAR1 | SP_FCR_CLEAR2);

	// Handler fifo
	if(sp_fifo_handler(base_addr, data, delay) != 0)
		return -1;

	// Disable FIFO
	sp_enable_fifo(base_addr, 0);

	return 0;
}
int sp_enable_fifo(unsigned short base_addr, unsigned long bitmask)
{
	unsigned long reg_fcr;

	if(sys_inb(base_addr + SP_REG_FCR, &reg_fcr) != 0)
		return -1;

	reg_fcr &= 0xF8;

	if(sys_outb(base_addr + SP_REG_FCR, reg_fcr | bitmask) != 0)
		return -1;

	return 0;
}
int sp_fifo_handler(unsigned short base_addr, unsigned long* data, unsigned long delay)
{
	unsigned long reg_lsr, i;

	sys_inb(base_addr + SP_REG_LSR, &reg_lsr);

	i = 0;
	do
	{

		while(reg_lsr & SP_LSR_THRE)
		{
			if(reg_lsr & SP_LSR_FIFOE)
			{
				printf("\n\t FIFO Error \n");
				return -1;
			}

			sys_outb(base_addr + SP_REG_THR, data[i]);
			if(data[i] == (unsigned long) '.')
				return 0;

			i++;

			sys_inb(base_addr + SP_REG_LSR, &reg_lsr);
		}

		tickdelay(micros_to_ticks(delay));

	} while(data[i] != (unsigned long) '.');

	return 0;
}
