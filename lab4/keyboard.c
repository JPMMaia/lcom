#include <minix/syslib.h>
#include <minix/drivers.h>
#include "keyboard.h"

static int kbHookId = 1;
static int kbLedsOn[3] = {0, 0, 0};

int kb_subscribe_int()
{
	int bitmask;

	kbHookId = 1;
	bitmask = BIT(kbHookId);

	if(sys_irqsetpolicy(KB_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbHookId) != OK)
	{
		printf("\t kb_subscribe_int: sys_irqsetpolicy() failed. \n");
		return -1;
	}

	if(sys_irqenable(&kbHookId) != OK)
	{
		printf("\t kb_subscribe_int: sys_irqenable() failed. \n");
		return -1;
	}

	return bitmask;
}

int kb_unsubscribe_int()
{
	if(sys_irqdisable(&kbHookId) != OK)
	{
		printf("\t kb_unsubscribe_int: sys_disable() failed. \n");
		return -1;
	}

	if(sys_irqrmpolicy(&kbHookId) != OK)
	{
		printf("\t kb_unsubscribe_int: sys_irqrmpolicy() failed. \n");
		return -1;
	}

	return 0;
}

int kb_read_data(unsigned long *data)
{
	unsigned long byte, stat, timeout;

	timeout = 10;
	while(timeout != 0)
	{
		if(sys_inb(KB_STAT_REG, &stat) != OK)
		{
			printf("\t kb_handle_int: sys_inb() #1 failed. \n");
			return -1;
		}

		if(stat & OBF)
		{
			if(sys_inb(KB_OUT_BUF, &byte) != OK)
			{
				printf("\t kb_handle_int: sys_inb() #2 failed. \n");
				return -1;
			}

			if((stat & (PAR_ERR | TO_ERR)) == 0)
			{
				*data = byte;
				return 0;
			}
			else
			{
				printf("\t kb_handle_int: parity or timeout error occurred. \n");
				return -1;
			}
		}
		tickdelay(micros_to_ticks(DELAY_US));

		timeout--;
	}

	return 0;
}

int kb_write_data(unsigned long cmd, unsigned long arg)
{
	unsigned long stat, timeout, readData, flag;

	arg = BIT(arg);

	readData = KBE_ACK;
	flag = 0;
	timeout = 10;
	while(timeout != 0)
	{
		if(sys_inb(KB_STAT_REG, &stat) != OK)
		{
			printf("\t kb_write_data: sys_inb() failed. \n");
			return -1;
		}
		if((stat & (IBF | PAR_ERR | TO_ERR)) == 0)
		{
			sys_outb(0x60, 0xED);
			tickdelay(micros_to_ticks(20000));
			sys_outb(0x60, arg);

			return 0;
			/*if(flag == 0 || flag == 1)
				{
					sys_outb(KBC_CMD_REG, cmd);
					sys_inb(KB_OUT_BUF, &readData);

					switch(readData)
					{
					case KBE_ACK:
						flag = 0;
						break;
					case KBE_RESEND: case KBE_ERROR:
						flag = 1;
					}
				}
				if(flag = 0 || flag == 2)
				{
					sys_outb(KBD_LED, arg);
					sys_inb(KB_OUT_BUF, &readData);
					switch(readData)
					{
					case KBE_ACK:
						flag = 0;
						break;
					case KBE_ERROR:
						flag = 1;
						break;
					case KBE_RESEND:
						flag = 2;
						break;
					}
				}*/

		}
		else
		{
			printf("\t kb_write_data: IBF set or parity or timeout error occurred. \n");
			return -1;
		}

		tickdelay(micros_to_ticks(DELAY_US));

		timeout--;
	}

	return 0;
}

int kb_handle_int()
{
	unsigned long data;

	if(kb_read_data(&data) != OK)
		return -1;

	if(data & BREAKCODE)
		printf("\t Breakcode: %X \n", data);
	else
		printf("\t Makecode: %X \n", data);

	return data;
}

int kb_handle_int_leds(unsigned long arg)
{
	if(kbLedsOn[arg] == 1)
		kbLedsOn[arg] = 0;
	else
		kbLedsOn[arg] = 1;
}
