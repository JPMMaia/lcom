#include "TimerClass.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

struct timer_class
{
	unsigned long config[3];
	char operatingMode[3];
	char access[3];
	char BCD[3];

	unsigned int counter;
};

/////////////////////////////// PUBLIC ///////////////////////////////////////

//============================= LIFECYCLE ====================================

TimerClass* newTimer()
{
	TimerClass* Timer;
	Timer = malloc(sizeof(TimerClass));

	return Timer;
}
void deleteTimer(TimerClass* Timer)
{
	free(Timer);
}

bool TimerInitialize(TimerClass* Timer)
{
	unsigned long timer;

	Timer->counter = 0;

	for(timer = 0; timer < 3; timer++)
		if(TimerUpdateConfig(Timer, timer) != true)
			return false;

	return true;
}


//============================= OPERATIONS ===================================

bool TimerSetSquare(TimerClass* Timer, unsigned long timer, unsigned long freq)
{
	port_t port;
	unsigned long byte;
	char st;
	byte = 0;

	if(timer > 2)
	{
		printf("\t TimerClass.c :: TimerSetSquare : Timer > 2. \n");
		return false;
	}

	// Calculating port.
	port = 0x40 + timer;

	// Update timer config
	TimerUpdateConfig(Timer, timer);

	// Setting LSB (BCD/binary)
	byte |= Timer->BCD[timer];

	// Reset bit 7 and 6
	byte &= 0x3F;
	// Selecting timer
	if(timer == 1)
		byte |= TM_CTRL_SEL1;
	else if(timer == 2)
		byte |= TM_CTRL_SEL2;

	byte |= TM_CTRL_LSB_MSB; 	// Read/Write LSB and MSB.
	byte |= TM_CTRL_SQR_WAVE;	// Mode 3.

	// Writing Control Word into Control Word Register.
	if(sys_outb(TM_REG_CTRL, byte) != OK)
	{
		printf("\t TimerClass.c :: TimerSetSquare : sys_outb() #1 failed. \n");
		return false;
	}

	// Calculating divisor.
	byte = TM_FREQ / freq;

	// Writing LSB.
	if(sys_outb(port, (byte & 0xFF)) != OK)
	{
		printf("\t TimerClass.c :: TimerSetSquare : sys_outb() #2 failed. \n");
		return false;
	}

	// Writing MSB.
	if(sys_outb(port, (byte >> 8)) != OK)
	{
		printf("\t TimerClass.c :: TimerSetSquare : sys_outb() #3 failed. \n");
		return false;
	}

	TimerSetConfig(Timer, timer, byte);

	return true;
}
void TimerHandleInt(TimerClass* Timer)
{
	Timer->counter++;
}
bool TimerShowConfig(TimerClass* Timer, unsigned long timer)
{
	unsigned char st;
	st = Timer->config[timer];

	if(timer > 2)
	{
		printf("\t TimerClass.c :: TimerShowConfig : Timer > 2. \n");
		return false;
	}

	printf("st: %X", st);

	printf("\n\t Timer %u:\n\n", timer);

	printf("\t\t Type of Access: ");
	switch(Timer->access[timer])
	{
	case 1:
		printf("Read/Write least significant byte only.\n");
		break;
	case 2:
		printf("Read/Write most significant byte only.\n");
		break;
	case 3:
		printf("Read/Write least significant byte first, then most significant byte.\n");
		break;
	}

	printf("\t\t Programmed Mode: ");
	printf("%d.\n", Timer->operatingMode[timer]);

	printf("\t\t Counting Mode: ");
	if(Timer->BCD[timer] == 1)
		printf("BCD.\n");
	else
		printf("Binary.\n");

	return true;
}


/////////////////////////////// PRIVATE //////////////////////////////////////

//============================= OPERATIONS ===================================

static bool TimerUpdateConfig(TimerClass* Timer, unsigned long timer)
{
	unsigned long config;
	port_t port;

	config = 0;
	port = TM_REG_TIMER0 + timer;

	if(timer > 2)
	{
		printf("\t TimerClass.c :: TimerUpdateConfig : Timer > 2. \n");
		return false;
	}

	// Creating Read-Back Control Word
	config |= TM_CTRL_RB_CMD | !TM_RB_COUNT_ | !TM_RB_STATUS_ | TM_RB_SEL(timer);

	// Writing Control Word into Control Register
	if(sys_outb(TM_REG_CTRL, config) != OK)
	{
		printf("\t TimerClass.c :: TimerUpdateConfig : sys_outb() failed. \n");
		return false;
	}
	// Retrieving info
	if(sys_inb(port, &config) != OK)
	{
		printf("\t TimerClass.c :: TimerUpdateConfig : sys_inb() failed. \n");
		return false;
	}

	TimerSetConfig(Timer, timer, (unsigned char) config & 0xFF);

	return true;
}
static void TimerSetConfig(TimerClass* Timer, unsigned long timer, unsigned char config)
{
	Timer->config[timer] = config;
	Timer->access[timer] = (config & TM_CTRL_LSB_MSB) >> 4;
	Timer->operatingMode[timer] = (config & TM_CTRL_OP_MODE) >> 1;
	Timer->BCD[timer] = config & TM_CTRL_BCD;
}
