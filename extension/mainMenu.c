#include "mainMenu.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void quitGame(Game* game) {
	game->window.running = false;
}

void initMainMenu(Game* game, void** stateData) {
	// dynamically allocate menu
	SAFE_MALLOC(*stateData, sizeof(MainMenu));

	MainMenu* m = *stateData;

	// set all
	for (int i = 0; i < NUM_GAME_BUTTONS; i++) {
		m->gameButtons[i].alive = false;
	}

	// initialise
	// skip main menu
	for (int i = 0; i < NUM_GAME_BUTTONS - 1; i++) {
		if (game->cartridges[i + 1].startFunc != NULL) {
			initButton(&m->gameButtons[i], game->cartridges[i + 1].name, game->cartridges[i + 1].startFunc, INITIAL_WIDTH / 2, 450 - 100 * i, 512, 64, 28, 36);
		}
	}
	// add quit
	initButton(&m->gameButtons[NUM_GAME_BUTTONS - 1], "Quit", &quitGame, INITIAL_WIDTH / 2, 50, 512, 64, 28, 36);
}

void updateMainMenu(Game* game, void* state) {
	MainMenu* m = state;

	// dynamically set keybind
	for (int i = 0; i < NUM_GAME_BUTTONS; i++) {
		if (game->keysDown['A' + i - 1] || game->keysDown['1' + i - 1]) {
			changeState(game, i);
		}
	}
	if (game->keysDown[GLFW_KEY_Q] || game->keysDown[GLFW_KEY_ESCAPE] || game->keysDown['1' + NUM_GAME_BUTTONS - 1]) {
		quitGame(game);
	}

	// update buttons
	for (int i = 0; i < NUM_GAME_BUTTONS; i++) {
		if (game->currentGame == MAIN_MENU && !game->stateTransition && m->gameButtons[i].alive) {
			updateButton(game, &m->gameButtons[i]);
		}
	}
}

void renderMainMenu(Game* game, void* state) {
	MainMenu* m = game->stateData[MAIN_MENU];

	// render all options
	renderStringCentered(&game->renderer, WINDOW_TITLE, 515, 54);

	// render buttons
	for (int i = 0; i < NUM_GAME_BUTTONS; i++) {
		if (game->currentGame == MAIN_MENU && m->gameButtons[i].alive) {
			renderButton(&game->renderer, &m->gameButtons[i]);
		}
	}
}

void exitMainMenu(Game* game, void* stateData) {
	// free main menu
	free(stateData);
}
