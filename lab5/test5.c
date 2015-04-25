//#include <minix/syslib.h>
#include <minix/drivers.h>
#include "test5.h"
#include "mouse.h"
#include "KBC.h"
#include "timer.h"

unsigned char packet[3];
unsigned short count;

int test_packet()
{
	int irq_set, ipc_status, r, done, state, prev;
	message msg;

	irq_set = mouse_subscribe_int();

	while(mouse_command(MOUSE_ENABLE) == KBC_ACKB_ERROR);

	count = 0;
	done = 0;
	state = 0;
	prev = -1;
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
					// Handle Interrupt
					if(mouse_handle_int(packet, &count) == 0 )
					{
						if(prev + 1 == packet[0] && state == 0)
							state = 1;

						else if(prev + 2 == packet[0] && state == 1)
							done = 1;

						else
							state = 0;

						prev = packet[0];
					}
				}
				break;
			default:
				break;
			}
		}
	}

	while(mouse_command(MOUSE_DISABLE) == KBC_ACKB_ERROR);

	mouse_unsubscribe_int();

	return 0;
}

int test_asynch(unsigned short duration)
{
	int irq_mouse_set, irq_timer_set, ipc_status, r, counter;
	message msg;

	irq_mouse_set = mouse_subscribe_int();

	while(mouse_command(MOUSE_ENABLE) == KBC_ACKB_ERROR);

	irq_timer_set = timer_subscribe_int();

	count = 0;
	do
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
				if(msg.NOTIFY_ARG & irq_mouse_set)
				{
					// Handle Mouse Interrupt
					mouse_handle_int(packet, &count);
				}
				if(msg.NOTIFY_ARG & irq_timer_set)
				{
					// Handle Timer Interrupt
					counter = timer_int_handler();
				}
				break;
			default:
				break;
			}
		}
	} while(counter / 60 < duration);

	while(mouse_command(MOUSE_DISABLE) == KBC_ACKB_ERROR);

	mouse_unsubscribe_int();
	timer_unsubscribe_int();

	return 0;
}
int test_config(void)
{
	int result;

	// Subscribing Mouse
	mouse_subscribe_int();

	result = mouse_show_config();

	// Unsubscribing Mouse
	if(mouse_unsubscribe_int() != 0)
		return -1;

	return result;
}
