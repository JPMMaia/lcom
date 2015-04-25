#ifndef _SYSTEM_CLASS_H_
#define _SYSTEM_CLASS_H_

#include "ClockClass.h"
#include "InputClass.h"
#include "GraphicsClass.h"
#include "GameLogicClass.h"

typedef struct system_class SystemClass;

SystemClass* newSystem();
void deleteSystem(SystemClass* System);

bool SystemInitialize(SystemClass* System, bool GraphicsModeOn);
void SystemShutdown(SystemClass* System);
void SystemRun(SystemClass* System);

static bool SystemInitializeInput(SystemClass* System, unsigned int x0, unsigned int y0, unsigned int screenWidth, unsigned int screenHeight);
static void SystemShutdownInput(SystemClass* System);

static bool SystemInitializeDispatcher(SystemClass* System, ClockClass* Clock, InputClass* Input, GameLogicClass* GameLogic, GraphicsClass* Graphics);
static void SystemShutdownDispatcher(SystemClass* System);

static bool SystemInitializeClock(SystemClass* System);
static void SystemShutdownClock(SystemClass* System);

static bool SystemInitializeGraphics(SystemClass* System, unsigned int mode, InputClass* Input);
static void SystemShutdownGraphics(SystemClass* System);

static void SystemInitializeGameLogic(SystemClass* System, ClockClass* Clock, InputClass* Input, GraphicsClass* Graphics);
static void SystemShutdownGameLogic(SystemClass* System);

#endif
