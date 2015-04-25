#include <minix/drivers.h>
#include "RTC.h"

int rtc_test_conf(void)
{
	if(rtc_show_config() != 0)
		return -1;

	return 0;
}

int rtc_test_date(void)
{
	int irq_set, ipc_status, r, done = 0;
	unsigned long config;
	message msg;

	// Setting periodic rate
	if(rtc_set_periodic_rate(15) != 0) //500ms
		return -1;

	// Clearing Register C flags
	if(rtc_clear_flagsC() != 0)
		return -1;

	// Subscribing
	irq_set = rtc_subscribe_int();

	// Enable periodic interrupts
	if(rtc_set_flag(RTC_REG_B, RTC_B_PIE, 1) != 0)
		return -1;

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
					rtc_read_reg(RTC_REG_C, &config);

					if(config & RTC_C_PF)
					{
						// Handle Interrupt
						if(rtc_show_date() != 0)
							return -1;

						done = 1;
					}
				}
				break;
			default:
				break;
			}
		}
	}

	// Disable periodic interrupts
	if(rtc_set_flag(RTC_REG_B, RTC_B_PIE, 0) != 0)
		return -1;

	// Clearing Register C flags
	if(rtc_clear_flagsC() != 0)
		return -1;

	// Unsubscribing
	if(rtc_unsubscribe_int() != 0)
		return -1;

	return 0;
}

int rtc_test_int(unsigned long delta)
{
	int irq_set, ipc_status, r, i, done = 0;
	unsigned long config, time[10];
	message msg;

	// Clearing Register C flags
	if(rtc_clear_flagsC() != 0)
		return -1;

	// Subscribing
	irq_set = rtc_subscribe_int();

	// Enable alarm interrupts
	if(rtc_set_flag(RTC_REG_B, RTC_B_AIE, 1) != 0)
		return -1;

	for(i = 0; i < 10; i++)
		if(rtc_read_reg(i, &time[i]) != 0)
			return -1;

	// Write Alarm sec
	time[1] = (delta + time[0]) % 60;
	if(rtc_write_reg(1, time[1]) != 0)
		return -1;

	// Write Alarm min
	time[3] = (((delta + time[0]) / 60) + time[2]) % 60;
	if(rtc_write_reg(3, time[3]) != 0)
		return -1;

	// Write Alarm hour
	time[5] = (((delta + time[0]) / 3600) + time[4]) % 24;
	if(rtc_write_reg(5, time[5]) != 0)
		return -1;

	printf("\t test_int: Setting alarm within %d s \n", delta);

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
					rtc_read_reg(RTC_REG_C, &config);

					if(config & RTC_C_AF)
					{
						// Handle Interrupt
						printf("\t test_int: Alarm raised after %d s \n", delta);

						done = 1;
					}
				}
				break;
			default:
				break;
			}
		}
	}


	// Disable periodic interrupts
	if(rtc_set_flag(RTC_REG_B, RTC_B_AIE, 0) != 0)
		return -1;

	// Clearing Register C flags
	if(rtc_clear_flagsC() != 0)
		return -1;

	// Unsubscribing
	if(rtc_unsubscribe_int() != 0)
		return -1;



	return 0;
}
