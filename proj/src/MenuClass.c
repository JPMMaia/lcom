#include "MenuClass.h"
#include "GameLogicClass.h"

typedef struct menu_entry MenuEntry;

struct menu_class
{
	SpriteClass* title;
	GraphicsClass* Graphics;
	InputClass* Input;
	struct GameLogicClass* GameLogic;

	MenuEntry **entries;
	unsigned long numberEntries;
	unsigned long size;
};
struct menu_entry
{
	SpriteClass* description;

	MenuClass* SubMenu;
	void (*function)(struct GameLogicClass*);
};


/////////////////////////////// PUBLIC ///////////////////////////////////////

//============================= LIFECYCLE ====================================

MenuClass* newMenu(char* title[], GraphicsClass* Graphics)
{
	MenuClass* Menu;
	Menu = (MenuClass*) malloc(sizeof(MenuClass));

	return Menu;
}
void deleteMenu(MenuClass* Menu)
{
	free(Menu);
}

bool MenuInitialize(MenuClass* Menu, char* title[], GraphicsClass* Graphics, InputClass* Input, struct GameLogicClass* GameLogic)
{
	Menu->Graphics = Graphics;
	Menu->Input = Input;
	Menu->GameLogic = GameLogic;

	Menu->title = newSprite();
	if(!SpriteInitializeXPM(Menu->title, title))
		return false;

	Menu->numberEntries = 0;
	Menu->size = 0;
	Menu->entries = 0;
	MenuAdjust(Menu);
}
void MenuShutdown(MenuClass* Menu)
{
	if(Menu)
	{
		while(Menu->numberEntries != 0)
		{
			if(Menu->entries[Menu->numberEntries - 1]->description)
			{
				SpriteShutdown(Menu->entries[Menu->numberEntries - 1]->description);
				deleteSprite(Menu->entries[Menu->numberEntries - 1]->description);

				free(Menu->entries[--Menu->numberEntries]);
				Menu->entries[Menu->numberEntries]->description = 0;
			}
		}
	}
}


//============================= OPERATIONS ===================================

void MenuAddFunction(MenuClass* Menu, char* description[], void (*function)())
{
	MenuEntry* Entry;
	Entry = (MenuEntry*) malloc(sizeof(MenuEntry));
	Entry->description = newSprite();
	SpriteInitializeXPM(Entry->description, description);
	Entry->SubMenu = 0;
	Entry->function = function;
	Menu->entries[Menu->numberEntries++] = Entry;
	MenuAdjust(Menu);
}
void MenuAddMenu(MenuClass* Menu, char* description[], MenuClass* SubMenu)
{
	MenuEntry* Entry;
	Entry = (MenuEntry*) malloc(sizeof(MenuEntry));
	Entry->description = newSprite();
	SpriteInitializeXPM(Entry->description, description);
	Entry->SubMenu = SubMenu;
	Entry->function = 0;
	Menu->entries[Menu->numberEntries++] = Entry;
	MenuAdjust(Menu);
}
bool MenuRun(MenuClass* Menu)
{
	char entry;

	entry = MenuSelectEntry(Menu);
	if(entry == Menu->numberEntries)
		return false;

	if(entry == -1)
		return true;

	if(Menu->entries[entry - 1]->function != 0)
	{
		(*(Menu->entries[entry - 1]->function))(Menu->GameLogic);
	}
	else
		MenuRun(Menu->entries[entry - 1]->SubMenu);

	return true;
}
void MenuPrint(MenuClass* Menu)
{
	Coord center;
	center.x = GraphicsGetXResolution(Menu->Graphics) / 3;
	center.y = GraphicsGetYResolution(Menu->Graphics) / 3;

	unsigned long i;
	for(i = 0; i < Menu->numberEntries; i++)
	{
		SpriteSetPosition(Menu->entries[i]->description, center);
		GraphicsDrawSprite(Menu->Graphics, Menu->entries[i]->description);
		center.y += SpriteGetDimensions(Menu->entries[i]->description).y;
	}

	GraphicsFrame(Menu->Graphics);
}


/////////////////////////////// PRIVATE //////////////////////////////////////

//============================= OPERATIONS ===================================

static void MenuAdjust(MenuClass* Menu)
{
	if(Menu->numberEntries == Menu->size)
		Menu->entries = (MenuEntry**) realloc(Menu->entries, sizeof(MenuEntry*) * ++Menu->size);
}
static int MenuSelectEntry(MenuClass* Menu)
{
	char answer;
	kb_key entry;
	ListClass* kbEvents;
	kbEvents = InputGetKBEvents(Menu->Input);

	// Wait for user key press
	if(ListIsEmpty(kbEvents))
		return -1;

	entry = *(kb_key* ) ListFront(kbEvents);
	ListPopFront(kbEvents);

	if(entry > kb_9)
		return -1;

	if(entry == kb_esc)
		return Menu->numberEntries;

	// Entry validation
	entry -= 1;
	if(entry > Menu->numberEntries)
		return -1;

	return entry;
}

void runStart(struct GameLogicClass* GameLogic)
{
	GameLogicEnableMenus((GameLogicClass*) GameLogic, false);
}
