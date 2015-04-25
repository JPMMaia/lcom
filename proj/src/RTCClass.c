#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include <stdio.h>

#include "RTCClass.h"

struct rtc_class
{
	unsigned long config[4];
	unsigned long date[10];
};

/////////////////////////////// PUBLIC ///////////////////////////////////////

//============================= LIFECYCLE ====================================

RTCClass* newRTC()
{
	RTCClass* RTC;
	RTC = (RTCClass*) malloc(sizeof(RTCClass));

	return RTC;
}
void deleteRTC(RTCClass* RTC)
{
	free(RTC);
}

void RTCInitialize(RTCClass* RTC)
{
	// Clearing interrupts
	RTCSetFlag(RTC, RTC_REG_B, (RTC_B_PIE | RTC_B_AIE | RTC_B_UIE | RTC_B_DM), false);

	// Clearing Register C flags
	RTCClearFlags(RTC, RTC_REG_C);

	unsigned int i;
	for(i = 0; i < 4; i++)
		RTC->config[i] = 0;

	for(i = 0; i < 10; i++)
		RTC->date[i] = 0;
}
void RTCShutdown(RTCClass* RTC)
{
	// Clearing Register C flags
	RTCClearFlags(RTC, RTC_REG_C);

	// Clearing interrupts
	RTCSetFlag(RTC, RTC_REG_B, (RTC_B_PIE | RTC_B_AIE | RTC_B_UIE), false);
}


//============================= OPERATIONS ===================================

void RTCShowConfig(RTCClass* RTC)
{
	RTCReadConfig(RTC);

	unsigned long* config;
	config = RTC->config;

	// Register A
	printf("\n\t Register A: \t\t 0x%X \n", config[0]);

	// UIP
	printf("\t Update-in-progress: ");
	if(config[0] & RTC_A_UIP)
		printf("\t Yes \n");
	else
		printf("\t No \n");

	//RS
	unsigned long rs;
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
}
void RTCShowDate(RTCClass* RTC)
{
	RTCReadDate(RTC);

	unsigned long* date;
	date = RTC->date;

	printf("\n\t Time: \t\t");
	RTCPrintTime(RTC, date[RTC_H], date[RTC_MIN], date[RTC_SEC]);

	printf("\t Date: \t\t");
	RTCPrintDate(RTC, date[RTC_DW], date[RTC_DM], date[RTC_MONTH], date[RTC_YEAR]);

	printf("\n\t Alarm Time: \t");
	RTCPrintTime(RTC, date[RTC_HA], date[RTC_MINA], date[RTC_SECA]);
	printf("\n");
}

void RTCPrintTime(RTCClass* RTC, unsigned long hour, unsigned long minute, unsigned long second)
{
	if(RTC->config[1] & RTC_B_DM)
		printf("\t %dh %dm %ds \n", hour, minute, second);
	else
		printf("\t %Xh %Xm %Xs \n", hour, minute, second);
}
void RTCPrintDate(RTCClass* RTC, unsigned long dayWeek, unsigned long dayMonth, unsigned long month, unsigned long year)
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
	if(RTC->config[1] & RTC_B_DM)
		printf(" %d,", dayMonth);
	else
		printf(" %X,", dayMonth);

	if(RTC->config[1] & RTC_B_DM)
		printf(" 20%d \n", year);
	else
		printf(" 20%X \n", year);
}

void RTCEnableUpdateEndedInterrupts(RTCClass* RTC)
{
	RTCSetFlag(RTC, RTC_REG_B, RTC_B_UIE, true);
}
bool RTCEnablePeriodicInterrupts(RTCClass* RTC, unsigned long mode)
{
	// Setting periodic rate
	if(!RTCSetPeriodicRate(RTC, 15)) //500ms
		return false;

	// Enable periodic interrupts
	RTCSetFlag(RTC, RTC_REG_B, RTC_B_PIE, true);

	return true;
}
void RTCEnableAlarmInterrupt(RTCClass* RTC, unsigned long delta)
{
	// Enable alarm interrupts
	RTCSetFlag(RTC, RTC_REG_B, RTC_B_AIE, true);

	// Read current date
	RTCReadDate(RTC);

	unsigned long* time;
	time = RTC->date;

	// Bit SET to 1
	RTCSetFlag(RTC, RTC_REG_B, RTC_B_SET, true);

	// Write Alarm sec
	time[RTC_SECA] = (delta + time[RTC_SEC]) % 0x60;
	RTCWriteReg(RTC_SECA, time[RTC_SECA]);

	// Write Alarm min
	time[RTC_MINA] = (((delta + time[RTC_SEC]) / 0x60) + time[RTC_MIN]) % 0x60;
	RTCWriteReg(RTC_MINA, time[RTC_MINA]);

	// Write Alarm hour
	time[RTC_HA] = (((((delta + time[RTC_SEC]) / 0x60) + time[RTC_MIN]) / 0x60) + time[RTC_H]) % 0x24;
	RTCWriteReg(RTC_HA, time[RTC_HA]);

	// Bit SET to 0
	RTCSetFlag(RTC, RTC_REG_B, RTC_B_SET, false);
}

rtc_int RTCHandleInt(RTCClass* RTC)
{
	unsigned long config;
	RTCReadReg(RTC_REG_C, &config);

	rtc_int interrupt;
	if(config & RTC_C_AF)
		interrupt = rtc_alarm;

	else if(config & RTC_C_PF)
		interrupt = rtc_periodic;

	else if(config & RTC_C_UF)
	{
		interrupt = rtc_update_ended;

		// Clearing IRQF bit as advised in the DS12887 data-sheet
		RTCSetFlag(RTC, RTC_REG_C, RTC_C_IRQF, false);
	}

	return interrupt;
}

/////////////////////////////// PRIVATE //////////////////////////////////////

//============================= OPERATIONS ===================================

static void RTCReadConfig(RTCClass* RTC)
{
	unsigned long i;
	for(i = 0; i < 4; i++)
		RTCReadReg(RTC_REG_A + i, &RTC->config[i]);
}
static void RTCReadDate(RTCClass* RTC)
{
	unsigned long i;
	for(i = 0; i < 10; i++)
		RTCReadReg(i, &RTC->date[i]);
}

static inline
void RTCReadReg(unsigned long addr, unsigned long* config)
{
	asm volatile ("outb %0, %1"
			:
			: "a" ((char) addr), "Nd" (RTC_ADDR_REG)
	);

	char byte;
	asm volatile ("inb %1, %0"
			: "=a" (byte)
			  : "Nd" (RTC_DATA_REG)
	);

	*config = byte;
}
static inline
void RTCWriteReg(unsigned long addr, unsigned long config)
{
	asm volatile ("outb %0, %1"
			:
			: "a" ((char) addr), "Nd" (RTC_ADDR_REG)
	);

	char byte;
	asm volatile ("outb %0, %1"
			:
			: "a" ((char) config), "Nd" (RTC_DATA_REG)
	);
}

static bool RTCSetPeriodicRate(RTCClass* RTC, unsigned long mode)
{
	if(mode > 0xF)
		return false;

	// Clear bit 0 to 3
	RTC->config[0] &= ~(RTC_A_RS3 | RTC_A_RS2 | RTC_A_RS1 | RTC_A_RS0);
	RTC->config[0] |= mode;

	RTCWriteReg(RTC_REG_A, RTC->config[0]);

	return true;
}

static void RTCSetFlag(RTCClass* RTC, unsigned long rtcReg, unsigned long flag, bool enable)
{
	unsigned long config;

	RTCReadReg(rtcReg, &config);

	if(enable)
		config |= flag;
	else
		config &= (~flag) & 0xFF;

	RTCWriteReg(rtcReg, config);

	RTC->config[rtcReg-10] = config;
}
static void RTCClearFlags(RTCClass* RTC, unsigned long rtcReg)
{
	// Clears Register C flags
	RTCWriteReg(rtcReg, 0);

	if(rtcReg < 10)
		RTC->date[rtcReg] = 0;
	else if(rtcReg < 14)
		RTC->config[rtcReg - 10] = 0;
}
