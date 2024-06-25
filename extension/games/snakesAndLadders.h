#ifndef SNAKE_LADDER_H
#define SNAKE_LADDER_H

#include "./../game.h"

#define SNL_BOARD_DIM 10
#define SNL_BOARD_AREA (SNL_BOARD_DIM * SNL_BOARD_DIM)
#define SNL_SL_NUM (SNL_BOARD_DIM + 2)

enum cellType { NONE, SNAKE, LADDER, OBSTACLE };

typedef struct {
	int squareNo;
	float delayTime;
	Vec2f size;
	Sprite sprite;
} SnLPlayer;

typedef struct {
	int beginning;
	int end;
} SnakeLadder;

typedef struct {
	enum cellType type;
	SnakeLadder skip;
} BoardObject;

typedef struct {
	int number;
	BoardObject object;
} BoardCell;

typedef struct {
	int number;
	float cooldown;
	Sprite spite;
} Dice;

typedef struct {
	enum GameState gameState;
	bool turn;
	bool dicePressed;
	bool turnPassed;
	Dice dice;
	SnLPlayer player1;
	SnLPlayer player2;
	SnakeLadder snakes[SNL_SL_NUM];
	SnakeLadder ladders[SNL_SL_NUM];
	BoardCell board[SNL_BOARD_AREA];
	Texture diceTexture;
	Texture ladderTexture;
	Texture playerTexture;
	Texture snakeTexture;
	Texture tile1Texture;
	Texture tile2Texture;
} SNLState;

extern void initSNL_RPG(Game* game, void** state);
extern void updateSNL_RPG(Game* game, void* state);
extern void renderSNL_RPG(Game* game, void* state);
extern void exitSNL_RPG(Game* game, void* state);

#endif
