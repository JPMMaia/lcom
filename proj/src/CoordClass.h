#ifndef _COORD_CLASS_H_
#define _COORD_CLASS_H_

typedef struct coord_class CoordClass;

CoordClass* newCoord(int x, int y);
void deleteCoord(CoordClass* Coord);

void CoordAdd(CoordClass* C1, CoordClass C2);
void CoordSub(CoordClass* C1, CoordClass c2);

#endif
