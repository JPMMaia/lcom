#include "SystemClass.h"
#include "GraphicsClass.h"
#include "SerialPortClass.h"
#include "MenuClass.h"
#include "DispatcherClass.h"
#include "GameLogicClass.h"

#include <minix/syslib.h>
#include <minix/drivers.h>

struct system_class
{
	InputClass* Input;
	GraphicsClass* Graphics;
	ClockClass* Clock;
	SerialPortClass* SerialPort;
	DispatcherClass* Dispatcher;
	GameLogicClass* GameLogic;

	int screenWidth;
	int screenHeight;
};


/////////////////////////////// PUBLIC ///////////////////////////////////////

//============================= LIFECYCLE ====================================

SystemClass* newSystem()
{
	SystemClass* System;
	System = (SystemClass*) malloc(sizeof(SystemClass));

	System->Input = 0;
	System->Graphics = 0;
	System->Clock = 0;
	System->SerialPort = 0;
	System->Dispatcher = 0;

	return System;
}
void deleteSystem(SystemClass* System)
{
	free(System);
}

bool SystemInitialize(SystemClass* System, bool GraphicsModeOn)
{
	unsigned int mode;

	mode = 0x117;
	System->screenWidth = 1024;
	System->screenHeight = 768;

	if(!SystemInitializeInput(System, 30, 30, System->screenWidth - 61, System->screenHeight - 61))
		return false;

	if(!SystemInitializeClock(System))
		return false;

	if(GraphicsModeOn)
		if(!SystemInitializeGraphics(System, mode, System->Input))
			return false;

	SystemInitializeGameLogic(System, System->Clock, System->Input, System->Graphics);

	if(!SystemInitializeDispatcher(System, System->Clock, System->Input, System->GameLogic, System->Graphics))
		return false;

	return true;
}
void SystemShutdown(SystemClass* System)
{
	SystemShutdownGameLogic(System);
	SystemShutdownDispatcher(System);
	SystemShutdownGraphics(System);
	SystemShutdownClock(System);
	SystemShutdownInput(System);
}


//============================= OPERATIONS ===================================

void SystemRun(SystemClass* System)
{
	while(true)
	{
		DispatcherRun(System->Dispatcher);

		if(!GameLogicRun(System->GameLogic))
			return;
	}
}


/////////////////////////////// PRIVATE //////////////////////////////////////

//============================= LIFECYCLE ====================================

static bool SystemInitializeInput(SystemClass* System, unsigned int x0, unsigned int y0, unsigned int screenWidth, unsigned int screenHeight)
{
	System->Input = newInput();
	if(!InputInitialize(System->Input, x0, y0, screenWidth, screenHeight))
		return false;

	return true;
}
static void SystemShutdownInput(SystemClass* System)
{
	if(System->Input)
	{
		InputShutdown(System->Input);
		deleteInput(System->Input);
		System->Input = 0;
	}
}

static bool SystemInitializeDispatcher(SystemClass* System, ClockClass* Clock, InputClass* Input, GameLogicClass* GameLogic, GraphicsClass* Graphics)
{
	System->Dispatcher = newDispatcher();
	if(DispatcherInitialize(System->Dispatcher, Clock, Input, GameLogic, Graphics) != true)
		return false;

	return true;
}
static void SystemShutdownDispatcher(SystemClass* System)
{
	DispatcherShutdown(System->Dispatcher);
	System->Dispatcher = 0;
}

static bool SystemInitializeClock(SystemClass* System)
{
	System->Clock = newClock();
	if(!ClockInitialize(System->Clock))
		return false;

	return true;
}
static void SystemShutdownClock(SystemClass* System)
{
	if(System->Clock)
	{
		ClockShutdown(System->Clock);
		deleteClock(System->Clock);
		System->Clock = 0;
	}
}

static bool SystemInitializeGraphics(SystemClass* System, unsigned int mode, InputClass* Input)
{
	System->Graphics = newGraphics();
	if(!GraphicsInitialize(System->Graphics, mode, Input))
		return false;

	return true;
}
static void SystemShutdownGraphics(SystemClass* System)
{
	if(System->Graphics)
	{
		GraphicsShutdown(System->Graphics);
		free(System->Graphics);
		System->Graphics = 0;
	}
}

static void SystemInitializeGameLogic(SystemClass* System, ClockClass* Clock, InputClass* Input, GraphicsClass* Graphics)
{
	System->GameLogic = newGameLogic();

	GameLogicInitialize(System->GameLogic, Clock, Input, Graphics);
}
static void SystemShutdownGameLogic(SystemClass* System)
{
	if(System->GameLogic)
	{
		GameLogicShutdown(System->GameLogic);
		deleteGameLogic(System->GameLogic);
		System->GameLogic = 0;
	}
}
