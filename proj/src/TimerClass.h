#ifndef _TIMER_CLASS_H_
#define _TIMER_CLASS_H_

#include "boolType.h"

#if (1)
#define BIT(n) 				(0x01<<(n))

#define TM_FREQ  1193182

#define TM_REG_TIMER0			0x40
#define TM_REG_TIMER1			0x41
#define TM_REG_TIMER2			0x42
#define TM_REG_CTRL				0x43
#define TM_REG_SPEAKER_CTRL		0x61

#define TM_CTRL_SEL0			0x00
#define TM_CTRL_SEL1			BIT(6)
#define TM_CTRL_SEL2			BIT(7)
#define TM_CTRL_RB_CMD			(BIT(7) | BIT(6))
#define TM_CTRL_LSB				BIT(4)
#define TM_CTRL_MSB				BIT(5)
#define TM_CTRL_LSB_MSB			(BIT(5) | BIT(4))
#define TM_CTRL_SQR_WAVE		(BIT(2) | BIT(1))
#define TM_CTRL_RATE_GEN		BIT(2)
#define TM_CTRL_OP_MODE			BIT(3) | BIT(2) | BIT(1)
#define	TM_CTRL_BCD				BIT(0)

#define TM_RB_COUNT_         BIT(5)
#define TM_RB_STATUS_        BIT(4)
#define TM_RB_SEL(n)         BIT((n)+1)
#endif

typedef struct timer_class TimerClass;

TimerClass* newTimer();
void deleteTimer(TimerClass* Timer);

bool TimerInitialize(TimerClass* Timer);

bool TimerSetSquare(TimerClass* Timer, unsigned long timer, unsigned long freq);
void TimerHandleInt(TimerClass* Timer);
bool TimerShowConfig(TimerClass* Timer, unsigned long timer);

static bool TimerUpdateConfig(TimerClass* Timer, unsigned long timer);
static void TimerSetConfig(TimerClass* Timer, unsigned long timer, unsigned char config);


#endif
