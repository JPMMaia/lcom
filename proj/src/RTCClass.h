#ifndef _RTC_CLASS_H_
#define _RTC_CLASS_H_

#include "boolType.h"

#if(1)
#define BIT(n) (0x01<<(n))

#define RTC_ADDR_REG		0x70
#define RTC_DATA_REG		0x71

#define RTC_SEC				0
#define RTC_SECA			1
#define RTC_MIN				2
#define RTC_MINA			3
#define RTC_H				4
#define RTC_HA				5
#define RTC_DW				6
#define RTC_DM				7
#define RTC_MONTH			8
#define RTC_YEAR			9
#define RTC_REG_A			10
#define RTC_REG_B			11
#define RTC_REG_C			12
#define RTC_REG_D			13

#define RTC_A_UIP			BIT(7)
#define RTC_A_RS3			BIT(3)
#define RTC_A_RS2			BIT(2)
#define RTC_A_RS1			BIT(1)
#define RTC_A_RS0			BIT(0)

#define RTC_B_SET			BIT(7)
#define RTC_B_PIE			BIT(6)
#define RTC_B_AIE			BIT(5)
#define RTC_B_UIE			BIT(4)
#define RTC_B_SQWE			BIT(3)
#define RTC_B_DM			BIT(2)
#define RTC_B_2412			BIT(1)
#define RTC_B_DSE			BIT(0)

#define RTC_C_IRQF			BIT(7)
#define RTC_C_PF			BIT(6)
#define RTC_C_AF			BIT(5)
#define RTC_C_UF			BIT(4)

#define RTC_D_VRT			BIT(7)
#endif

typedef struct rtc_class RTCClass;
typedef enum
{
	rtc_alarm,
	rtc_periodic,
	rtc_update_ended
} rtc_int;

RTCClass* newRTC();
void deleteRTC(RTCClass* RTC);

void RTCInitialize(RTCClass* RTC);
void RTCShutdown(RTCClass* RTC);

void RTCShowConfig(RTCClass* RTC);
void RTCShowDate(RTCClass* RTC);

void RTCPrintTime(RTCClass* RTC, unsigned long hour, unsigned long minute, unsigned long second);
void RTCPrintDate(RTCClass* RTC, unsigned long dayWeek, unsigned long dayMonth, unsigned long month, unsigned long year);

void RTCEnableUpdateEndedInterrupts(RTCClass* RTC);
bool RTCEnablePeriodicInterrupts(RTCClass* RTC, unsigned long mode);
void RTCEnableAlarmInterrupt(RTCClass* RTC, unsigned long delta);

rtc_int RTCHandleInt(RTCClass* RTC);

static void RTCReadConfig(RTCClass* RTC);
static void RTCReadDate(RTCClass* RTC);

static inline
void RTCReadReg(unsigned long addr, unsigned long* config);
static inline
void RTCWriteReg(unsigned long addr, unsigned long config);

static bool RTCSetPeriodicRate(RTCClass* RTC, unsigned long mode);

static void RTCSetFlag(RTCClass* RTC, unsigned long rtcReg, unsigned long flag, bool enable);
static void RTCClearFlags(RTCClass* RTC, unsigned long rtcReg);

#endif
