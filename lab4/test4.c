#include <minix/syslib.h>
#include <minix/drivers.h>
#include "test4.h"

int test_scan(void)
{
	int irq_set, ipc_status, r;
	message msg;

	irq_set = kb_subscribe_int();

	while(1)
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
					// Handle Interrupt
					r = kb_handle_int();
					if(r == -1)
						return -1;
				}

				break;
			default:
				break;
			}
			if(r == BC_ESC)
				break;
		}
	}

	if(kb_unsubscribe_int() != 0)
		return -1;

	return 0;
}

int test_leds(unsigned short n, unsigned short *toggle)
{
	int irq_set_timer, irq_set_kb, r, ipc_status, i, gCounter;
	message msg;

	irq_set_timer = timer_subscribe_int();
	//irq_set_kb = kb_subscribe_int();

	i = 0;
	while(1)
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
				if(msg.NOTIFY_ARG & irq_set_timer)
				{
					if(gCounter % 60 == 0)
					{
						kb_write_data(0xED, toggle[i]);
						i++;
					}

					// Handle Timer Interrupt
					gCounter = timer_int_handler();
				}

				break;
			default:
				break;
			}
			if(i == n)
				break;
		}
	}



	timer_unsubscribe_int();
	//kb_unsubscribe_int();

	return 0;
}
