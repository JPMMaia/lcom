#ifndef _INPUT_CLASS_H_
#define _INPUT_CLASS_H_

#include "MouseClass.h"
#include "KeyboardClass.h"
#include "ListClass.h"
#include "CoordStruct.h"

typedef struct input_class InputClass;

typedef enum
{
	ms_LB_down = 0,
	ms_MB_down = 1,
	ms_RB_down = 2,

	ms_LB_up = 3,
	ms_MB_up = 4,
	ms_RB_up = 5
} ms_event;

typedef enum
{
	none_dir = 0,
	east = 1,
	north = 2,
	west = -1,
	south = -2
} directions_t;

InputClass* newInput();
void deleteInput(InputClass* Input);

bool InputInitialize(InputClass* Input, int x0, int y0, int screenWidth, int screenHeight);
void InputShutdown(InputClass* Input);

bool InputHandleMouseInt(InputClass* Input);
directions_t InputMSCheckDirection(InputClass* Input);

bool InputHandleKeyboardInt(InputClass* Input);

ListClass* InputGetKBEvents(InputClass* Input);
ListClass* InputGetMSEvents(InputClass* Input);
Coord InputGetMouseCursor(InputClass* Input);
void InputMSLock(InputClass* Input, bool lock);

void InputMSEnableMovementReport(InputClass* Input, bool enable);

bool InputIsKBKeyDown(InputClass* Input, kb_key key);
bool InputIsMSKeyDown(InputClass* Input, ms_key key);
bool InputIsMSLocked(InputClass* Input);
bool InputMSEvent(InputClass* Input);


static bool InputInitializeMouse(InputClass* Input, int x0, int y0, int screenWidth, int screenHeight);
static void InputShutdownMouse(InputClass* Input);

static void InputInitializeKeyboard(InputClass* Input);
static void InputShutdownKeyboard(InputClass* Input);

static void InputMSSetPosition(InputClass* Input, Coord* currentMouseCursor);
static void InputProcessMouseMovement(InputClass* Input);
static void InputProcessMouseKeys(InputClass* Input);

static void InputProcessKeyboardKeys(InputClass* Input);

#endif
