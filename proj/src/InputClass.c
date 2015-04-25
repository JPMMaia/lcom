#include <math.h>
#include <stdio.h>

#include "InputClass.h"

#define MS_DEBUG_1 	0
#define MS_DEBUG_2 	0
#define KB_DEBUG	0

struct input_class
{
	// Keyboard
	KeyboardClass* Keyboard;
	bool keyboardKeys[256];
	ListClass* KeyboardEvents;
	bool scancode;

	// Mouse
	MouseClass* Mouse;
	bool mouseKeys[3];
	Coord MouseCursor;
	ListClass* MouseEvents;
	int x0;
	int y0;
	int screenWidth;
	int screenHeight;

	bool mouseLock;
	Coord MouseCursorLock;
	unsigned int innerRange;
	unsigned int outterRange;
};

/////////////////////////////// PUBLIC ///////////////////////////////////////

//============================= LIFECYCLE ====================================

InputClass* newInput()
{
	unsigned int i;

	InputClass* Input;
	Input = (InputClass*) malloc(sizeof(InputClass));

	Input->Mouse = 0;
	Input->MouseEvents = 0;
	Input->Keyboard = 0;

	return Input;
}
void deleteInput(InputClass* Input)
{
	free(Input);
}

bool InputInitialize(InputClass* Input, int x0, int y0, int screenWidth, int screenHeight)
{
	if(!InputInitializeMouse(Input, x0, y0, screenWidth, screenHeight))
		return false;

	InputInitializeKeyboard(Input);

	return true;
}
void InputShutdown(InputClass* Input)
{
	InputShutdownKeyboard(Input);
	InputShutdownMouse(Input);
}


//============================= OPERATIONS ===================================

bool InputHandleMouseInt(InputClass* Input)
{
	if(MouseHandleInt(Input->Mouse) != true)
		return false;

	InputProcessMouseMovement(Input);
	InputProcessMouseKeys(Input);

	if(MS_DEBUG_1)
		printf("MouseCursor: (%d, %d), Lock: (%d, %d) \n", Input->MouseCursor.x, Input->MouseCursor.y, Input->MouseCursorLock.x, Input->MouseCursorLock.y);

	if(Input->mouseLock && MS_DEBUG_2)
		printf("Direction: %d\n", InputMSCheckDirection(Input));

	return true;
}
directions_t InputMSCheckDirection(InputClass* Input)
{
	Coord* cursor = &Input->MouseCursor;
	Coord* lock = &Input->MouseCursorLock;
	Coord delta;

	delta.x = lock->x - cursor->x;
	delta.y = lock->y - cursor->y;

	if(sqrt(delta.x*delta.x + delta.y*delta.y) < Input->innerRange)
		return none_dir;


	float angle, pi;
	angle = (float) atan2((double) delta.y, (double) delta.x);
	pi = atan(1.0)*4.0;

	if(angle >= pi / 4.0 && angle < 3.0*pi/4.0)
		return south;

	if(angle >= 3.0*pi/4.0 || angle < -3.0*pi/4.0)
		return west;

	if(angle >= -3.0*pi/4.0 && angle < -pi/4.0)
		return north;

	if(angle >= -pi/4.0 && angle < pi/4.0)
		return east;

	return none_dir;
}

bool InputHandleKeyboardInt(InputClass* Input)
{
	if(!KeyboardHandleInt(Input->Keyboard))
		return false;

	if(KB_DEBUG)
		KeyboardShowData(Input->Keyboard);

	InputProcessKeyboardKeys(Input);
}

//=============================   ACCESS   ===================================

ListClass* InputGetKBEvents(InputClass* Input)
{
	return Input->KeyboardEvents;
}
ListClass* InputGetMSEvents(InputClass* Input)
{
	return Input->MouseEvents;
}
Coord InputGetMouseCursor(InputClass* Input)
{
	return Input->MouseCursor;
}

void InputMSLock(InputClass* Input, bool lock)
{
	if(lock)
		Input->MouseCursorLock = Input->MouseCursor;

	Input->mouseLock = lock;
}
void InputMSEnableMovementReport(InputClass* Input, bool enable)
{
	MouseEnableMovementReport(Input->Mouse, enable);
}


//============================= INQUIRY    ===================================

bool InputIsKBKeyDown(InputClass* Input, kb_key key)
{
	return Input->keyboardKeys[key];
}
bool InputIsMSKeyDown(InputClass* Input, ms_key key)
{
	return Input->mouseKeys[key];
}
bool InputIsMSLocked(InputClass* Input)
{
	return Input->mouseLock;
}
bool InputMSEvent(InputClass* Input)
{
	if(ListIsEmpty(Input->MouseEvents))
		return false;

	return true;
}


/////////////////////////////// PRIVATE //////////////////////////////////////

//============================= LIFECYCLE ====================================

static bool InputInitializeMouse(InputClass* Input, int x0, int y0, int screenWidth, int screenHeight)
{
	unsigned long i;

	Input->Mouse = newMouse();
	if(!MouseInitialize(Input->Mouse))
		return false;

	// Initialize mouse key presses
	for(i = 0; i < 3; i++)
		Input->mouseKeys[i] = false;

	Input->MouseEvents = newList(sizeof(ms_event));

	Input->x0 = x0;
	Input->y0 = y0;
	Input->screenWidth = screenWidth;
	Input->screenHeight = screenHeight;
	Input->MouseCursor.x = x0;
	Input->MouseCursor.y = y0;

	Input->mouseLock = false;
	Input->MouseCursorLock.x = x0;
	Input->MouseCursorLock.y = y0;
	Input->innerRange = 20;
	Input->outterRange = 50;

	return true;
}
static void InputShutdownMouse(InputClass* Input)
{
	if(Input->Mouse)
	{
		MouseShutdown(Input->Mouse);
		deleteMouse(Input->Mouse);
		Input->Mouse = 0;
	}

	if(Input->MouseEvents)
	{
		deleteList(Input->MouseEvents);
		Input->MouseEvents = 0;
	}
}

static void InputInitializeKeyboard(InputClass* Input)
{
	unsigned long i;

	Input->Keyboard = newKeyboard();

	for(i = 0; i < 256; i++)
		Input->keyboardKeys[i] = false;

	Input->KeyboardEvents = newList(sizeof(kb_key));

	Input->scancode = false;
}
static void InputShutdownKeyboard(InputClass* Input)
{
	if(Input->Keyboard)
	{
		deleteKeyboard(Input->Keyboard);
		Input->Keyboard = 0;
	}

	if(Input->KeyboardEvents)
	{
		deleteList(Input->KeyboardEvents);
		Input->KeyboardEvents = 0;
	}
}


//============================= OPERATIONS ===================================

static void InputProcessMouseMovement(InputClass* Input)
{
	ms_event event;
	Coord delta;
	Coord* currentMouseCursor;

	if(Input->mouseLock)
		currentMouseCursor = &Input->MouseCursorLock;
	else
		currentMouseCursor = &Input->MouseCursor;

	// Get position change
	delta.x = MouseGetDeltaX(Input->Mouse);
	delta.y = MouseGetDeltaY(Input->Mouse);

	// If there is a change
	if(delta.x != 0 || delta.y != 0)
	{
		// Calculate new position
		currentMouseCursor->x += delta.x;
		currentMouseCursor->y += delta.y;

		InputMSSetPosition(Input, currentMouseCursor);
	}
}
static void InputProcessMouseKeys(InputClass* Input)
{
	unsigned long i;

	ms_key keys[3] = { ms_lb, ms_mb, ms_rb };
	ms_event keyDown[3] = { ms_LB_down, ms_MB_down, ms_RB_down };
	ms_event keyUp[3] = { ms_LB_up, ms_MB_up, ms_RB_up };

	for(i = 0; i < 3; i++)
	{
		// If there is a change
		if(InputIsMSKeyDown(Input, keys[i]) != MouseIsBtnDown(Input->Mouse, keys[i]))
		{
			// Apply changes
			Input->mouseKeys[i] = MouseIsBtnDown(Input->Mouse, keys[i]);

			// Generate event
			if(Input->mouseKeys[i] == true)
				ListPushBack(Input->MouseEvents, &keyDown[i]);
			else
				ListPushBack(Input->MouseEvents, &keyUp[i]);
		}
	}
}

static void InputMSSetPosition(InputClass* Input, Coord* currentMouseCursor)
{
	if(Input->mouseLock)
	{
		// Checking if position X is within the range
		if(currentMouseCursor->x < Input->MouseCursor.x - Input->outterRange)
			currentMouseCursor->x = Input->MouseCursor.x - Input->outterRange;
		else if(currentMouseCursor->x >= Input->MouseCursor.x + Input->outterRange)
			currentMouseCursor->x = Input->MouseCursor.x + Input->outterRange;

		// Checking if position Y is within the screen height
		if(currentMouseCursor->y < Input->MouseCursor.y - Input->outterRange)
			currentMouseCursor->y = Input->MouseCursor.y - Input->outterRange;
		else if(currentMouseCursor->y >= Input->MouseCursor.y + Input->outterRange)
			currentMouseCursor->y = Input->MouseCursor.y + Input->outterRange;
	}

	// Checking if position X is within the screen width
	if(currentMouseCursor->x < Input->x0)
		currentMouseCursor->x = Input->x0;
	else if(currentMouseCursor->x >= Input->screenWidth)
		currentMouseCursor->x = Input->screenWidth - 1;

	// Checking if position Y is within the screen height
	if(currentMouseCursor->y < Input->y0)
		currentMouseCursor->y = Input->y0;
	else if(currentMouseCursor->y >= Input->screenHeight)
		currentMouseCursor->y = Input->screenHeight - 1;
}

static void InputProcessKeyboardKeys(InputClass* Input)
{
	unsigned long scancode;
	scancode = KeyboardGetData(Input->Keyboard);

	if(scancode == 0xE0)
	{
		Input->scancode = true;
		return;
	}

	// Handle secondary keys
	if(Input->scancode)
	{
		Input->scancode = false;

		// Handle break codes
		if(scancode & KB_BREAKCODE)
		{
			Input->keyboardKeys[scancode] = false;
			return;
		}

		// Handle make codes
		scancode += KB_BREAKCODE; // Making a shift of 0x80, for secondary keys (arrows for example)
		Input->keyboardKeys[scancode] = true;
		ListPushBack(Input->KeyboardEvents, &scancode);

		return;
	}

	// Handle break codes
	if(scancode & KB_BREAKCODE)
	{
		scancode -= KB_BREAKCODE;
		Input->keyboardKeys[scancode] = false;
		return;
	}

	// Handle make codes
	Input->keyboardKeys[scancode] = true;
	ListPushBack(Input->KeyboardEvents, &scancode);
}
