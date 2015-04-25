#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>

#include "KeyboardClass.h"

struct keyboard_class
{
	unsigned long data;
	bool kbLedsOn[3];
};


/////////////////////////////// PUBLIC ///////////////////////////////////////

//============================= LIFECYCLE ====================================

KeyboardClass* newKeyboard()
{
	KeyboardClass* Keyboard;
	Keyboard = (KeyboardClass*) malloc(sizeof(KeyboardClass));

	return Keyboard;
}
void deleteKeyboard(KeyboardClass* Keyboard)
{
	free(Keyboard);
}


//============================= OPERATIONS ===================================

bool KeyboardHandleInt(KeyboardClass* Keyboard)
{
	if(!KeyboardReadData(Keyboard))
		return false;

	return true;
}
void KeyboardShowData(KeyboardClass* Keyboard)
{
	if(Keyboard->data & KB_BREAKCODE)
		printf("\t Breakcode: %X \n", Keyboard->data);
	else
		printf("\t Makecode: %X \n", Keyboard->data);
}


//=============================   ACCESS   ===================================

unsigned long KeyboardGetData(KeyboardClass* Keyboard)
{
	return Keyboard->data;
}


/////////////////////////////// PRIVATE //////////////////////////////////////

//============================= OPERATIONS ===================================

static bool KeyboardReadData(KeyboardClass* Keyboard)
{
	unsigned long byte, stat, timeout;

	timeout = 10;
	while(timeout != 0)
	{
		if(sys_inb(KB_REG_STATUS, &stat) != OK)
		{
			printf("\t KeyboardClass.c :: KeyboardReadData : sys_inb() #1 failed. \n");
			return false;
		}

		if(stat & KB_ST_OUT_BUF_FULL)
		{
			if(sys_inb(KB_REG_OUT_BUF, &byte) != OK)
			{
				printf("\t KeyboardClass.c :: KeyboardReadData : sys_inb() #2 failed. \n");
				return false;
			}

			if((stat & (KB_ST_PARITY_ERR | KB_ST_TIME_OUT_ERR)) == 0)
			{
				Keyboard->data = byte;
				return true;
			}
			else
			{
				printf("\t KeyboardClass.c :: KeyboardReadData : parity or timeout error occurred. \n");
				return false;
			}
		}
		tickdelay(micros_to_ticks(KB_DELAY_US));

		timeout--;
	}

	return false;
}
static bool KeyboardWriteData(unsigned long cmd, unsigned long* arg)
{
	unsigned long stat, timeout, readData, flag, argument;

	if(arg != 0)
		argument = BIT(*arg);

	readData = KB_ACKB_ACK;
	flag = 0;
	timeout = 10;
	while(timeout != 0)
	{
		if(sys_inb(KB_REG_STATUS, &stat) != OK)
		{
			printf("\t KeyboardClass.c :: KeyboardWriteData : sys_inb() failed. \n");
			return false;
		}
		if((stat & (KB_ST_IN_BUF_FULL | KB_ST_PARITY_ERR | KB_ST_TIME_OUT_ERR)) == 0)
		{
			sys_outb(KB_REG_IN_BUF, cmd);
			tickdelay(micros_to_ticks(20000));
			if(arg != 0)
				sys_outb(KB_REG_IN_BUF, argument);

			return true;
		}
		else
		{
			printf("\t KeyboardClass.c :: KeyboardWriteData : IBF set or parity or timeout error occurred. \n");
			return false;
		}

		tickdelay(micros_to_ticks(KB_DELAY_US));

		timeout--;
	}

	return false;
}
