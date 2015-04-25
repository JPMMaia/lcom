#include "GameLogicClass.h"
#include "ListClass.h"
#include "WallsClass.h"
#include "MenuClass.h"
#include "MenuSprites.h"

typedef struct
{
	directions_t direction;
	Coord position;
	unsigned int thickness;
	unsigned int color;
	unsigned int pixelsPerFrame;

	bool building;
} wall_struct;


struct game_logic_class
{
	unsigned int* counter;
	MenuClass* Menu;
	bool menus;

	// Interfaces
	InputClass* Input;
	GraphicsClass* Graphics;
	wall_struct wall1;
	wall_struct wall2;
	unsigned int area1;
	unsigned int area2;
	unsigned int win_area;

	// Events
	ListClass* MSEvents;
	ListClass* RTCEvents;
};


/////////////////////////////// PUBLIC ///////////////////////////////////////

//============================= LIFECYCLE ====================================

GameLogicClass* newGameLogic()
{
	GameLogicClass* GameLogic;
	GameLogic = (GameLogicClass*) malloc(sizeof(GameLogicClass));

	GameLogic->Input = 0;
	GameLogic->Graphics = 0;

	return GameLogic;
}
void deleteGameLogic(GameLogicClass* GameLogic)
{
	free(GameLogic);
}

void GameLogicInitialize(GameLogicClass* GameLogic, ClockClass* Clock, InputClass* Input, GraphicsClass* Graphics)
{
	wall_struct wall1;
	wall_struct wall2;
	GameLogic->Input = Input;
	GameLogic->MSEvents = InputGetMSEvents(Input);

	GameLogic->Graphics = Graphics;

	wall1.thickness = 30;
	wall1.color = 7;
	wall1.pixelsPerFrame = 6;
	wall1.building = false;
	GameLogic->wall1 = wall1;

	wall2.thickness = 30;
	wall2.color = 7;
	wall2.pixelsPerFrame = 6;
	wall2.building = false;
	GameLogic->wall2 = wall2;

	GameLogic->area1 = 100;
	GameLogic->area2 = 100;
	GameLogic->win_area = 10;

	GameLogicInitializeMenus(GameLogic);
	if(GameLogic->Graphics)
	{
		GameLogic->counter = GraphicsGetCurrentNumber(Graphics);
		*GameLogic->counter = 60;
	}


	GameLogic->RTCEvents = ClockGetRTCEvents(Clock);
}
void GameLogicShutdown(GameLogicClass* GameLogic)
{
	GameLogic->counter = 0;

	GameLogic->Graphics = 0;

	GameLogic->MSEvents = 0;
	GameLogic->Input = 0;

	GameLogicShutdownMenus(GameLogic);
}


//============================= OPERATIONS ===================================

bool GameLogicRun(GameLogicClass* GameLogic)
{
	if(GameLogic->menus)
	{
		if(!GameLogicRunMenu(GameLogic))
			return false;
	}
	else
	{
		if(!GameLogicProcessMSEvents(GameLogic))
			return false;

		GameLogicProcessMSDirections(GameLogic);

		if(!GameLogicProcessKBEvents(GameLogic))
			return false;

		if(!GameLogicProcessRTCEvents(GameLogic))
			return false;

		if(GameLogic->Graphics)
		{
			if(!GameLogicProcessBall(GameLogic))
			{
				printf("Perdeu!");
				return false;
			}

			if(!GameLogicHasWon(GameLogic))
			{
				printf("Ganhou!");
				return false;
			}
		}
	}

	GraphicsFrame(GameLogic->Graphics);

	return true;
}
bool GameLogicDrawWall(GameLogicClass* GameLogic)
{
	unsigned int x1 = GameLogic->wall1.position.x;
	unsigned int y1 = GameLogic->wall1.position.y;
	unsigned int x2 = GameLogic->wall2.position.x;
	unsigned int y2 = GameLogic->wall2.position.y;
	unsigned int pixelsPerFrame1 = GameLogic->wall1.pixelsPerFrame;
	unsigned int thickness1 = GameLogic->wall1.thickness;
	unsigned int color1 = GameLogic->wall1.color;
	unsigned int pixelsPerFrame2 = GameLogic->wall2.pixelsPerFrame;
	unsigned int thickness2 = GameLogic->wall2.thickness;
	unsigned int color2 = GameLogic->wall2.color;
	int up1 ,down1,left1,right1;
	GraphicsGetBall1Limits(GameLogic->Graphics,&up1,&down1,&left1,&right1);
	int up2 ,down2,left2,right2;
	GraphicsGetBall2Limits(GameLogic->Graphics,&up2,&down2,&left2,&right2);
	Coord pos1 = GraphicsGetBall1Position(GameLogic->Graphics);
	Coord pos2 = GraphicsGetBall2Position(GameLogic->Graphics);
	Coord mouseCursor;
	mouseCursor = InputGetMouseCursor(GameLogic->Input);

	if(GameLogic->wall1.building)
	{
		switch(GameLogic->wall1.direction)
		{
		case east:
			if(GraphicsIsPixelSet(GameLogic->Graphics, x1 + 30 + pixelsPerFrame1, y1))
			{
				if(!(mouseCursor.x < left1 || mouseCursor.x > right1 || mouseCursor.y < up1 || mouseCursor.y > down1))
				{
					if(pos1.y < y1)
						down1 = y1;
					else
						up1 = y1+thickness1;
					GraphicsSetBall1Limits(GameLogic->Graphics, up1,down1,left1,right1);
				}
				if(!(mouseCursor.x < left2 || mouseCursor.x > right2 || mouseCursor.y < up2 || mouseCursor.y > down2))
				{
					if(pos2.y < y1)
						down2 = y1;
					else
						up2 = y1+thickness1;
					GraphicsSetBall2Limits(GameLogic->Graphics, up2,down2,left2,right2);
				}

				//GraphicsFillSpace(GameLogic->Graphics, 0);
				GameLogic->wall1.building = false;
			}
			GraphicsDrawHorizontalLine(GameLogic->Graphics, y1, x1, x1 + 30 + pixelsPerFrame1, thickness1, color1);
			GameLogic->wall1.position.x = x1 + pixelsPerFrame1;

			break;
		case north:
			if(GraphicsIsPixelSet(GameLogic->Graphics, x1, y1 - pixelsPerFrame1))
			{
				if(!(mouseCursor.x < left1 || mouseCursor.x > right1 || mouseCursor.y < up1 || mouseCursor.y > down1))
				{
					if(pos1.x < x1)
						right1 = x1;
					else
						left1 = x1+thickness1;
					GraphicsSetBall1Limits(GameLogic->Graphics, up1,down1,left1,right1);
				}
				if(!(mouseCursor.x < left2 || mouseCursor.x > right2 || mouseCursor.y < up2 || mouseCursor.y > down2))
				{
					if(pos2.x < x1)
						right2 = x1;
					else
						left2 = x1+thickness1;
					GraphicsSetBall2Limits(GameLogic->Graphics, up2,down2,left2,right2);
				}
				//GraphicsFillSpace(GameLogic->Graphics, 1);
				GameLogic->wall1.building = false;
			}
			GraphicsDrawVerticalLine(GameLogic->Graphics, x1, y1 - pixelsPerFrame1, y1 + 30, thickness1, color1);
			GameLogic->wall1.position.y = y1 - pixelsPerFrame1;

			break;
		case west:
			if(GraphicsIsPixelSet(GameLogic->Graphics, x1 - pixelsPerFrame1, y1))
			{
				if(!(mouseCursor.x < left1 || mouseCursor.x > right1 || mouseCursor.y < up1 || mouseCursor.y > down1))
				{
					if(pos1.y < y1)
						down1 = y1;
					else
						up1 = y1+thickness1;
					GraphicsSetBall1Limits(GameLogic->Graphics, up1,down1,left1,right1);
				}
				if(!(mouseCursor.x < left2 || mouseCursor.x > right2 || mouseCursor.y < up2 || mouseCursor.y > down2))
				{
					if(pos2.y < y1)
						down2 = y1;
					else
						up2 = y1+thickness1;
					GraphicsSetBall2Limits(GameLogic->Graphics, up2,down2,left2,right2);
				}
				//GraphicsFillSpace(GameLogic->Graphics, 0);
				GameLogic->wall1.building = false;
			}
			GraphicsDrawHorizontalLine(GameLogic->Graphics, y1, x1 - pixelsPerFrame1, x1 + 30, thickness1, color1);
			GameLogic->wall1.position.x = x1 - pixelsPerFrame1;
			break;
		case south:
			if(GraphicsIsPixelSet(GameLogic->Graphics, x1, y1 + 30 + pixelsPerFrame1))
			{
				if(!(mouseCursor.x < left1 || mouseCursor.x > right1 || mouseCursor.y < up1 || mouseCursor.y > down1))
				{
					if(pos1.x < x1)
						right1 = x1;
					else
						left1 = x1+thickness1;
					GraphicsSetBall1Limits(GameLogic->Graphics, up1,down1,left1,right1);
				}
				if(!(mouseCursor.x < left2 || mouseCursor.x > right2 || mouseCursor.y < up2 || mouseCursor.y > down2))
				{
					if(pos2.x < x1)
						right2 = x1;
					else
						left2 = x1+thickness1;
					GraphicsSetBall2Limits(GameLogic->Graphics, up2,down2,left2,right2);
				}
				//GraphicsFillSpace(GameLogic->Graphics, 1);
				GameLogic->wall1.building = false;
			}
			GraphicsDrawVerticalLine(GameLogic->Graphics, x1, y1, y1 + 30 + pixelsPerFrame1, thickness1, color1);
			GameLogic->wall1.position.y = y1 + pixelsPerFrame1;


			break;
		}
	}


	if(GameLogic->wall2.building)
	{
		switch(GameLogic->wall2.direction)
		{
		case east:
			if(GraphicsIsPixelSet(GameLogic->Graphics, x2 + 30 + pixelsPerFrame2, y2))
				GameLogic->wall2.building = false;

			GraphicsDrawHorizontalLine(GameLogic->Graphics, y2, x2, x2 + 30 + pixelsPerFrame2, thickness2, color2);
			GameLogic->wall2.position.x = x2 + pixelsPerFrame2;

			break;
		case north:
			if(GraphicsIsPixelSet(GameLogic->Graphics, x2, y2 - pixelsPerFrame2))
				GameLogic->wall2.building = false;
			GraphicsDrawVerticalLine(GameLogic->Graphics, x2, y2 - pixelsPerFrame2, y2 + 30, thickness2, color2);
			GameLogic->wall2.position.y = y2 - pixelsPerFrame2;
			break;
		case west:
			if(GraphicsIsPixelSet(GameLogic->Graphics, x2 - pixelsPerFrame2, y2))
				GameLogic->wall2.building = false;
			GraphicsDrawHorizontalLine(GameLogic->Graphics, y2, x2 - pixelsPerFrame2, x2 + 30, thickness2, color2);
			GameLogic->wall2.position.x = x2 - pixelsPerFrame2;
			break;
		case south:
			if(GraphicsIsPixelSet(GameLogic->Graphics, x2, y2 + 30 + pixelsPerFrame2))
				GameLogic->wall2.building = false;
			GraphicsDrawVerticalLine(GameLogic->Graphics, x2, y2, y2 + 30 + pixelsPerFrame2, thickness2, color2);
			GameLogic->wall2.position.y = y2 + pixelsPerFrame2;
			break;
		}
	}
}
bool GameLogicProcessBall(GameLogicClass* GameLogic)
{
	if(GameLogic->wall1.building || GameLogic->wall2.building)
	{
		SpriteClass* sp = GraphicsGetBall1(GameLogic->Graphics);
		Coord dimensions = SpriteGetDimensions(sp);
		Coord position = SpriteGetPosition(sp);


		if(GraphicsIsPixelSet(GameLogic->Graphics, position.x, position.y))
		{
			return false;
		}

		if(GraphicsIsPixelSet(GameLogic->Graphics, position.x, position.y + dimensions.x))
		{
			return false;
		}

		if(GraphicsIsPixelSet(GameLogic->Graphics, position.x+dimensions.x, position.y))
		{
			return false;
		}

		if(GraphicsIsPixelSet(GameLogic->Graphics, position.x+dimensions.x, position.y + dimensions.x))
		{
			return false;
		}

		sp = GraphicsGetBall2(GameLogic->Graphics);
		dimensions = SpriteGetDimensions(sp);
		position = SpriteGetPosition(sp);


		if(GraphicsIsPixelSet(GameLogic->Graphics, position.x, position.y))
		{
			return false;
		}

		if(GraphicsIsPixelSet(GameLogic->Graphics, position.x, position.y + dimensions.x))
		{
			return false;
		}

		if(GraphicsIsPixelSet(GameLogic->Graphics, position.x+dimensions.x, position.y))
		{
			return false;
		}

		if(GraphicsIsPixelSet(GameLogic->Graphics, position.x+dimensions.x, position.y + dimensions.x))
		{
			return false;
		}
	}



	return true;

}
void GameLogicCalculateArea(GameLogicClass* GameLogic)
{
	int TArea = (GraphicsGetXResolution(GameLogic->Graphics)-60) * (GraphicsGetYResolution(GameLogic->Graphics)-60);
	int up ,down,left,right;
	GraphicsGetBall1Limits(GameLogic->Graphics,&up,&down,&left,&right);
	GameLogic->area1 = (down-up)*(right-left);
	GameLogic->area1 =GameLogic->area1* 100/TArea;

	GraphicsGetBall2Limits(GameLogic->Graphics,&up,&down,&left,&right);
	GameLogic->area2 = (down-up)*(right-left);
	GameLogic->area2 =GameLogic->area2* 100/TArea;


}
void GameLogicDrawAll(GameLogicClass* GameLogic)
{
	if(!GameLogic->menus)
	{
		GraphicsDraw(GameLogic->Graphics);

		if(GameLogicIsBuildingWall(GameLogic))
		{
			GameLogicDrawWall(GameLogic);
			if(!GameLogic->wall1.building && !GameLogic->wall2.building)
			{
				GameLogicCalculateArea(GameLogic);
			}
		}
		if(GameLogic->area1 != 100 && GameLogic->area2 != 100 )
		{
			GraphicsDrawNumber(GameLogic->Graphics, GameLogic->area1/10,0 ,1);
			GraphicsDrawNumber(GameLogic->Graphics, GameLogic->area1%10, 15,1);
			GraphicsDrawNumber(GameLogic->Graphics, GameLogic->area2/10,0 ,40);
			GraphicsDrawNumber(GameLogic->Graphics, GameLogic->area2%10, 15,40);
		}

		GraphicsDrawNumber(GameLogic->Graphics, GameLogic->win_area/10,0 ,738);
		GraphicsDrawNumber(GameLogic->Graphics, GameLogic->win_area%10, 15,738);
		GraphicsFrame(GameLogic->Graphics);
	}
}

bool GameLogicRunMenu(GameLogicClass* GameLogic)
{
	if(GameLogic->Graphics)
		MenuPrint(GameLogic->Menu);

	if(!MenuRun(GameLogic->Menu))
		return false;

	return true;
}

bool GameLogicHasWon(GameLogicClass* GameLogic)
{
	if(GameLogic->area1 <= GameLogic->win_area && GameLogic->area2 <= GameLogic->win_area)
		return false;
	return true;
}

//=============================   ACCESS   ===================================

bool GameLogicIsBuildingWall(GameLogicClass* GameLogic)
{
	if(GameLogic->wall1.building)
		return true;
	if(GameLogic->wall2.building)
		return true;
	else
		return false;
}
void GameLogicEnableMenus(GameLogicClass* GameLogic, bool enable)
{
	GameLogic->menus = enable;
}

/////////////////////////////// PRIVATE //////////////////////////////////////

static bool GameLogicInitializeWalls(GameLogicClass* GameLogic, unsigned int thickness, unsigned int color, unsigned int x0, unsigned int y0, unsigned int xf, unsigned int yf)
{

	return true;
}
static bool GameLogicInitializeMenus(GameLogicClass* GameLogic)
{
	GameLogic->Menu = newMenu();
	if(!MenuInitialize(GameLogic->Menu, start, GameLogic->Graphics, GameLogic->Input, (struct GameLogicClass*) GameLogic))
		return false;

	MenuAddFunction(GameLogic->Menu, start, runStart);
	MenuAddFunction(GameLogic->Menu, exitXPM, 0);

	GameLogic->menus = true;

	return true;
}
static void GameLogicShutdownWalls(GameLogicClass* GameLogic)
{
}
static void GameLogicShutdownMenus(GameLogicClass* GameLogic)
{
	if(GameLogic->Menu)
	{
		MenuShutdown(GameLogic->Menu);
		deleteMenu(GameLogic->Menu);
		GameLogic->Menu = 0;
	}
}

//============================= OPERATIONS ===================================

static bool GameLogicProcessMSEvents(GameLogicClass* GameLogic)
{
	// Process all the MS events
	while(!ListIsEmpty(GameLogic->MSEvents))
	{
		ms_event event;
		event = *(ms_event*) ListFront(GameLogic->MSEvents);
		ListPopFront(GameLogic->MSEvents);

		switch(event)
		{
		case ms_LB_down:
			if(!GameLogic->wall1.building)
				InputMSLock(GameLogic->Input, true);
			if(!GameLogic->wall2.building)
				InputMSLock(GameLogic->Input, true);
			break;
		case ms_MB_down:
			break;
		case ms_RB_down:
			break;
		case ms_LB_up:
			InputMSLock(GameLogic->Input, false);
			if(GameLogic->wall1.direction != none_dir)
				GameLogic->wall1.building = true;
			if(GameLogic->wall2.direction != none_dir)
				GameLogic->wall2.building = true;
			break;
		case ms_MB_up:
			break;
		case ms_RB_up:
			break;
		}
	}

	return true;
}
static void GameLogicProcessMSDirections(GameLogicClass* GameLogic)
{
	directions_t direction;
	Coord mouseCursor;

	if(InputIsMSLocked(GameLogic->Input))
	{
		direction = InputMSCheckDirection(GameLogic->Input);
		mouseCursor = InputGetMouseCursor(GameLogic->Input);

		GameLogic->wall1.direction = direction;
		GameLogic->wall1.position = mouseCursor;
		GameLogic->wall2.direction = -direction;
		GameLogic->wall2.position = mouseCursor;
	}
}


static bool GameLogicProcessKBEvents(GameLogicClass* GameLogic)
{
	ListClass* kbEvents;

	kbEvents = InputGetKBEvents(GameLogic->Input);
	kb_key key;
	while(!ListIsEmpty(kbEvents))
	{
		key = *(kb_key*) ListFront(kbEvents);
		ListPopFront(kbEvents);

		switch(key)
		{
		case kb_esc:
			if(GameLogic->menus)
				return false;
			else
				GameLogic->menus = true;
		default:
			break;
		}
	}

	return true;
}

static bool GameLogicProcessRTCEvents(GameLogicClass* GameLogic)
{
	while(!ListIsEmpty(GameLogic->RTCEvents))
	{
		rtc_int event;
		event = *(rtc_int*) ListFront(GameLogic->RTCEvents);
		ListPopFront(GameLogic->RTCEvents);

		switch(event)
		{
		case rtc_alarm:
			// Game over
			break;
		case rtc_periodic:
			break;
		case rtc_update_ended:
			*GameLogic->counter -= 1;
			if(*GameLogic->counter == 0)
				return false;
			break;
		}
	}

	return true;
}
