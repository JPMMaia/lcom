#ifndef _GAME_LOGIC_CLASS_H_
#define _GAME_LOGIC_CLASS_H_

#include "GraphicsClass.h"
#include "ClockClass.h"

typedef struct game_logic_class GameLogicClass;

GameLogicClass* newGameLogic();
void deleteGameLogic(GameLogicClass* GameLogic);

void GameLogicInitialize(GameLogicClass* GameLogic, ClockClass* Clock, InputClass* Input, GraphicsClass* Graphics);
void GameLogicShutdown(GameLogicClass* GameLogic);

bool GameLogicRun(GameLogicClass* GameLogic);
bool GameLogicDrawWall(GameLogicClass* GameLogic);
bool GameLogicDrawBalls(GameLogicClass* GameLogic);
bool GameLogicProcessBall(GameLogicClass* GameLogic);
void GameLogicCalculateArea(GameLogicClass* GameLogic);
void GameLogicDrawAll(GameLogicClass* GameLogic);

bool GameLogicRunMenu(GameLogicClass* GameLogic);

bool GameLogicHasWon(GameLogicClass* GameLogic);

bool GameLogicIsBuildingWall(GameLogicClass* GameLogic);
void GameLogicEnableMenus(GameLogicClass* GameLogic, bool enable);

static bool GameLogicInitializeWalls(GameLogicClass* GameLogic, unsigned int thickness, unsigned int color, unsigned int x0, unsigned int y0, unsigned int xf, unsigned int yf);
static bool GameLogicInitializeMenus(GameLogicClass* GameLogic);
static void GameLogicShutdownWalls(GameLogicClass* GameLogic);
static void GameLogicShutdownMenus(GameLogicClass* GameLogic);

static bool GameLogicProcessMSEvents(GameLogicClass* GameLogic);
static void GameLogicProcessMSDirections(GameLogicClass* GameLogic);

static bool GameLogicProcessKBEvents(GameLogicClass* GameLogic);

static bool GameLogicProcessRTCEvents(GameLogicClass* GameLogic);

#endif
