#include "game.h"
#include "mainMenu.h"
#include "./games/minotaurs.h"
#include "./games/pong.h"
#include "./games/snake.h"
#include "./games/snakesAndLadders.h"
#include <assert.h>
#include <stdio.h>

void startMinotaurs(Game* game) { changeState(game, MINOTAUR_GAME); }
void startPong(Game* game) { changeState(game, PONG_GAME); }
void startSnake(Game* game) { changeState(game, SNAKE_GAME); }
void startSNL(Game* game) { changeState(game, SNL_GAME); }

static void buildStateTables(Game* game) {
	// main menu
	game->cartridges[MAIN_MENU] = (GameCartridge) {
		.name = "Main Menu",
		.initFunc = &initMainMenu,
		.updateFunc = &updateMainMenu,
		.renderFunc = &renderMainMenu,
		.exitFunc = &exitMainMenu,
		.startFunc = NULL,
	};

	// minotaurs game
	game->cartridges[MINOTAUR_GAME] = (GameCartridge) {
		.name = "Minotaurs",
		.initFunc = &initMinotaurs,
		.updateFunc = &updateMinotaurs,
		.renderFunc = &renderMinotaurs,
		.exitFunc = &exitMinotaurs,
		.startFunc = &startMinotaurs
	};

	// pong game
	game->cartridges[PONG_GAME] = (GameCartridge) {
		.name = "Pong",
		.initFunc = &initPong,
		.updateFunc = &updatePong,
		.renderFunc = &renderPong,
		.exitFunc = &exitPong,
		.startFunc = &startPong
	};

	// snake game
	game->cartridges[SNAKE_GAME] = (GameCartridge) {
		.name = "Snake",
		.initFunc = &initSnake,
		.updateFunc = &updateSnake,
		.renderFunc = &renderSnake,
		.exitFunc = &exitSnake,
		.startFunc = &startSnake
	};

	// snakes and ladders game
	game->cartridges[SNL_GAME] = (GameCartridge) {
		.name = "Snakes and Ladders",
		.initFunc = &initSNL_RPG,
		.updateFunc = &updateSNL_RPG,
		.renderFunc = &renderSNL_RPG,
		.exitFunc = &exitSNL_RPG,
		.startFunc = &startSNL,
	};
}

void changeState(Game* game, enum Games nextGame) {
	assert(nextGame <= NUM_GAMES);
	assert(nextGame >= 0);

	// set state transition to true
	game->stateTransition = true;

	// call exit function
	if (game->cartridges[game->currentGame].exitFunc != NULL) {
		(*game->cartridges[game->currentGame].exitFunc)(game, game->stateData[game->currentGame]);
	}

	// set new state
	game->currentGame = nextGame;

	if (game->cartridges[nextGame].initFunc != NULL) {
		(*game->cartridges[nextGame].initFunc)(game, &game->stateData[nextGame]);
	}
}

// declare functions
void initialiseGame(Game* game) {
	// initialise window
	initialiseWindow(&game->window);
	initialiseRenderer(&game->renderer);

	// intialise renderer

	// build the state tables
	buildStateTables(game);

	// set state
	game->currentGame = MAIN_MENU;
	(*game->cartridges[MAIN_MENU].initFunc)(game, &game->stateData[MAIN_MENU]);

	game->stateTransition = true;

	// start game loop
	gameLoop(game);
}

void handleEvents(Game* game) {
	// handle events in window event queue
	pollWindowEvents(&game->window);

	// set booleans
	for (int i = 0; i < NUM_KEYS; i++) {
		if (glfwGetKey(game->window.windowHandle, i) == GLFW_PRESS) {
			// key pressed
			game->keysDown[i] = true;
		} else {
			// key not pressed
			game->keysDown[i] = false;
		}
	}

	// check button
	if (glfwGetMouseButton(game->window.windowHandle, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		game->mousePressed = true;
	} else {
		game->mousePressed = false;
	}

	// get mouse coordinates
	glfwGetCursorPos(game->window.windowHandle, &game->mouseX, &game->mouseY);
	game->mouseY = INITIAL_HEIGHT - game->mouseY;
}

void gameLoop(Game* game) {
	while (game->window.running) {
		// get start time
		game->startTime = glfwGetTime();

		// handle events
		handleEvents(game);

		// update
		if (!game->stateTransition && game->cartridges[game->currentGame].updateFunc != NULL) {
			(*game->cartridges[game->currentGame].updateFunc)(game, game->stateData[game->currentGame]);
		}

		// universal menu button
		if (game->keysDown[GLFW_KEY_M] && game->keyTime[GLFW_KEY_M] == 0.0) {
			changeState(game, MAIN_MENU);
		}

		// run renderer
		if (!game->stateTransition) {
			runRenderer(game);
		}

		// update mouse timer
		if (game->mousePressed) {
			game->mouseTimer += game->deltaTime;
		} else {
			game->mouseTimer = 0;
		}

		// update key timers
		for (int i = 0; i < NUM_KEYS; i++) {
			if (glfwGetKey(game->window.windowHandle, i) == GLFW_PRESS) {
				// key pressed
				game->keyTime[i] += game->deltaTime;
			} else {
				// key not pressed
				game->keyTime[i] = 0.0;
			}
		}

		// get end time
		game->endTime = glfwGetTime();

		// compute delta time
		game->deltaTime = game->endTime - game->startTime;

		if (game->stateTransition) {
			// transition state
			game->stateTransition = false;
		}
	}

	// free the current state
	if (game->cartridges[game->currentGame].exitFunc != NULL) {
		// free
		(*game->cartridges[game->currentGame].exitFunc)(game, game->stateData[game->currentGame]);
	}

	// free font image
	freeRenderer(&game->renderer);

	// free image
	freeWindow(&game->window);
}

void runRenderer(Game* game) {
	// clear screen
	glClearColor(0.0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	// render function
	if (game->cartridges[game->currentGame].renderFunc != NULL) {
		(*game->cartridges[game->currentGame].renderFunc)(game, game->stateData[game->currentGame]);
	}

	// present render buffer
	glfwSwapBuffers(game->window.windowHandle);
}
