#include "ClockClass.h"
#include "TimerClass.h"

#define RTC_DEBUG 1

struct clock_class
{
	// Timer
	TimerClass* Timer;
	unsigned long timer0Freq;

	// RTC
	RTCClass* RTC;
	ListClass* RTCEvents;
};

/////////////////////////////// PUBLIC ///////////////////////////////////////

//============================= LIFECYCLE ====================================

ClockClass* newClock()
{
	ClockClass* Clock;
	Clock = (ClockClass*) malloc(sizeof(ClockClass));

	Clock->Timer = 0;

	return Clock;
}
void deleteClock(ClockClass* Clock)
{
	free(Clock);
}

bool ClockInitialize(ClockClass* Clock)
{
	if(!ClockInitializeTimer(Clock))
		return false;

	if(!ClockInitializeRTC(Clock))
		return false;

	return true;
}
void ClockShutdown(ClockClass* Clock)
{
	ClockShutdownRTC(Clock);
	ClockShutdownTimer(Clock);
}


//============================= OPERATIONS ===================================

void ClockHandleRTCInt(ClockClass* Clock)
{
	rtc_int interrupt;
	interrupt = RTCHandleInt(Clock->RTC);

	ListPushBack(Clock->RTCEvents, (void*) &interrupt);
}
void ClockSetAlarm(ClockClass* Clock, unsigned long delta)
{
	RTCEnableAlarmInterrupt(Clock->RTC, delta);
}

//=============================   ACCESS   ===================================

ListClass* ClockGetRTCEvents(ClockClass* Clock)
{
	return Clock->RTCEvents;
}


/////////////////////////////// PRIVATE //////////////////////////////////////

//============================= LIFECYCLE ====================================

static bool ClockInitializeTimer(ClockClass* Clock)
{
	// Initializing Timer
	Clock->Timer = newTimer();
	if(!TimerInitialize(Clock->Timer))
		return false;

	// Setting Timer 0 frequency of 60 Hz
	Clock->timer0Freq = 60;
	if(!TimerSetSquare(Clock->Timer, 0, Clock->timer0Freq))
		return false;

	return true;
}
static void ClockShutdownTimer(ClockClass* Clock)
{
	if(Clock->Timer)
	{
		deleteTimer(Clock->Timer);
		Clock->Timer = 0;
	}
}

static bool ClockInitializeRTC(ClockClass* Clock)
{
	Clock->RTC = newRTC();
	RTCInitialize(Clock->RTC);

	Clock->RTCEvents = newList(sizeof(rtc_int));
	if(Clock->RTCEvents == 0)
		return false;

	RTCEnableUpdateEndedInterrupts(Clock->RTC);

	return true;
}
static void ClockShutdownRTC(ClockClass* Clock)
{
	if(Clock->RTCEvents)
	{
		deleteList(Clock->RTCEvents);
		Clock->RTCEvents = 0;
	}

	if(Clock->RTC)
	{
		RTCShutdown(Clock->RTC);
		deleteRTC(Clock->RTC);
		Clock->RTC = 0;
	}
}
