#include "CoordClass.h"

struct coord_class
{
	int x;
	int y;
};

CoordClass* newCoord(int x, int y)
{
	CoordClass* Coord;
	Coord = (CoordClass*) malloc(sizeof(CoordClass));

	Coord->x = x;
	Coord->y = y;

	return Coord;
}
void deleteCoord(CoordClass* Coord)
{
	free(Coord);
}

void CoordAdd(CoordClass* C1, CoordClass C2)
{
	C1->x += C2.x;
	C1->y += C2.y;
}
void CoordSub(CoordClass* C1, CoordClass C2)
{
	C1->x -= C2.x;
	C1->y -= C2.y;
}
