#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "game.h"
#include "gameUI.h"

#define NUM_GAME_BUTTONS (NUM_GAMES + 1)

typedef struct {
	Button gameButtons[NUM_GAME_BUTTONS];
} MainMenu;

// declare functions
void initMainMenu(Game* game, void** stateData);
void updateMainMenu(Game* game, void* state);
void renderMainMenu(Game* game, void* state);
void exitMainMenu(Game* game, void* stateData);

#endif
