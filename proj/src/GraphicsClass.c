#include <sys/video.h>
#include <minix/drivers.h>
#include <minix/syslib.h>
#include <machine/int86.h>

#include "GraphicsClass.h"
#include "lmlib.h"
#include "numbers.h"

char* triangle[18];
//char* ball[18];

typedef struct
{
	Coord lastP;
	Coord currentP;
	short* map;
	int width, height;
} mouseCursor;


struct graphics_class
{
	unsigned short mode;
	short* videoMem;
	short* walls;
	short* backBuffer;

	unsigned int wallsSize;
	unsigned int wallsX0;
	unsigned int wallsY0;
	unsigned int wallsXF;
	unsigned int wallsYF;

	mouseCursor mouse;
	SpriteClass* triangle;
	SpriteClass* ball1;
	SpriteClass* ball2;

	short* numbers[10];
	unsigned int currentNumber;
	Coord numbersPosition;

	unsigned long XResolution;
	unsigned long YResolution;
	unsigned long bitsPerPixel;
	unsigned long size;

	// Interfaces
	InputClass* Input;
};

/////////////////////////////// PUBLIC ///////////////////////////////////////

//============================= LIFECYCLE ====================================

GraphicsClass* newGraphics()
{
	GraphicsClass* Graphics;
	Graphics = (GraphicsClass*) malloc(sizeof(GraphicsClass));

	Graphics->videoMem = 0;
	Graphics->backBuffer = 0;
	Graphics->walls = 0;

	return Graphics;
}
void deleteGraphics(GraphicsClass* Graphics)
{
	free(Graphics);
}

bool GraphicsInitialize(GraphicsClass* Graphics, unsigned short mode, InputClass* Input)
{
	unsigned long i;

	if(!GraphicsInitializeVBE(Graphics, mode))
		return false;

	/*Graphics->XResolution = 1024;
	Graphics->YResolution = 768;*/
	Graphics->backBuffer = (short*) malloc(Graphics->XResolution*Graphics->YResolution*sizeof(short));

	Graphics->Input = Input;

	GraphicsInitializeMouseCursor(Graphics, 63488);

	Graphics->walls = (short*) malloc(Graphics->XResolution*Graphics->YResolution*sizeof(short));
	GraphicsDrawBackground(Graphics);



	if(!GraphicsInitializeSprites(Graphics))
		return false;

	if(!GraphicsInitializeNumbers(Graphics))
		return false;

	GraphicsInitializeBall(Graphics);

	return true;
}
void GraphicsShutdown(GraphicsClass* Graphics)
{
	GraphicsShutdownNumbers(Graphics);
	GraphicsShutdownSprites(Graphics);
	GraphicsShutdownMouseCursor(Graphics);
	Graphics->Input = 0;
	free(Graphics->backBuffer);
	GraphicsShutdownVBE(Graphics);
}

bool GraphicsIsPixelSet(GraphicsClass* Graphics, unsigned int x, unsigned int y)
{
	return Graphics->walls[x + y * Graphics->XResolution] != 0;
}

//============================= OPERATIONS ===================================

void GraphicsShowConfig(GraphicsClass* Graphics)
{
	printf("\n\t Mode:\t\t\t %X \n", Graphics->mode);
	printf("\t Resolution:\t\t %dx%d \n", Graphics->XResolution, Graphics->YResolution);
	printf("\t Bits per pixel:\t %d \n\n", Graphics->bitsPerPixel);
}
void GraphicsDraw(GraphicsClass* Graphics)
{
	memcpy(Graphics->backBuffer, Graphics->walls, Graphics->XResolution*Graphics->YResolution*sizeof(short));
	GraphicsDrawMouseCursor(Graphics);
	if(InputIsMSLocked(Graphics->Input))
		GraphicsDrawTriangle(Graphics);

	GraphicsDrawBall(Graphics);

	GraphicsDrawNumber(Graphics, Graphics->currentNumber / 10, Graphics->numbersPosition.x, Graphics->numbersPosition.y);
	GraphicsDrawNumber(Graphics, Graphics->currentNumber % 10, Graphics->numbersPosition.x + 18, Graphics->numbersPosition.y);
}
bool GraphicsDrawMouseCursor(GraphicsClass* Graphics)
{
	Coord position;
	mouseCursor* mouse;
	int radius;

	position.x = Graphics->mouse.currentP.x + radius;
	position.y = Graphics->mouse.currentP.y + radius;

	mouse = &Graphics->mouse;
	radius = 15;

	mouse->lastP = Graphics->mouse.currentP;
	mouse->currentP = InputGetMouseCursor(Graphics->Input);

	int i, j;
	for(i = 0; i < mouse->height; i++)
		for(j = 0; j < mouse->width; j++)
			if(mouse->map[j + i*mouse->width] != 0)
				Graphics->backBuffer[(mouse->currentP.x + j) + (mouse->currentP.y + i) * Graphics->XResolution] = mouse->map[j + i*mouse->width];

	return true;
}
void GraphicsFrame(GraphicsClass* Graphics)
{
	memcpy(Graphics->videoMem, Graphics->backBuffer, Graphics->XResolution*Graphics->YResolution*sizeof(short));
}
bool GraphicsDrawBall(GraphicsClass* Graphics)
{
	SpriteMove(Graphics->ball1);
	SpriteMove(Graphics->ball2);
	GraphicsDrawSprite(Graphics, Graphics->ball1);
	GraphicsDrawSprite(Graphics, Graphics->ball2);

	return true;
}
bool GraphicsClearTraceMouseCursor(GraphicsClass* Graphics, int radius)
{
	mouseCursor* mouse;
	mouse = &Graphics->mouse;

	int i;
	for (i = 0; i < mouse->height; i++)
		memset(Graphics->backBuffer + mouse->lastP.x + mouse->lastP.y * Graphics->XResolution, 0, mouse->width);

	return true;
}
bool GraphicsDrawCircle(GraphicsClass* Graphics, int radius,  int color)
{
	mouseCursor* mouse;
	mouse = &Graphics->mouse;

	int i, j;
	for(i = 0; i < radius; i++)
	{
		for(j = 0; j < radius; j++)
		{
			if(i * i + j * j < radius * radius)
			{
				mouse->map[radius + i + (radius + j)*mouse->width] = color;
				mouse->map[radius - i + (radius + j)*mouse->width] = color;
				mouse->map[radius + i + (radius - j)*mouse->width] = color;
				mouse->map[radius - i + (radius - j)*mouse->width] = color;
			}
		}
	}

	mouse->map[0] = 0;
	return true;
}
void GraphicsDrawTriangle(GraphicsClass* Graphics)
{
	directions_t direction;
	direction = InputMSCheckDirection(Graphics->Input);

	Coord dimensions;
	dimensions = SpriteGetDimensions(Graphics->triangle);

	Coord position1;
	Coord position2;
	switch(direction)
	{
	case east:
		position1.x = Graphics->mouse.currentP.x + Graphics->mouse.width;
		position1.y = Graphics->mouse.currentP.y;
		position2.x= Graphics->mouse.currentP.x - dimensions.y + 1;
		position2.y = position1.y;
		break;
	case north:
		position1.x = Graphics->mouse.currentP.x;
		position1.y = Graphics->mouse.currentP.y - dimensions.y + 1;
		position2.x = Graphics->mouse.currentP.x;
		position2.y = Graphics->mouse.currentP.y + Graphics->mouse.height;
		break;
	case west:
		position1.x = Graphics->mouse.currentP.x - dimensions.y + 1;
		position1.y = Graphics->mouse.currentP.y;
		position2.x = Graphics->mouse.currentP.x + Graphics->mouse.width;
		position2.y = Graphics->mouse.currentP.y;
		break;
	case south:
		position1.x = Graphics->mouse.currentP.x;
		position1.y = Graphics->mouse.currentP.y + Graphics->mouse.height;
		position2.x = Graphics->mouse.currentP.x;
		position2.y = Graphics->mouse.currentP.y - dimensions.y + 1;
		break;
	case none_dir:
		return;
	}

	SpriteSetPosition(Graphics->triangle, position1);
	GraphicsDrawSpriteDir(Graphics, Graphics->triangle, direction);
	SpriteSetPosition(Graphics->triangle, position2);
	GraphicsDrawSpriteDir(Graphics, Graphics->triangle, -direction);
}
bool GraphicsDrawNumber(GraphicsClass* Graphics, unsigned int number, unsigned int x, unsigned int y)
{
	if(number > 9)
		return false;

	unsigned int width, height;
	width = 16;
	height = 25;

	unsigned int i, j;
	for(i = 0; i < height; i++)
		for(j = 0; j < width; j++)
			if(Graphics->numbers[number][j + i*width] != 0)
				Graphics->backBuffer[(x + j) + (y + i) * Graphics->XResolution] = Graphics->numbers[number][j + i*width];

	return true;
}


//=============================   ACCESS   ===================================

unsigned int* GraphicsGetCurrentNumber(GraphicsClass* Graphics)
{
	return &Graphics->currentNumber;
}
unsigned long GraphicsGetXResolution(GraphicsClass* Graphics)
{
	return Graphics->XResolution;
}
unsigned long GraphicsGetYResolution(GraphicsClass* Graphics)
{
	return Graphics->YResolution;
}

/////////////////////////////// PRIVATE //////////////////////////////////////

//============================= LIFECYCLE ====================================

static bool GraphicsInitializeVBE(GraphicsClass* Graphics, unsigned short mode)
{
	struct mem_range range;
	struct reg86u reg86;
	vbe_mode_info_t info;
	int r;

	// Set VBE Mode
	if(!GraphicsSetVBEMode(Graphics, mode))
		return false;

	// Get VBE Mode Info
	if(!GraphicsGetVBEInfo(Graphics, &info))
	{
		GraphicsShutdownVBE(Graphics);
		return false;
	}

	// Allow memory map
	range.mr_base = info.PhysBasePtr;
	range.mr_limit = range.mr_base + info.XResolution * info.YResolution * info.BitsPerPixel;
	if((r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &range)) != OK)
	{
		printf("\t GraphicsClass.c :: GraphicsInitializeVBE : sys_privctl (ADD_MEM) failed: %d\n", r);
		return false;
	}

	// Extract information
	Graphics->size = info.XResolution*info.YResolution*info.BitsPerPixel;
	Graphics->videoMem = vm_map_phys(SELF, (void*) range.mr_base, Graphics->size);
	Graphics->XResolution = info.XResolution;
	Graphics->YResolution = info.YResolution;
	Graphics->bitsPerPixel = info.BitsPerPixel;

	return true;
}
static bool GraphicsShutdownVBE(GraphicsClass* Graphics)
{
	struct reg86u reg86;

	// BIOS Video Services
	reg86.u.b.intno = GR_INT_NUMBER;

	// Set Video Mode function
	reg86.u.b.ah = 0x00;

	// 80x25 text mode
	reg86.u.b.al = 0x03;

	if(sys_int86(&reg86) != OK)
	{
		GraphicsError("\t GraphicsClass.c :: GraphicsShutdownVBE : sys_int86() failed \n");
		return false;
	}

	//GraphicsSetVBEMode(Graphics, GR_MODE_TEXT_MODE);

	return true;
}

static bool GraphicsInitializeSprites(GraphicsClass* Graphics)
{
	Graphics->triangle = newSprite();
	if(!SpriteInitializeXPM(Graphics->triangle, triangle))
		return false;

	Graphics->ball1 = newSprite();
	if(!SpriteInitializeCircle(Graphics->ball1,15,7,30, 30, Graphics->XResolution - 30, Graphics->YResolution - 30 ))
		return false;

	Graphics->ball2 = newSprite();
	if(!SpriteInitializeCircle(Graphics->ball2,15,7,30, 30, Graphics->XResolution - 30, Graphics->YResolution - 30 ))
		return false;


	return true;
}
static void GraphicsShutdownSprites(GraphicsClass* Graphics)
{
	if(Graphics->triangle)
	{
		SpriteShutdown(Graphics->triangle);
		deleteSprite(Graphics->triangle);
		Graphics->triangle = 0;
	}

	if(Graphics->ball1)
	{
		SpriteShutdown(Graphics->ball1);
		deleteSprite(Graphics->ball1);
		Graphics->ball1 = 0;
	}
	if(Graphics->ball2)
	{
		SpriteShutdown(Graphics->ball2);
		deleteSprite(Graphics->ball2);
		Graphics->ball2 = 0;
	}
}

static bool GraphicsInitializeMouseCursor(GraphicsClass* Graphics, int color)
{
	int radius = 15;

	Graphics->mouse.currentP = InputGetMouseCursor(Graphics->Input);
	Graphics->mouse.lastP = Graphics->mouse.currentP;
	Graphics->mouse.width = 2*radius;
	Graphics->mouse.height = 2*radius;
	Graphics->mouse.map = malloc(Graphics->mouse.width*Graphics->mouse.height*sizeof(short));


	GraphicsDrawCircle(Graphics, radius, color);

	return true;
}
static void GraphicsShutdownMouseCursor(GraphicsClass* Graphics)
{
	if(Graphics->mouse.map)
	{
		free(Graphics->mouse.map);
		Graphics->mouse.map = 0;
	}
}

static bool GraphicsInitializeBall(GraphicsClass* Graphics)
{
	Coord position;
	position.x =512;
	position.y = 380;

	int left = 30;
	int right = 994;
	int up = 30;
	int down = 738;

	SpriteSetLimits(Graphics->ball1, up,down,left,right);
	SpriteSetPosition(Graphics->ball1,position);
	SpriteSetMovement(Graphics->ball1,5,5);

	position.x =300;
	position.y = 480;

	SpriteSetLimits(Graphics->ball2, up,down,left,right);
	SpriteSetPosition(Graphics->ball2,position);
	SpriteSetMovement(Graphics->ball2,-5,4);

	return true;

}

void GraphicsGetBall1Limits(GraphicsClass* Graphics, int *up, int *down, int *left, int *right)
{
	*up = SpriteGetLimitU(Graphics->ball1);
	*down = SpriteGetLimitD(Graphics->ball1);
	*left = SpriteGetLimitL(Graphics->ball1);
	*right= SpriteGetLimitR(Graphics->ball1);

}
void GraphicsGetBall2Limits(GraphicsClass* Graphics, int *up, int *down, int *left, int *right)
{
	*up = SpriteGetLimitU(Graphics->ball2);
	*down = SpriteGetLimitD(Graphics->ball2);
	*left = SpriteGetLimitL(Graphics->ball2);
	*right= SpriteGetLimitR(Graphics->ball2);

}
Coord GraphicsGetBall1Position(GraphicsClass* Graphics)
{
	return SpriteGetPosition(Graphics->ball1);
}
Coord GraphicsGetBall2Position(GraphicsClass* Graphics)
{
	return SpriteGetPosition(Graphics->ball2);
}
void GraphicsSetBall1Limits(GraphicsClass* Graphics, int up, int down, int left, int right)
{
	SpriteSetLimits(Graphics->ball1, up,down,left,right);
}
void GraphicsSetBall2Limits(GraphicsClass* Graphics, int up, int down, int left, int right)
{
	SpriteSetLimits(Graphics->ball2, up,down,left,right);
}
SpriteClass* GraphicsGetBall1(GraphicsClass* Graphics)
{
	return Graphics->ball1;
}
SpriteClass* GraphicsGetBall2(GraphicsClass* Graphics)
{
	return Graphics->ball2;
}

static bool GraphicsInitializeNumbers(GraphicsClass* Graphics)
{
	Graphics->currentNumber = 0;
	Graphics->numbersPosition.x = Graphics->XResolution * 9 / 10;
	Graphics->numbersPosition.y = Graphics->YResolution * 4 / 1000;

	Graphics->numbers[0] = GraphicsReadXPM(zero);
	Graphics->numbers[1] = GraphicsReadXPM(one);
	Graphics->numbers[2] = GraphicsReadXPM(two);
	Graphics->numbers[3] = GraphicsReadXPM(three);
	Graphics->numbers[4] = GraphicsReadXPM(four);
	Graphics->numbers[5] = GraphicsReadXPM(five);
	Graphics->numbers[6] = GraphicsReadXPM(six);
	Graphics->numbers[7] = GraphicsReadXPM(seven);
	Graphics->numbers[8] = GraphicsReadXPM(eight);
	Graphics->numbers[9] = GraphicsReadXPM(nine);

	unsigned int i;
	for(i = 0; i < 10; i++)
		if(Graphics->numbers[i] == 0)
			return false;

	return true;
}
static void GraphicsShutdownNumbers(GraphicsClass* Graphics)
{
	unsigned long i;

	for(i = 0; i < 10; i++)
	{
		if(Graphics->numbers[i])
		{
			free(Graphics->numbers[i]);
			Graphics->numbers[i] = 0;
		}
	}
}

//============================= OPERATIONS ===================================

static bool GraphicsGetVBEInfo(GraphicsClass* Graphics, vbe_mode_info_t* pVmi)
{
	struct reg86u reg86;
	mmap_t address;

	// Function 0x01 is a real mode function
	if(lm_init() != 0)
	{
		GraphicsError("\t GraphicsClass.c :: GraphicsGetVBEInfo : lm_init() failed \n");
		return false;
	}
	if(lm_alloc(sizeof(vbe_mode_info_t), &address) == 0)
	{
		GraphicsError("\t GraphicsClass.c :: GraphicsGetVBEInfo : lm_alloc() failed \n");
		return false;
	}

	reg86.u.w.ax = GR_FUNC_GET_VBE_MODE_INFO;
	reg86.u.w.es = PB2BASE(address.phys);
	reg86.u.w.di = PB2OFF(address.phys);
	reg86.u.w.cx = BIT(GR_LINEAR_MODEL_BIT) | Graphics->mode;
	reg86.u.b.intno = GR_INT_NUMBER;

	sys_int86(&reg86);
	if(reg86.u.b.al != 0x4F || reg86.u.b.ah != 0x0)
	{
		printf("\t GraphicsClass :: GraphicsGetVBEInfo : sys_int86() failed - AL: %X, AH: %X \n", reg86.u.b.al, reg86.u.b.ah);
		lm_free(&address);
		return false;
	}

	*pVmi = *(vbe_mode_info_t*) address.virtual;

	lm_free(&address);

	return true;
}
static bool GraphicsSetVBEMode(GraphicsClass* Graphics, unsigned short mode)
{
	struct reg86u reg86;

	// Set VBE Mode
	reg86.u.b.intno = GR_INT_NUMBER;
	reg86.u.w.ax = GR_FUNC_SET_VBE_MODE;
	reg86.u.w.bx = mode | BIT(GR_LINEAR_MODEL_BIT);
	if(sys_int86(&reg86) != OK)
	{
		GraphicsError("\t GraphicsClass.c :: GraphicsSetVBEMode : sys_int86() failed \n");
		return false;
	}

	Graphics->mode = mode;

	return true;
}
static void GraphicsFill(GraphicsClass* Graphics, unsigned long color)
{
	unsigned long i, j;

	while(i < Graphics->XResolution*Graphics->YResolution)
	{
		Graphics->backBuffer[i] = color;
		i++;
	}
}
static bool GraphicsSetPixel(GraphicsClass* Graphics, unsigned long x, unsigned long y, unsigned long color)
{
	unsigned long i;

	if(x < 0 || y < 0 || x > Graphics->XResolution || y > Graphics->YResolution)
	{
		printf("\t GraphicsClass.c :: GraphicsSetPixel : values out of range - x: %d, y: %d \n", x, y);
		return false;
	}

	i = x + y * Graphics->XResolution;
	Graphics->backBuffer[i] = (short) color;

	return true;
}
static unsigned long GraphicsGetPixel(GraphicsClass* Graphics, unsigned long x, unsigned long y)
{
	if(x < 0 || y < 0 || x > Graphics->XResolution || y > Graphics->YResolution)
	{
		GraphicsError("\t GraphicsClass.c :: GraphicsGetPixel : values out of range \n");
		return -1;
	}

	return Graphics->backBuffer[ x + y*Graphics->XResolution];
}
static bool GraphicsDrawLine(GraphicsClass* Graphics, unsigned long xi, unsigned long yi, unsigned long xf, unsigned long yf, unsigned long color)
{
	double m,b;
	unsigned long position, i;

	if(xi < 0 || yi < 0 || xi > Graphics->XResolution || yi > Graphics->YResolution || xf < 0 || yf < 0 || xf > Graphics->XResolution || yf > Graphics->YResolution)
	{
		GraphicsError("\t GraphicsClass.c :: GraphicsDrawLine : values out of range \n");
		return false;
	}

	m = (double) (yf-yi) / (double) (xf-xi);
	b =  (double) yi-m * (double) xi;
	i = xi;

	while(i <= xf)
	{
		unsigned long y = (unsigned long) (m*i + b + 0.5);
		position = i + y*Graphics->XResolution;
		Graphics->backBuffer[position] = color;
		i++;
	}

	return true;
}


void GraphicsFillSpace(GraphicsClass* Graphics, int orientation)
{
	/*
	int up ,down,left,right, y;
	GraphicsGetBallLimits(Graphics,&up,&down,&left,&right);
	limits_t limits;



	if(orientation == 0)
	{
		limits.left = left;
		limits.right = right;
		if(up != 30)
			limits.up = 30;
		else
			limits.up = down;

		if(down != 730)
			limits.down = 730;
		else
			limits.down = up;
	}

	if(orientation == 1)
	{
		limits.up = up;
		limits.down = down;
		if(30 != left)
			limits.left = 30;
		else
			limits.left = right;
		if(994 != right)
			limits.right = 994;
		else
			limits.right = left;
	}




	for(y = limits.up; y <= limits.down; y++)
	{
		printf("Limits: %d , %d", limits.up, limits.down);
		memset(Graphics->walls + limits.left + y * Graphics->XResolution, 7, (limits.right - limits.left)*2);
	}*/
}

static void GraphicsDrawBackground(GraphicsClass* Graphics)
{
	unsigned int color = 7;
	unsigned int thickness = 30;

	GraphicsDrawHorizontalLine(Graphics, 0, 0, Graphics->XResolution, thickness, color);
	GraphicsDrawHorizontalLine(Graphics, Graphics->YResolution - thickness, 0, Graphics->XResolution, thickness, color);
	GraphicsDrawVerticalLine(Graphics, 0, 0, Graphics->YResolution, thickness, color);
	GraphicsDrawVerticalLine(Graphics, Graphics->XResolution - thickness, 0, Graphics->YResolution, thickness, color);
}
void GraphicsDrawHorizontalLine(GraphicsClass* Graphics, unsigned long y, unsigned long xi, unsigned long xf, unsigned long thickness, unsigned int color)
{
	unsigned long i;
	for(i = 0; i < thickness; i++)
		memset(Graphics->walls + (y + i) * Graphics->XResolution + xi, color, sizeof(short)*(xf-xi));
}
void GraphicsDrawVerticalLine(GraphicsClass* Graphics, unsigned long x, unsigned long yi, unsigned long yf, unsigned long thickness, unsigned int color)
{
	unsigned long i;
	for(i = yi; i < yf; i++)
		memset(Graphics->walls + i * Graphics->XResolution + x, color, sizeof(short)*thickness);
}

bool GraphicsDrawSprite(GraphicsClass* Graphics, SpriteClass* sp)
{
	unsigned long i, j, m;
	Coord position, dimensions;
	short* map;

	position = SpriteGetPosition(sp);
	dimensions = SpriteGetDimensions(sp);
	map = SpriteGetMap(sp);

	for(i = position.y, m = 0; i < position.y + dimensions.y; i++)
	{
		for(j = position.x; j < position.x + dimensions.x; j++, m++)
		{
			if(m >= dimensions.x * dimensions.y)
				return false;

			if(map[m] != 0)
				if(!GraphicsSetPixel(Graphics, j, i, map[m]))
					return false;
		}
	}

	return true;
}
bool GraphicsDrawSpriteDir(GraphicsClass* Graphics, SpriteClass* sp, directions_t direction)
{
	unsigned long i, j, m;
	Coord position, dimensions;
	short* map;

	position = SpriteGetPosition(sp);
	dimensions = SpriteGetDimensions(sp);
	map = SpriteGetMap(sp);

	switch(direction)
	{
	case east: case south:
		m = dimensions.x*dimensions.y - 1;
		break;
	case north: case west:
		m = 0;
		break;
	}

	for(i = 0; i < dimensions.y; i++)
	{
		for(j = 0; j < dimensions.x; j++)
		{
			if(m >= dimensions.x * dimensions.y)
				return false;

			if(map[m] != 0)
			{
				switch(direction)
				{
				case east: case west:
					if(!GraphicsSetPixel(Graphics, i + position.x, j + position.y, map[m]))
						return false;
					break;
				case north: case south:
					if(!GraphicsSetPixel(Graphics, j + position.x, i + position.y, map[m]))
						return false;
				}
			}

			switch(direction)
			{
			case east: case south:
				m--;
				break;
			case north: case west:
				m++;
				break;
			}
		}
	}
	return true;
}
bool GraphicsCleanSpriteTrace(GraphicsClass* Graphics, SpriteClass* sp)
{
	Coord lastPosition;
	Coord dimensions;
	unsigned int i, j;

	lastPosition = SpriteGetLastPosition(sp);
	dimensions = SpriteGetDimensions(sp);

	for(i = lastPosition.x; i < lastPosition.x + dimensions.x; i++)
		for(j = lastPosition.y; j < lastPosition.y + dimensions.y; j++)
			if(!GraphicsSetPixel(Graphics, i, j, 0))
				return false;

	return true;
}

static short* GraphicsReadXPM(char* map[])
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
		printf("\t GraphicsReadXPM: failed to read \n");
		return 0;
	}

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

	return pix;
}
static void GraphicsError(char* message)
{
	FILE* file;
	file = fopen("log.txt", "a");
	if(file == 0)
		return;

	fprintf(file, "%s\n", message);

	fclose(file);
}
