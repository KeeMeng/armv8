#ifndef GAME_H
#define GAME_H

#include "window.h"
#include "renderer.h"
#include "utilities.h"

enum GameState { GAME_MENU, GAME_PLAY, GAME_OVER };

#define NUM_GAMES 4

enum Games {
	MAIN_MENU = 0,
	MINOTAUR_GAME,
	PONG_GAME,
	SNAKE_GAME,
	SNL_GAME,
};

struct Game;
typedef struct Game Game;

typedef struct {
	char* name;
	void (*initFunc)(Game* game, void** state);
	void (*updateFunc)(Game* game, void* state);
	void (*renderFunc)(Game* game, void* state);
	void (*exitFunc)(Game* game, void* state);
	void (*startFunc)(Game* game);
} GameCartridge;

#define NUM_KEYS 348

struct Game {
	Window window;
	Renderer renderer;
	enum Games currentGame;
	GameCartridge cartridges[NUM_GAMES + 1];
	void* stateData[NUM_GAMES + 1];
	double startTime;
	double endTime;
	double deltaTime;
	bool keysDown[NUM_KEYS];
	double keyTime[NUM_KEYS];
	bool stateTransition;
	bool mousePressed;
	float mouseTimer;
	double mouseX;
	double mouseY;
};

// declare functions
void changeState(Game* game, enum Games nextGame);
void initialiseGame(Game* game);
void gameLoop(Game* game);
void runRenderer(Game* game);

#endif
