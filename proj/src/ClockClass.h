#ifndef _CLOCK_CLASS_H_
#define _CLOCK_CLASS_H_

#include "boolType.h"
#include "ListClass.h"
#include "RTCClass.h"

typedef struct clock_class ClockClass;

ClockClass* newClock();
void deleteClock(ClockClass* Clock);

bool ClockInitialize(ClockClass* Clock);
void ClockShutdown(ClockClass* Clock);

void ClockHandleRTCInt(ClockClass* Clock);
void ClockSetAlarm(ClockClass* Clock, unsigned long delta);

ListClass* ClockGetRTCEvents(ClockClass* Clock);

static bool ClockInitializeTimer(ClockClass* Clock);
static void ClockShutdownTimer(ClockClass* Clock);

static bool ClockInitializeRTC(ClockClass* Clock);
static void ClockShutdownRTC(ClockClass* Clock);

#endif
