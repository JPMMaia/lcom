#ifndef _SPRITE_CLASS_H_
#define _SPRITE_CLASS_H_

#include <stdio.h>
#include <stdlib.h>

#include "boolType.h"
#include "CoordStruct.h"

typedef struct sprite_class SpriteClass;

typedef struct limits_struct
{
	int left;
	int right;
	int up;
	int down;
}limits_t;

typedef struct movement_struct movement_t;

SpriteClass* newSprite();
void deleteSprite(SpriteClass* sp);

bool SpriteInitializeXPM(SpriteClass* sp, char* pic[]);
bool SpriteInitializeBMP(SpriteClass* sp, char* filename, int left, int right, int up, int down);
bool SpriteInitializeCircle(SpriteClass* sp, int radius, int color,int left, int up, int right, int down);
void SpriteShutdown(SpriteClass* sp);

Coord SpriteGetPosition(SpriteClass* sp);
movement_t SpriteGetMovement(SpriteClass* sp);
float SpriteGetMovementH(SpriteClass* sp);
float SpriteGetMovementB(SpriteClass* sp);
Coord SpriteGetLastPosition(SpriteClass* sp);
Coord SpriteGetDimensions(SpriteClass* sp);
limits_t SpriteGetLimits(SpriteClass* sp);
Coord SpriteGetSpeed(SpriteClass* sp);
short* SpriteGetMap(SpriteClass* sp);
int SpriteGetLimitU(SpriteClass* sp);
int SpriteGetLimitD(SpriteClass* sp);
int SpriteGetLimitL(SpriteClass* sp);
int SpriteGetLimitR(SpriteClass* sp);


void SpriteSetPosition(SpriteClass* sp, Coord position);
void SpriteSetLimits(SpriteClass* sp, int up, int down, int left, int right);
void SpriteSetSpeed(SpriteClass* sp, Coord speed);
void SpriteSetMovement(SpriteClass* sp, float h, float b);

static bool SpriteReadXPM(SpriteClass* sp, char* map[]);
static bool SpriteReadBMP(SpriteClass* sp, char* filename);

#endif
