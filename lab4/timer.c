#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include "i8254.h"
#include "timer.h"

static unsigned int gCounter = 0;
static int gHook_id;

int timer_set_square(unsigned long timer, unsigned long freq)
{
	port_t port;
	unsigned long byte;
	char st;
	byte = 0;

	if(timer > 2)
	{
		printf("\t timer_set_square:: timer > 2. \n");
		return -1;
	}

	// Calculating port.
	port = 0x40 + timer;

	// Get previous Control Word
	timer_get_config(timer, &st);

	// Setting LSB (BCD/binary)
	byte |= st & 1;

	// Selecting Timer.
	if(timer == 1)
		byte |= TIMER_SEL1;
	else if(timer == 2)
		byte |= TIMER_SEL2;

	byte |= TIMER_LSB_MSB; 	// Read/Write LSB and MSB.
	byte |= TIMER_SQR_WAVE;	// Mode 3.

	// Writing Control Word into Control Word Register.
	if(sys_outb(TIMER_CTRL, byte) != OK)
	{
		printf("\t timer_set_square:: sys_outb() #1 failed. \n");
		return -1;
	}

	// Calculating divisor.
	byte = TIMER_FREQ / freq;

	// Writing LSB.
	if(sys_outb(port, (byte & 0xFF)) != OK)
	{
		printf("\t timer_set_square:: sys_outb() #2 failed. \n");
		return -1;
	}

	// Writing MSB.
	if(sys_outb(port, (byte >> 8)) != OK)
	{
		printf("\t timer_set_square:: sys_outb() #3 failed. \n");
		return -1;
	}

	return 0;
}

int timer_subscribe_int(void )
{
	int bitmask;

	// Setting hook id.
	gHook_id = 0;

	// Creating bitmask.
	bitmask = BIT(gHook_id);

	// Subscribing a notification on every interrupt.
	if(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &gHook_id) != OK)
	{
		printf("\t timer_subscribe_int:: sys_irqsetpolicy() failed. \n");
		return -1;
	}

	// Enabling interrupts on the IRQ0.
	if(sys_irqenable(&gHook_id) != OK)
	{
		printf("\t timer_subscribe_int:: sys_irqenable() failed. \n");
		return -1;
	}

	return bitmask;
}

int timer_unsubscribe_int()
{
	// Disabling interrupt on IRQ0
	if(sys_irqdisable(&gHook_id) != OK)
	{
		printf("\t timer_unsubscribe_int:: sys_irqdisable() failed. \n");
		return -1;
	}

	// Removing subscription on IRQ0
	if(sys_irqrmpolicy(&gHook_id) != OK)
	{
		printf("\t timer_unsubscribe_int:: sys_irqrmpolicy() failed. \n");
		return -1;
	}

	return 0;
}

int timer_int_handler()
{
	gCounter++;

	return gCounter;
}

int timer_get_config(unsigned long timer, unsigned char *st)
{
	unsigned long byte;
	port_t port;

	byte = 0;
	port = TIMER_0 + timer;

	if(timer > 2)
	{
		printf("\t timer_get_config:: timer > 2. \n");
		return -1;
	}

	// Creating Read-Back Control Word
	byte |= TIMER_RB_CMD | !TIMER_RB_COUNT_ | !TIMER_RB_STATUS_ | TIMER_RB_SEL(timer);

	// Writing Control Word into Control Register
	if(sys_outb(TIMER_CTRL, byte) != OK)
	{
		printf("\t timer_get_config:: sys_outb() failed. \n");
		return -1;
	}
	// Retrieving info
	if(sys_inb(port, &byte) != OK)
	{
		printf("\t timer_get_config:: sys_inb() failed. \n");
		return -1;
	}

	*st = byte & 0xFF;

	return 0;
}

int timer_show_config(unsigned long timer)
{
	unsigned char st;

	if(timer > 2)
	{
		printf("\t timer_show_config:: timer > 2. \n");
		return -1;
	}

	if(timer_get_config(timer, &st) != 0)
	{
		printf("\t timer_show_config:: timer_get_config() failed. \n");
		return -1;
	}

	printf("st: %X", st);

	printf("\n\t Timer %u:\n\n", timer);

	printf("\t\t Type of Access: ");
	if(TIMER_LSB & st && TIMER_MSB & st)
		printf("Read/Write least significant byte first, then most significant byte.\n");
	else if(TIMER_LSB & st)
		printf("Read/Write least significant byte only.\n");
	else if(TIMER_MSB & st)
		printf("Read/Write most significant byte only.\n");

	printf("\t\t Programmed Mode: ");
	printf("%d.\n", ((st >> 1) & 7));

	printf("\t\t Counting Mode: ");
	if(TIMER_BCD & st)
		printf("BCD.\n");
	else
		printf("Binary.\n");

	return 0;
}

int timer_test_square(unsigned long freq)
{
	int result;

	result = timer_set_square(0, freq);
	if(result != 0)
	{
		printf("\t timer_test_square:: timer_set_square() failed. \n");
		return -1;
	}

	return result;
}

int timer_test_int(unsigned long time)
{
	int irq_set, ipc_status, r;
	unsigned long remaining;
	message msg;

	// Subscribing IRQ0
	irq_set = timer_subscribe_int();
	if(irq_set == -1)
	{
		printf("\t timer_test_int:: timer_subscribe_int() failed. \n");
		return -1;
	}

	while(gCounter < time * 60)
	{
		if((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
		{
			printf("\t driver_receive() failed: %d \n", r);
			continue;
		}

		if(is_ipc_notify(ipc_status))
		{
			switch(_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:
				if(msg.NOTIFY_ARG & irq_set)
				{
					// Handle Interrupt
					timer_int_handler();

					// Print a message every second
					if(gCounter % 60 == 0)
						printf("Counter: %u \n", gCounter);
				}
				break;
			default:
				break;
			}
		}
	}

	// Removing subscription
	if(timer_unsubscribe_int() != 0)
	{
		printf("\t timer_test_int:: timer_unsubscrive_int() failed. \n");
		return -1;
	}

	return 0;
}

int timer_test_config(unsigned long timer)
{
	if(timer_show_config(timer) != 0)
	{
		printf("\t timer_test_config:: timer_show_config() failed. \n");
		return -1;
	}

	return 0;
}
