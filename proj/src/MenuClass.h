#ifndef _MENU_CLASS_H_
#define _MENU_CLASS_H_

#include "InputClass.h"
#include "GraphicsClass.h"

typedef struct menu_class MenuClass;
struct GameLogicClass;

MenuClass* newMenu();
void deleteMenu(MenuClass* Menu);

bool MenuInitialize(MenuClass* Menu, char* title[], GraphicsClass* Graphics, InputClass* Input, struct GameLogicClass* GameLogic);
void MenuShutdown(MenuClass* Menu);

void MenuAddFunction(MenuClass* Menu, char* description[], void (*function)());
void MenuAddMenu(MenuClass* Menu, char* description[], MenuClass* SubMenu);
bool MenuRun(MenuClass* Menu);
void MenuPrint(MenuClass* Menu);

static void MenuAdjust(MenuClass* Menu);
static int MenuSelectEntry(MenuClass* Menu);

void runStart(struct GameLogicClass* GameLogic);

#endif
