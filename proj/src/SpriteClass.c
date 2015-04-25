#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "SpriteClass.h"
#include "CoordStruct.h"
#include "Sprites.h"


struct movement_struct
{
	float h;
	float b;
};

struct sprite_class
{
	Coord position; // current position
	Coord lastPosition;
	Coord dimensions; // dimensions
	limits_t limits;
	Coord speed; // current speed
	movement_t movement;
	short* map; // the pixmap
};

SpriteClass* newSprite()
{
	SpriteClass *sp;
	sp = (SpriteClass*) malloc(sizeof(SpriteClass));

	return sp;
}
void deleteSprite(SpriteClass* sp)
{
	free(sp);
}

bool SpriteInitializeXPM(SpriteClass* sp, char* pic[])
{
	sp->position.x = 0;
	sp->position.y = 0;
	sp->lastPosition.x = 0;
	sp->lastPosition.y = 0;

	sp->movement.h = 1;
	sp->movement.b = 0;

	sp->speed.x = 0;
	sp->speed.y = 0;

	if(!SpriteReadXPM(sp, pic))
		return false;

	return true;
}
bool SpriteInitializeBMP(SpriteClass* sp, char* filename, int left, int right, int up, int down)
{
	sp->limits.left = left;
	sp->limits.right = right;
	sp->limits.up = up;
	sp->limits.down = down;

	sp->position.x = 0;
	sp->position.y = 0;
	sp->lastPosition.x = 0;
	sp->lastPosition.y = 0;

	sp->speed.x = 0;
	sp->speed.y = 0;

	if(!SpriteReadBMP(sp, filename))
		return false;

	return true;
}
bool SpriteInitializeCircle(SpriteClass* sp, int radius, int color, int left, int up, int right, int down)
{

	sp->limits.left = left;
	sp->limits.right = right;
	sp->limits.up = up;
	sp->limits.down = down;

	sp->position.x = 0;
	sp->position.y = 0;
	sp->lastPosition.x = 0;
	sp->lastPosition.y = 0;

	sp->movement.h = 1;
	sp->movement.b = 0;

	sp->speed.x = 0;
	sp->speed.y = 0;

	sp->dimensions.x = 2*radius-1;
	sp->dimensions.y = 2*radius-1;


	int i, j;
	sp->map=(short*)malloc(sizeof (short)*sp->dimensions.x*sp->dimensions.y);
	for(i = 0; i < radius; i++)
	{
		for(j = 0; j < radius; j++)
		{
			if(i * i + j * j < radius * radius)
			{
				sp->map[radius + i + (radius + j)*sp->dimensions.x] = color;
				sp->map[radius - i + (radius + j)*sp->dimensions.x] = color;
				sp->map[radius + i + (radius - j)*sp->dimensions.x] = color;
				sp->map[radius - i + (radius - j)*sp->dimensions.x] = color;
			}
		}
	}
	sp->map[0] = 0;

	return true;
}
void SpriteShutdown(SpriteClass* sp)
{
	if(sp->map)
	{
		free(sp->map);
		sp->map = 0;
	}
}

Coord SpriteGetPosition(SpriteClass* sp)
{
	return sp->position;
}
movement_t SpriteGetMovement(SpriteClass* sp)
{
	return sp->movement;
}
float SpriteGetMovementH(SpriteClass* sp)
{
	return sp->movement.h;
}
float SpriteGetMovementB(SpriteClass* sp)
{
	return sp->movement.b;
}
int SpriteGetLimitU(SpriteClass* sp)
{
	return sp->limits.up;
}
int SpriteGetLimitD(SpriteClass* sp)
{
	return sp->limits.down;
}
int SpriteGetLimitL(SpriteClass* sp)
{
	return sp->limits.left;
}
int SpriteGetLimitR(SpriteClass* sp)
{
	return sp->limits.right;
}
Coord SpriteGetLastPosition(SpriteClass* sp)
{
	return sp->lastPosition;
}
Coord SpriteGetDimensions(SpriteClass* sp)
{
	return sp->dimensions;
}
limits_t SpriteGetLimits(SpriteClass* sp)
{
	return sp->limits;
}
Coord SpriteGetSpeed(SpriteClass* sp)
{
	return sp->speed;
}
short* SpriteGetMap(SpriteClass* sp)
{
	return sp->map;
}

void SpriteSetPosition(SpriteClass* sp, Coord position)
{
	sp->lastPosition = sp->position;
	sp->position = position;
}
void SpriteSetSpeed(SpriteClass* sp, Coord speed)
{
	sp->speed = speed;
}
void SpriteSetMovement(SpriteClass* sp, float h, float b)
{
	sp->movement.h = h;
	sp->movement.b = b;
}
void SpriteSetLimits(SpriteClass* sp, int up, int down, int left, int right)
{
	sp->limits.up = up;
	sp->limits.down = down;
	sp->limits.left = left;
	sp->limits.right = right;
}

void SpriteMove(SpriteClass* sp)
{
	sp->position.x += sp->movement.b;
	sp->position.y += sp->movement.h;

	if(sp->movement.b < 0 && sp->position.x < sp->limits.left)
	{
		sp->position.x = sp->limits.left;
		sp->movement.b = -sp->movement.b;
	}
	else if(sp->movement.b > 0 && sp->position.x + sp->dimensions.x+5 > sp->limits.right)
	{
		sp->position.x = sp->limits.right - sp->dimensions.x -5;
		sp->movement.b = -sp->movement.b;
	}


	if(sp->movement.h < 0 && sp->position.y < sp->limits.up)
	{
		sp->position.y = sp->limits.up;
		sp->movement.h = -sp->movement.h;
	}
	else if(sp->movement.h > 0 && sp->position.y + sp->dimensions.y+5 > sp->limits.down)
	{
		sp->position.y = sp->limits.down - sp->dimensions.y-5;
		sp->movement.h = -sp->movement.h;
	}
}

static bool SpriteReadXPM(SpriteClass* sp, char* map[])
{
	int width, height, nColors;
	int  col;
	int i, j;
	short* pix;
	short* pixtmp;
	char* tmp;
	char* line;
	char symbol;


	/* read width, height, colors */
	if (sscanf(map[0],"%d %d %d", &width, &height, &nColors) != 3)
	{
		printf("\t SpriteReadXPM: failed to read \n");
		return false;
	}

	sp->dimensions.x = width;
	sp->dimensions.y = height;

	char* symbols;
	symbols = (char*) malloc(sizeof(char) * nColors);
	short* colors;
	colors = (short*) malloc(sizeof(short) * nColors);

	for(i = 0; i < nColors; i++)
		symbols[i] = 0;

	for(i = 0; i < nColors; i++)
		colors[i] = 0;

	// read symbols <-> colors
	for (i = 0; i < nColors; i++)
	{
		if(sscanf(map[i+1], "%c %d", &symbol, &col) != 2)
		{
			printf("\t GraphicsReadXPM: incorrect symbol, color at line %d \n", i + 1);
			return 0;
		}
		if (col > 0xFFFF)
		{
			printf("\t GraphicsReadXPM: incorrect color at line %d \n", i + 1);
			return 0;
		}
		symbols[i] = symbol;
		colors[i] = col;
	}

	// allocate pixmap memory
	pix = pixtmp = malloc(width*height*sizeof(short));

	// parse each pixmap symbol line
	for (i = 0; i < height; i++)
	{
		line = map[nColors + 1 + i];

		for (j = 0; j < width; j++)
		{
			tmp = (char*) memchr((char*) symbols, line[j], 256);  // find color of each symbol WARNING HERE
			if (tmp == 0)
			{
				printf("\t GraphicsReadXPM: incorrect symbol at line %d, col %d \n", nColors+i+1, j);
				return 0;
			}
			*pixtmp++ = colors[(unsigned int) (tmp - symbols)];
		}
	}

	free(symbols);
	free(colors);

	sp->map = pix;

	return true;
}
static bool SpriteReadBMP(SpriteClass* sp, char* filename)
{
	int i, size;
	char info[54];
	FILE* file;

	errno = 0;
	file = fopen(filename, "rb");
	if(file == 0)
	{
		printf("\t SpriteClass.c :: SpriteReadBMP : %s \n", strerror(errno));
		return false;
	}

	// read the 54-byte header
	fread(info, sizeof(char), 54, file);

	// extract image height and width from header
	sp->dimensions.x = *(int*) &info[18];
	sp->dimensions.y = *(int*) &info[22];

	size = sp->dimensions.x * sp->dimensions.y;
	sp->map = (short*) malloc(size);

	// read the rest of the data at once
	fread(sp->map, sizeof(char), size, file);

	fclose(file);

	return true;
}
