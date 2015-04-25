#include "WallsClass.h"
#include "ListClass.h"
#include "SpriteClass.h"

char* wall[25] =
{
		"23 23 1",
		"+ 7",

		"++++++++++++++++++++++",
		"++++++++++++++++++++++",
		"++++++++++++++++++++++",
		"++++++++++++++++++++++",
		"++++++++++++++++++++++",
		"++++++++++++++++++++++",
		"++++++++++++++++++++++",
		"++++++++++++++++++++++",
		"++++++++++++++++++++++",
		"++++++++++++++++++++++",
		"++++++++++++++++++++++",
		"++++++++++++++++++++++",
		"++++++++++++++++++++++",
		"++++++++++++++++++++++",
		"++++++++++++++++++++++",
		"++++++++++++++++++++++"
};

struct walls_class
{
	ListClass* list;

	char* buffer;
	unsigned int x0;
	unsigned int y0;
	unsigned int screenWidth;
	unsigned int height;
	unsigned int thickness;
	char color;
};

WallsClass* newWalls()
{
	WallsClass* Walls;
	Walls = (WallsClass*) malloc(sizeof(WallsClass));

	Walls->list = 0;
	Walls->buffer = 0;

	return Walls;
}
void deleteWalls(WallsClass* Walls)
{
	free(Walls);
}

bool WallsInitialize(WallsClass* Walls, char* buffer, unsigned int thickness, unsigned int color, unsigned int x0, unsigned int y0, unsigned int xf, unsigned int yf)
{
	/*Walls->buffer = buffer;
	Walls->x0 = x0;
	Walls->y0 = y0;
	Walls->screenWidth;
	Walls->screenHeight;
	Walls->color = color;
	Walls->thickness = thickness;

	WallsDrawBackground(Walls);*/

	return true;
}
void WallsShutdown(WallsClass* Walls)
{
	Walls->buffer = 0;
}
