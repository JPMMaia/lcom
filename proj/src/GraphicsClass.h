#ifndef _GRAPHICS_CLASS_H_
#define _GRAPHICS_CLASS_H_

#include "boolType.h"
#include "VBEModeInfoStruct.h"
#include "SpriteClass.h"
#include "InputClass.h"

#if(1)
#define BIT(n) 				(0x01<<(n))
#define PB2BASE(x) 			(((x) >> 4) & 0x0F000)
#define PB2OFF(x) 			((x) & 0x0FFFF)

#define GR_LINEAR_MODEL_BIT 			14

#define GR_AH_FUNCTION_CALL_FAILED		0x01
#define GR_AH_FUNCTION_NOT_SUPPORTED 	0x02
#define GR_AH_FUNCTION_NOT_VALID		0x03

#define GR_MODE_640x480_8BITS			0x101
#define GR_MODE_800x600_8BITS			0x103
#define GR_MODE_1024x768_8BITS			0x105
#define GR_MODE_1280x1024_8BITS			0x107
#define GR_MODE_1024x768_32BITS			0x118
#define GR_MODE_TEXT_MODE				0x03

#define GR_INT_NUMBER				0x10
#define GR_FUNC_GET_VBE_MODE_INFO	0x4F01
#define GR_FUNC_SET_VBE_MODE		0x4F02

#endif

typedef struct graphics_class GraphicsClass;

GraphicsClass* newGraphics();
void deleteGraphics(GraphicsClass* Graphics);

bool GraphicsInitialize(GraphicsClass* Graphics, unsigned short mode, InputClass* Input);
void GraphicsShutdown(GraphicsClass* Graphics);

void GraphicsDraw(GraphicsClass* Graphics);
void GraphicsShowConfig(GraphicsClass* Graphics);
bool GraphicsDrawMouseCursor(GraphicsClass* Graphics);
void GraphicsFrame(GraphicsClass* Graphics);
bool GraphicsClearTraceMouseCursor(GraphicsClass* Graphics, int radius);
bool GraphicsDrawCircle(GraphicsClass* Graphics, int radius,  int color);
void GraphicsDrawTriangle(GraphicsClass* Graphics);
bool GraphicsDrawBall(GraphicsClass* Graphics);
bool GraphicsDrawNumber(GraphicsClass* Graphics, unsigned int number, unsigned int x, unsigned int y);

unsigned int* GraphicsGetCurrentNumber(GraphicsClass* Graphics);

bool GraphicsIsPixelSet(GraphicsClass* Graphics, unsigned int x, unsigned int y);

static bool GraphicsInitializeVBE(GraphicsClass* Graphics, unsigned short mode);
static bool GraphicsShutdownVBE(GraphicsClass* Graphics);

static bool GraphicsInitializeSprites(GraphicsClass* Graphics);
static void GraphicsShutdownSprites(GraphicsClass* Graphics);

static bool GraphicsInitializeMouseCursor(GraphicsClass* Graphics, int color);
static void GraphicsShutdownMouseCursor(GraphicsClass* Graphics);

static bool GraphicsInitializeNumbers(GraphicsClass* Graphics);
static void GraphicsShutdownNumbers(GraphicsClass* Graphics);

static bool GraphicsInitializeBall(GraphicsClass* Graphics);
static bool GraphicsShutdownBall(GraphicsClass* Graphics);

void GraphicsGetBall1Limits(GraphicsClass* Graphics, int *up, int *down, int *left, int *right);
Coord GraphicsGetBall1Position(GraphicsClass* Graphics);
void GraphicsSetBall1Limits(GraphicsClass* Graphics, int up, int down, int left, int right);
SpriteClass* GraphicsGetBall1(GraphicsClass* Graphics);
void GraphicsGetBall2Limits(GraphicsClass* Graphics, int *up, int *down, int *left, int *right);
Coord GraphicsGetBall2Position(GraphicsClass* Graphics);
void GraphicsSetBall2Limits(GraphicsClass* Graphics, int up, int down, int left, int right);
SpriteClass* GraphicsGetBall2(GraphicsClass* Graphics);
unsigned long GraphicsGetXResolution(GraphicsClass* Graphics);
unsigned long GraphicsGetYResolution(GraphicsClass* Graphics);

static bool GraphicsGetVBEInfo(GraphicsClass* Graphics, vbe_mode_info_t* pVmi);
static bool GraphicsSetVBEMode(GraphicsClass* Graphics, unsigned short mode);
static void GraphicsFill(GraphicsClass* Graphics, unsigned long color);
static bool GraphicsSetPixel(GraphicsClass* Graphics, unsigned long x, unsigned long y, unsigned long color);
static unsigned long GraphicsGetPixel(GraphicsClass* Graphics, unsigned long x, unsigned long y);
static bool GraphicsDrawLine(GraphicsClass* Graphics, unsigned long xi, unsigned long yi, unsigned long xf, unsigned long yf, unsigned long color);

void GraphicsFillSpace(GraphicsClass* Graphics, int orientation); // 0 for horizontal, 1 for vertical
static void GraphicsDrawBackground(GraphicsClass* Graphics);
void GraphicsDrawHorizontalLine(GraphicsClass* Graphics, unsigned long y, unsigned long xi, unsigned long xf, unsigned long thickness, unsigned int color);
void GraphicsDrawVerticalLine(GraphicsClass* Graphics, unsigned long x, unsigned long yi, unsigned long yf, unsigned long thickness, unsigned int color);

bool GraphicsDrawSprite(GraphicsClass* Graphics, SpriteClass* sp);
static bool GraphicsDrawSpriteDir(GraphicsClass* Graphics, SpriteClass* sp, directions_t direction);
static bool GraphicsCleanSpriteTrace(GraphicsClass* Graphics, SpriteClass* sp);

static short* GraphicsReadXPM(char* map[]);
static void GraphicsError(char* message);

#endif
