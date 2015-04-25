#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include <stdio.h>

#include "RTC.h"

#define DELAY_US    20000

static int rtcHookId;

int rtc_subscribe_int()
{
	int bitmask;

	rtcHookId = RTC_IRQL;
	bitmask = BIT(rtcHookId);

	if(sys_irqsetpolicy(RTC_IRQL, IRQ_REENABLE | IRQ_EXCLUSIVE, &rtcHookId) != OK)
	{
		printf("\t rtc_subscribe_int: sys_irqsetpolicy() failed. \n");
		return -1;
	}

	if(sys_irqenable(&rtcHookId) != OK)
	{
		printf("\t rtc_subscribe_int: sys_irqenable() failed. \n");
		return -1;
	}

	return bitmask;
}
int rtc_unsubscribe_int()
{
	if(sys_irqdisable(&rtcHookId) != OK)
	{
		printf("\t rtc_unsubscribe_int: sys_disable() failed. \n");
		return -1;
	}

	if(sys_irqrmpolicy(&rtcHookId) != OK)
	{
		printf("\t rtc_unsubscribe_int: sys_irqrmpolicy() failed. \n");
		return -1;
	}

	return 0;
}

int rtc_read_reg(unsigned long addr, unsigned long* config)
{
	if(sys_outb(RTC_ADDR_REG, addr) != OK)
	{
		printf("\t rtc_read_reg: sys_outb() failed. \n");
		return -1;
	}

	if(sys_inb(RTC_DATA_REG, config) != OK)
	{
		printf("\t rtc_read_reg: sys_inb() failed. \n");
		return -1;
	}

	return 0;
}
int rtc_write_reg(unsigned long addr, unsigned long config)
{
	if(sys_outb(RTC_ADDR_REG, addr) != OK)
	{
		printf("\t rtc_write_reg: sys_outb() failed. \n");
		return -1;
	}

	if(sys_outb(RTC_DATA_REG, config) != OK)
	{
		printf("\t rtc_write_reg: sys_inb() failed. \n");
		return -1;
	}

	return 0;
}

int rtc_show_config()
{
	unsigned long config[4], i, rs;

	for(i = 0; i < 4; i++)
		if(rtc_read_reg(RTC_REG_A + i, &config[i]) != 0)
			return -1;

	// Register A
	printf("\n\t Register A: \t\t 0x%X \n", config[0]);

	// UIP
	printf("\t Update-in-progress: ");
	if(config[0] & RTC_A_UIP)
		printf("\t Yes \n");
	else
		printf("\t No \n");

	//RS
	rs = 0
			| (config[0] & RTC_A_RS0)
			| (config[0] & RTC_A_RS1)
			| (config[0] & RTC_A_RS2)
			| (config[0] & RTC_A_RS3);

	if(rs == 0)
	{
		printf("\t SQW Output Freq: \t None \n");
		printf("\t Periodic Int Rate: \t None \n");
	}
	else
	{
		if(rs <= 2)
			rs += 7;	// 1 and 2 values are equal to 8 and 9 values

		rs = 1 << (16-rs);

		printf("\t SQW Output Freq: \t %d Hz \n", rs);
		printf("\t Periodic Int Rate: \t %d us \n", (1000000/rs));
	}

	// Register B
	printf("\n\t Register B: \t\t 0x%X \n", config[1]);

	printf("\t Updates: \t");
	if(config[1] & RTC_B_SET)
		printf("\t Enabled \n");
	else
		printf("\t Inhibited \n");

	printf("\t Periodic Int: \t");
	if(config[1] & RTC_B_PIE)
		printf("\t Enabled \n");
	else
		printf("\t Disabled \n");

	printf("\t Alarm Int: \t");
	if(config[1] & RTC_B_AIE)
		printf("\t Enabled \n");
	else
		printf("\t Disabled \n");

	printf("\t Update-ended Int: ");
	if(config[1] & RTC_B_UIE)
		printf("\t Enabled \n");
	else
		printf("\t Disabled \n");

	printf("\t Square-wave: \t");
	if(config[1] & RTC_B_SQWE)
		printf("\t Enabled \n");
	else
		printf("\t Disabled \n");

	printf("\t Mode: \t\t");
	if(config[1] & RTC_B_DM)
		printf("\t Binary \n");
	else
		printf("\t BCD \n");

	printf("\t Convention: \t");
	if(config[1] & RTC_B_2412)
		printf("\t 24h \n");
	else
		printf("\t 12h \n");

	printf("\t DST: \t\t");
	if(config[1] & RTC_B_DSE)
		printf("\t Enabled \n");
	else
		printf("\t Disabled \n");


	// Register C
	printf("\n\t Register C: \t\t 0x%X \n", config[2]);

	printf("\t IRQ line: \t");
	if(config[2] & RTC_C_IRQF)
		printf("\t Active \n");
	else
		printf("\t Inactive \n");

	printf("\t Pending Int: \t\t");
	if(config[2] & RTC_C_PF)
		printf(" PF");
	if(config[2] & RTC_C_AF)
		printf(" AF");
	if(config[2] & RTC_C_UF)
		printf(" UF");
	if(config[2] == BIT(7) || config[2] == 0)
		printf(" None");
	printf("\n");

	// Register D
	printf("\n\t Register D: \t\t 0x%X \n", config[3]);

	printf("\t VRT: \t\t");
	if(config[3] & RTC_D_VRT)
		printf("\t Valid \n\n");
	else
		printf("\t Invalid \n\n");

	return 0;
}

int rtc_show_date()
{
	unsigned long config[10], i, bin;

	tickdelay(micros_to_ticks(DELAY_US));

	rtc_read_reg(RTC_REG_B, &bin);
	bin &= RTC_B_DM;

	for(i = 0; i < 10; i++)
		if(rtc_read_reg(i, &config[i]) != 0)
			return -1;

	printf("\n\t Time: \t\t");
	rtc_print_time(config[RTC_H], config[RTC_MIN], config[RTC_SEC], bin);

	printf("\t Date: \t\t");
	rtc_print_date(config[RTC_DW], config[RTC_DM], config[RTC_MONTH], config[RTC_YEAR], bin);

	printf("\n\t Alarm Time: \t");
	rtc_print_time(config[RTC_HA], config[RTC_MINA], config[RTC_SECA], bin);
	printf("\n");

	return 0;
}

void rtc_print_time(unsigned long hour, unsigned long minute, unsigned long second, unsigned long bin)
{
	if(bin)
		printf("\t %dh %dm %ds \n", hour, minute, second);
	else
		printf("\t %Xh %Xm %Xs \n", hour, minute, second);
}
void rtc_print_date(unsigned long dayWeek, unsigned long dayMonth, unsigned long month, unsigned long year, unsigned long bin)
{
	switch(dayWeek)
	{
	case 1:
		printf("\t Sun,");
		break;
	case 2:
		printf("\t Mon,");
		break;
	case 3:
		printf("\t Tue,");
		break;
	case 4:
		printf("\t Wed,");
		break;
	case 5:
		printf("\t Thu,");
		break;
	case 6:
		printf("\t Fri,");
		break;
	case 7:
		printf("\t Sat,");
		break;
	}
	switch(month)
	{
	case 1:
		printf(" Jan");
		break;
	case 2:
		printf(" Feb");
		break;
	case 3:
		printf(" Mar");
		break;
	case 4:
		printf(" Apr");
		break;
	case 5:
		printf(" May");
		break;
	case 6:
		printf(" Jun");
		break;
	case 7:
		printf(" Jul");
		break;
	case 8:
		printf(" Aug");
		break;
	case 9:
		printf(" Sep");
		break;
	case 10:
		printf(" Oct");
		break;
	case 11:
		printf(" Nov");
		break;
	case 12:
		printf(" Dec");
		break;
	case 0x10:
		printf(" Oct");
		break;
	case 0x11:
		printf(" Nov");
		break;
	case 0x12:
		printf(" Dec");
		break;
	}
	if(bin)
		printf(" %d,", dayMonth);
	else
		printf(" %X,", dayMonth);

	if(bin)
		printf(" 20%d \n", year);
	else
		printf(" 20%X \n", year);
}

int rtc_set_flag(unsigned long addr, unsigned long flag, char enable)
{
	unsigned long config;

	if(rtc_read_reg(addr, &config) != 0)
		return -1;

	if(enable)
		config |= flag;
	else
		config &= ~flag;

	if(rtc_write_reg(addr, config) != 0)
		return -1;

	return 0;
}
int rtc_set_periodic_rate(unsigned long mode)
{
	unsigned long config;

	if(mode > 15)
		return -1;

	if(rtc_read_reg(RTC_REG_A, &config) != 0)
		return -1;

	config |= mode;

	if(rtc_write_reg(RTC_REG_A, config) != 0)
		return -1;

	return 0;
}
int rtc_clear_flagsC()
{
	unsigned long config;

	// Clears Register C flags
	if(rtc_write_reg(RTC_REG_C, 0) != 0)
		return -1;

	return 0;
}
