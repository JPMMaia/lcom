#ifndef _WALLS_CLASS_H_
#define _WALLS_CLASS_H_

#include "CoordStruct.h"
#include "boolType.h"

typedef struct walls_class WallsClass;

WallsClass* newWalls();
void deleteWalls(WallsClass* Walls);

bool WallsInitialize(WallsClass* Walls, char* buffer, unsigned int thickness, unsigned int color, unsigned int x0, unsigned int y0, unsigned int xf, unsigned int yf);
void WallsShutdown(WallsClass* Walls);

static void WallsDrawBackground(WallsClass* Walls);
static void WallsDrawHorizontalWall(WallsClass* Walls, unsigned int x0, unsigned int y0, unsigned int length);
static void WallsDrawVerticalWall(WallsClass* Walls, unsigned int x0, unsigned int y0, unsigned int length);

#endif
