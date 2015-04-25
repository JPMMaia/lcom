#include <minix/syslib.h>
#include <minix/drivers.h>

#include "DispatcherClass.h"

struct dispatcher_class
{
	//	Keyboad
	int kbHookID;
	int kbIRQSet;

	// Mouse
	int msHookID;
	int msIRQSet;

	//	RTC
	int rtcHookID;
	int rtcIRQSet;

	//	Serial Port
	int spHookID;
	int spIRQSet;
	unsigned short spBaseAddr;

	//	Timer
	int tmHookID;
	int tmIRQSet;

	// Interfaces
	ClockClass* Clock;
	InputClass* Input;
	GameLogicClass* GameLogic;
	GraphicsClass* Graphics;
	ListClass* MSEvents;
};

/////////////////////////////// PUBLIC ///////////////////////////////////////

//============================= LIFECYCLE ====================================

DispatcherClass* newDispatcher()
{
	DispatcherClass* Dispatcher;
	Dispatcher = (DispatcherClass*) malloc(sizeof(DispatcherClass));

	Dispatcher->Clock = 0;
	Dispatcher->Input = 0;

	return Dispatcher;
}
void deleteDispatcher(DispatcherClass* Dispatcher)
{
	free(Dispatcher);
}

bool DispatcherInitialize(DispatcherClass* Dispatcher, ClockClass* Clock, InputClass* Input, GameLogicClass* GameLogic, GraphicsClass* Graphics)
{
	// Setting IRQ Sets
	Dispatcher->kbIRQSet = 0;
	Dispatcher->msIRQSet = 0;
	Dispatcher->rtcIRQSet = 0;
	Dispatcher->tmIRQSet = 0;
	Dispatcher->spIRQSet = 0;

	// Initialize Clock
	if(!DispatcherInitializeClock(Dispatcher, Clock))
		return false;

	// Initialize Input
	if(!DispatcherInitializeInput(Dispatcher, Input))
		return false;

	Dispatcher->Graphics = Graphics;

	Dispatcher->GameLogic = GameLogic;

	return true;
}
void DispatcherShutdown(DispatcherClass* Dispatcher)
{
	DispatcherShutdownInput(Dispatcher);
	DispatcherShutdownClock(Dispatcher);
}

bool DispatcherInitializeSerialPort(DispatcherClass* Dispatcher, SerialPortClass* SerialPort)
{
	Dispatcher->spBaseAddr = SerialPortGetBaseAddr(SerialPort);

	if(!DispatcherSubscribeInt(Dispatcher, SP, IRQ_REENABLE | IRQ_EXCLUSIVE))
		return false;
}
void DispatcherShutdownSerialPort(DispatcherClass* Dispatcher)
{
	DispatcherUnsubscribeInt(Dispatcher, SP);
}


//============================= OPERATIONS ===================================

void DispatcherRun(DispatcherClass* Dispatcher)
{
	int ipc_status, r;
	message msg;
	bool done;

	if((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
	{
		printf("\t DispatcherClass.c :: DispatcherRun : driver_receive() failed: %d \n", r);
		return;
	}

	if(is_ipc_notify(ipc_status))
	{
		switch(_ENDPOINT_P(msg.m_source))
		{
		case HARDWARE:
			if(msg.NOTIFY_ARG & Dispatcher->kbIRQSet)
			{
				DispatcherHandleInt(Dispatcher, KB);
			}
			if(msg.NOTIFY_ARG & Dispatcher->msIRQSet)
			{
				DispatcherHandleInt(Dispatcher, MS);
			}
			if(msg.NOTIFY_ARG & Dispatcher->tmIRQSet)
			{
				DispatcherHandleInt(Dispatcher, TM);
			}
			if(msg.NOTIFY_ARG & Dispatcher->rtcIRQSet)
			{
				DispatcherHandleInt(Dispatcher, RTC);
			}
			break;
		default:
			break;
		}
	}

	return;
}


/////////////////////////////// PRIVATE //////////////////////////////////////

//============================= LIFECYCLE ====================================

static bool DispatcherInitializeClock(DispatcherClass* Dispatcher, ClockClass* Clock)
{
	if(Clock == 0)
		return false;

	Dispatcher->Clock = Clock;

	// Subscribing Timer
	if(!DispatcherSubscribeInt(Dispatcher, TM, IRQ_REENABLE))
		return false;

	// Subscribing RTC
	if(!DispatcherSubscribeInt(Dispatcher, RTC, IRQ_REENABLE | IRQ_EXCLUSIVE))
		return false;

	return true;
}
static void DispatcherShutdownClock(DispatcherClass* Dispatcher)
{
	// Unsubscribing RTC
	DispatcherUnsubscribeInt(Dispatcher, RTC);

	// Unsubscribing Timer
	DispatcherUnsubscribeInt(Dispatcher, TM);

	Dispatcher->Clock = 0;
}

static bool DispatcherInitializeInput(DispatcherClass* Dispatcher, InputClass* Input)
{
	if(Input == 0)
		return false;

	Dispatcher->Input = Input;

	// Subscribing Mouse
	if(!DispatcherSubscribeInt(Dispatcher, MS, IRQ_REENABLE | IRQ_EXCLUSIVE))
		return false;

	Dispatcher->MSEvents = InputGetMSEvents(Dispatcher->Input);

	// Subscribing Keyboard
	if(!DispatcherSubscribeInt(Dispatcher, KB, IRQ_REENABLE | IRQ_EXCLUSIVE))
		return false;

	return true;
}
static void DispatcherShutdownInput(DispatcherClass* Dispatcher)
{
	// Unsubscribing Keyboard
	DispatcherUnsubscribeInt(Dispatcher, KB);

	// Unsubscribing Mouse
	DispatcherUnsubscribeInt(Dispatcher, MS);

	Dispatcher->Input = 0;
}

//============================= OPERATIONS ===================================

static bool DispatcherSubscribeInt(DispatcherClass* Dispatcher, devices_t device, unsigned long flags)
{
	int hookID;

	// Checking if the device is already subscribed
	if(DispatcherGetIRQSet(Dispatcher, device) != 0)
	{
		printf("\n\t DispatcherClass.c :: DispatcherSubscribeInt : Warning - device %d already subscribed \n", device);
		return false;
	}

	// Get IRQ Line
	hookID = DispatcherGetIRQLine(Dispatcher, device);

	// Subscribing
	if(sys_irqsetpolicy(hookID, flags, &hookID) != OK)
	{
		printf("\t DispatcherSubscribeInt: sys_irqsetpolicy() failed. \n");
		return false;
	}

	if(sys_irqenable(&hookID) != OK)
	{
		printf("\t DispatcherSubscribeInt: sys_irqenable() failed. \n");
		return false;
	}

	// Setting hookID
	DispatcherSetHookID(Dispatcher, device, hookID);

	// Setting IRQSet
	DispatcherSetIRQSet(Dispatcher, device, true);

	return true;
}
static bool DispatcherUnsubscribeInt(DispatcherClass* Dispatcher, devices_t device)
{
	int hookID;

	if(DispatcherGetIRQSet(Dispatcher, device) == 0)
		return false;

	hookID = DispatcherGetHookID(Dispatcher, device);

	if(sys_irqdisable(&hookID) != OK)
	{
		printf("\t DispatcherUnsubscribeInt: sys_disable() failed. \n");
		return false;
	}

	if(sys_irqrmpolicy(&hookID) != OK)
	{
		printf("\t DispatcherUnsubscribeInt: sys_irqrmpolicy() failed. \n");
		return false;
	}

	// Setting IRQSet
	DispatcherSetIRQSet(Dispatcher, device, false);

	return true;
}
static bool DispatcherHandleInt(DispatcherClass* Dispatcher, devices_t device)
{
	switch(device)
	{
	case KB:
		if(!InputHandleKeyboardInt(Dispatcher->Input))
			return false;
		break;
	case MS:
		if(!InputHandleMouseInt(Dispatcher->Input))
			return false;
		break;
	case RTC:
		ClockHandleRTCInt(Dispatcher->Clock);
		break;
	case SP:
		break;
	case TM:
		if(Dispatcher->Graphics)
		{
			GameLogicDrawAll(Dispatcher->GameLogic);
		}
		break;
	}

	return true;
}


//=============================   ACCESS   ===================================

static int DispatcherGetIRQLine(DispatcherClass* Dispatcher, devices_t device)
{
	int IRQ_line;

	switch(device)
	{
	case KB:	IRQ_line = IRQ_KB;		break;
	case MS:	IRQ_line = IRQ_MS;		break;
	case RTC:	IRQ_line = IRQ_RTC;		break;
	case SP:
		if(Dispatcher->spBaseAddr == SP_ADDR_COM1)
			Dispatcher->spIRQSet = IRQ_COM1;
		else
			Dispatcher->spIRQSet = IRQ_COM2;
		break;
	case TM:	IRQ_line = IRQ_TM;		break;
	}

	return IRQ_line;
}
static int DispatcherGetHookID(DispatcherClass* Dispatcher, devices_t device)
{
	int hookID;

	switch(device)
	{
	case KB:	hookID = Dispatcher->kbHookID;		break;
	case MS:	hookID = Dispatcher->msHookID;		break;
	case RTC:	hookID = Dispatcher->rtcHookID;		break;
	case SP:	hookID = Dispatcher->spHookID;		break;
	case TM:	hookID = Dispatcher->tmHookID;		break;
	}

	return hookID;
}
static int DispatcherGetIRQSet(DispatcherClass* Dispatcher, devices_t device)
{
	int IRQSet;

	switch(device)
	{
	case KB:	IRQSet = Dispatcher->kbIRQSet;		break;
	case MS:	IRQSet = Dispatcher->msIRQSet;		break;
	case RTC:	IRQSet = Dispatcher->rtcIRQSet;		break;
	case SP:	IRQSet = Dispatcher->spIRQSet;		break;
	case TM:	IRQSet = Dispatcher->tmIRQSet;		break;
	}

	return IRQSet;
}


static void DispatcherSetHookID(DispatcherClass* Dispatcher, devices_t device, int hookID)
{
	switch(device)
	{
	case KB:	Dispatcher->kbHookID = hookID;		break;
	case MS:	Dispatcher->msHookID = hookID;		break;
	case RTC:	Dispatcher->rtcHookID = hookID;		break;
	case SP:	Dispatcher->spHookID = hookID;		break;
	case TM:	Dispatcher->tmHookID = hookID;		break;
	}
}
static void DispatcherSetIRQSet(DispatcherClass* Dispatcher, devices_t device, bool enable)
{
	if(enable)
	{
		switch(device)
		{
		case KB:	Dispatcher->kbIRQSet = BIT(IRQ_KB);		break;
		case MS:	Dispatcher->msIRQSet = BIT(IRQ_MS);		break;
		case RTC:	Dispatcher->rtcIRQSet = BIT(IRQ_RTC);	break;
		case SP:
			if(Dispatcher->spBaseAddr == SP_ADDR_COM1)
				Dispatcher->spIRQSet = BIT(IRQ_COM1);
			else
				Dispatcher->spIRQSet = BIT(IRQ_COM2);
			break;
		case TM:	Dispatcher->tmIRQSet = BIT(IRQ_TM);		break;
		}
	}
	else
	{
		switch(device)
		{
		case KB:	Dispatcher->kbIRQSet = 0;		break;
		case MS:	Dispatcher->msIRQSet = 0;		break;
		case RTC:	Dispatcher->rtcIRQSet = 0;		break;
		case SP:	Dispatcher->spIRQSet = 0;		break;
		case TM:	Dispatcher->tmIRQSet = 0;		break;
		}
	}
}
